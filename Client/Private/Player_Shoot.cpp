#include "stdafx.h"
#include "Player_Shoot.h"
#include "Player_StateMachine.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "Player_ShootingStateUI.h "

CPlayer_Shoot::CPlayer_Shoot()
{
}

HRESULT CPlayer_Shoot::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

    m_pShootingUI = static_cast<CPlayer_ShootingStateUI*>(m_pGameInstance->Find_Clone_UIObj(L"Player_ShootingUI"));
    Safe_AddRef(m_pShootingUI);
    m_pAutoShootingUI = static_cast<CPlayer_ShootingStateUI*>(m_pGameInstance->Find_Clone_UIObj(L"Player_AutoShootingUI"));
    Safe_AddRef(m_pAutoShootingUI);

    if (FAILED(UI_CallBack()))
        return S_OK;

	return S_OK;
}

void CPlayer_Shoot::State_Enter(_uint* pState, _uint* pPreState)
{
    _int CurMotion{};
    switch (*m_pCurWeapon)
    {
    case CWeapon::HendGun: 
        CurMotion = CBody_Player::BODY_HENDGUN_SHOOT;
        break;
    case CWeapon::AssaultRifle: 
        CurMotion = CBody_Player::BODY_ASSAULTRIFlLE_SHOOT;
        break;
    case CWeapon::MissileGatling: 
        CurMotion = CBody_Player::BODY_MissileGatling_SHOOT;
        break;
    case CWeapon::HeavyCrossbow: 
        CurMotion = CBody_Player::BODY_HEAVYCROSSBOW_SHOOT;
        break;
    default: break;
    }
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CurMotion;

    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_SHOOT;
    m_StateDesc.bLoop = m_bAutoFire;

    __super::State_Enter(pState, pPreState);
}

_bool CPlayer_Shoot::State_Processing(_float fTimedelta, _uint* pState, _uint* pPreState)
{
    if (m_pParentObject->Get_Weapon_Info().iCurBullet <= 0)
        return true;

    if (false == m_bAutoFire) // 단발
    {
        if (__super::State_Processing(fTimedelta, pState, pPreState))
        {
            return true;
        }
        return false;
    }
    
    if (m_bAutoFire) // 연사
    {
        if (m_pGameInstance->Get_DIMouseUp(MOUSEKEYSTATE::DIM_LB))
        {
            m_bAutoFire = false;
            m_fLastFireTime = 0.f;

            return true;
        }
        else
        {
            m_fLastFireTime += fTimedelta;
      
            if (m_fLastFireTime >= m_pParentObject->Get_Weapon_Info().fFireRate)
            {
                m_fLastFireTime = 0.f;
              
                __super::State_Enter(pState, pPreState);
            }
   
            __super::State_Processing(fTimedelta, pState, pPreState);
        }
    }
    
    return false;
}

_bool CPlayer_Shoot::State_Exit(_uint* pState)
{
    m_bAutoFire = false;
    m_fLastFireTime = 0.f;
    m_pShootingUI->Set_Open(false);
    m_pAutoShootingUI->Set_Open(false);
    if (!m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB))
    {
        return true;
    }
    
    return true;
}

void CPlayer_Shoot::Init_CallBack_Func()
{
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_SHOOT, 3,[this]()
                                    {
                                      m_pGameInstance->Play_Sound(L"ST_Handgun_PF_Shoot.ogg",  &m_pChannel, 1.f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::AssaultRifle, CWeapon::WS_SHOOT, 3, [this]()
                                    { m_pGameInstance->Play_Sound(L"ST_AssaultRifle_PF_Shoot.ogg", &m_pChannel, 1.f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::MissileGatling, CWeapon::WS_SHOOT, 3, [this]()
                                    { 
                                      m_pGameInstance->Play_Sound(L"ST_MissileGatling_PF_Shoot.ogg", &m_pChannel, 1.f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::HeavyCrossbow, CWeapon::WS_SHOOT, 3, [this]()
                                    { 
                                      m_pGameInstance->Play_Sound(L"ST_HeavyCrossbow_SF_Shoot_A.ogg", &m_pChannel, 1.f);
                                    });
}

_bool CPlayer_Shoot::IsActive(_uint stateFlags) const
{
    return (stateFlags & CPlayer::BEH_SHOOT) != 0;
}

void CPlayer_Shoot::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= CPlayer::BEH_SHOOT;
    else
        *pState &= ~CPlayer::BEH_SHOOT;
}

_bool CPlayer_Shoot::CanEnter(_uint* pState) 
{
    if (*pState & CPlayer::MOV_FALL)
         return false;
         
     if ((*pState & (CPlayer::BEH_RELOAD | CPlayer::BEH_SHOOT | CPlayer::MOV_STURN | CPlayer::MOV_HIT)) != 0)
         return false;
    
    if (true == m_pParentObject->HasState(CPlayer::FLAG_KEYLOCK))
        return false;

    if (m_pParentObject->Get_Weapon_Info().iCurBullet <= 0 ||
        m_pParentObject->Get_Weapon_Info().iCurBullet > m_pParentObject->Get_Weapon_Info().iMaxBullet)
        return false;

    // 점프 중에도 총을 쏠 수 있도록 허용
    if (m_pGameInstance->Get_DIMouseDown(MOUSEKEYSTATE::DIM_LB))
    {
        m_bAutoFire = false;
        return true;
    }
    
    if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB))
    {
        m_bAutoFire = true;
        return true;
    }
    
    return false;
}

_bool CPlayer_Shoot::CheckInputCondition(_uint stateFlags) 
{
    if (stateFlags & (CPlayer::MOV_STURN | CPlayer::MOV_HIT))
    {
        m_pShootingUI->Set_Open(false);
        m_pAutoShootingUI->Set_Open(false);
        m_bAutoFire = false;
        m_fLastFireTime = 0.f;
        return false;
    }
    
    if (m_pGameInstance->Get_DIMouseUp(MOUSEKEYSTATE::DIM_LB))
    {
        m_bAutoFire = false;
        m_fLastFireTime = 0.f;
        m_pShootingUI->Set_Open(false);
        m_pAutoShootingUI->Set_Open(false);
        return false;
    }
    
    return true;
}

HRESULT CPlayer_Shoot::UI_CallBack()
{   
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_SHOOT, 3,[this]()
                                    {  
                                      m_pShootingUI->Set_RandomPos(true, false, false);
                                      m_pShootingUI->Set_Open(true);

                                      if (m_bAutoFire)
                                      {
                                          m_pAutoShootingUI->Set_Open(true);
                                          m_pAutoShootingUI->Set_RandomPos(true, false, false);
                                      }
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon, 0.2f, 0.2f, 0.2f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerHP, 0.2f, -0.2f, 0.2f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon_Aim, 0.2f, 0.2f,-0.2f);
                                      m_pGameInstance->Set_UI_shaking(UIID_BossHP, 0.2f, 0.2f,-0.2f);
                                    });

    m_pParentObject->WeaponCallBack(CWeapon::AssaultRifle, CWeapon::WS_SHOOT, 3,[this]()
                                    {
                                      m_pShootingUI->Set_RandomPos(true, false, false);
                                      m_pShootingUI->Set_Open(true);

                                      if (m_bAutoFire)
                                      {
                                          m_pAutoShootingUI->Set_Open(true);
                                          m_pAutoShootingUI->Set_RandomPos(true, false, false);
                                      }

                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon, 0.2f, 0.3f, 0.3f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerHP, 0.2f, -0.3f, 0.3f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon_Aim, 0.2f, 0.3f,-0.3f);
                                      m_pGameInstance->Set_UI_shaking(UIID_BossHP, 0.2f, 0.3f,-0.3f);
                                    });

    m_pParentObject->WeaponCallBack(CWeapon::MissileGatling, CWeapon::WS_SHOOT, 3,[this]()
                                    {
                                      m_pShootingUI->Set_RandomPos(true, false, false);
                                      m_pShootingUI->Set_Open(true);

                                      if (m_bAutoFire)
                                      {
                                          m_pAutoShootingUI->Set_Open(true);
                                          m_pAutoShootingUI->Set_RandomPos(true, false, false);
                                      }
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon, 0.2f, 0.6f, 0.6f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerHP, 0.2f, -0.6f, 0.6f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon_Aim, 0.2f, 0.6f,-0.6f);
                                      m_pGameInstance->Set_UI_shaking(UIID_BossHP, 0.2f, 0.6f,-0.6f);
                                    });

    m_pParentObject->WeaponCallBack(CWeapon::HeavyCrossbow, CWeapon::WS_SHOOT, 3,[this]()
                                    {
                                      m_pShootingUI->Set_RandomPos(true, false, false);
                                      m_pShootingUI->Set_Open(true);

                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon, 0.2f, 1.f, 1.f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerHP, 0.2f, -1.f, 1.f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon_Aim, 0.2f, 1.f,-1.f); 
                                      m_pGameInstance->Set_UI_shaking(UIID_BossHP, 0.2f, 1.f,-1.f);
                                    });

    return S_OK;
}

CPlayer_Shoot* CPlayer_Shoot::Create(void* pArg)
{
    CPlayer_Shoot* pInstance = new CPlayer_Shoot();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Shoot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Shoot::Free()
{
	__super::Free();
    Safe_Release(m_pShootingUI);
    Safe_Release(m_pAutoShootingUI);
}
