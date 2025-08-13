#include "stdafx.h"
#include "Player_Shoot.h"
#include "Player_StateMachine.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "ShootingUI.h"

CPlayer_Shoot::CPlayer_Shoot()
{
}

HRESULT CPlayer_Shoot::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

    CShootingUI::CShootingUI_DESC SDesc{};
    SDesc.iWeaPonTYPE = m_pCurWeapon;
    SDesc.iWeaPonState = CWeapon::WS_SHOOT;
    SDesc.fX = g_iWinSizeX * 0.7f;
    SDesc.fY = g_iWinSizeY * 0.57f;
    SDesc.fZ = 0.001f;
    SDesc.fSizeX = 100.f;
    SDesc.fSizeY = 100.f;
    SDesc.UID = UIID_PlayerShooting;
    SDesc.Update = false;
    m_pGameInstance->Add_UI_To_CLone(L"ShootingShoot", L"Prototype GameObject_ShootingUI", &SDesc);
    m_pShootingUI = static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingShoot"));
    Safe_AddRef(m_pShootingUI);

    if (FAILED(UI_CallBack()))
        return S_OK;

	return S_OK;
}

void CPlayer_Shoot::State_Enter(_uint* pState)
{
    static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingReload"))->Set_Open(false);
    static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingShoot"))->Set_Open(false);

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
    
    //*pState &= ~MOV_HIT;

    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CurMotion;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_SHOOT;
    m_StateDesc.bLoop = m_bAutoFire;

    __super::State_Enter(pState);
}

_bool CPlayer_Shoot::State_Processing(_float fTimedelta, _uint* pState)
{
    if (m_pParentObject->Get_Weapon_Info().iCurBullet <= 0)
        return true;

    if (false == m_bAutoFire) // 단발
    {
        // 단발 모드에서는 애니메이션이 끝나면 발사 종료
        if (__super::State_Processing(fTimedelta, pState))
        {
            return true;
        }
        return false;
    }

    if (m_bAutoFire) // 연사
    {
        // 마우스를 떼면 연사 종료
        if (m_pGameInstance->Get_DIMouseUp(MOUSEKEYSTATE::DIM_LB))
        {
            return true;
        }
        else
        {
            m_fLastFireTime += fTimedelta;
      
            // 발사 속도에 따라 새로운 발사
            if (m_fLastFireTime >= m_pParentObject->Get_Weapon_Info().fFireRate)
            {
                m_fLastFireTime = 0.f;
                // 새로운 발사 애니메이션 시작
                __super::State_Enter(pState);
            }

            // 애니메이션 재생
            __super::State_Processing(fTimedelta, pState);
        }
    }

    return false;
}

_bool CPlayer_Shoot::State_Exit(_uint* pState)
{
    m_pShootingUI->Set_Open(false);

    m_bAutoFire = false;
    m_fLastFireTime = 0.f;
   
 
    if ((*pState & MOV_SPRINT) != 0)
    {
        m_pParentObject->Set_PartObj_Set_Anim(CPlayer::PART_BODY, CBody_Player::BODY_SPRINT, true);
    }
    else if ((*pState & MOV_RUN) != 0)
    {
        m_pParentObject->Set_PartObj_Set_Anim(CPlayer::PART_BODY, CBody_Player::BODY_RUN, true);
    }
    else if ((*pState & MOV_JUMP) != 0)
    {
        m_pParentObject->Set_PartObj_Set_Anim(CPlayer::PART_BODY, CBody_Player::BODY_JUMP_RUN_LOOP, true);
    }
    else
    {
        m_pParentObject->Set_PartObj_Set_Anim(CPlayer::PART_BODY, CBody_Player::BODY_IDLE, true);
    }
    
    m_pParentObject->Set_PartObj_Set_Anim(CPlayer::PART_WEAPON, CWeapon::WS_IDLE, true);
    
    return true;
}

void CPlayer_Shoot::Init_CallBack_Func()
{
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_SHOOT, 3,[this]()
                                    {
                                      m_pGameInstance->Play_Sound(L"ST_Handgun_PF_Shoot.ogg", CSound::SOUND_BGM, 1.f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::AssaultRifle, CWeapon::WS_SHOOT, 3, [this]()
                                    {
                                      m_pGameInstance->Play_Sound(L"ST_AssaultRifle_PF_Shoot.ogg", CSound::SOUND_BGM, 1.f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::MissileGatling, CWeapon::WS_SHOOT, 3, [this]()
                                    { 
                                      m_pGameInstance->Play_Sound(L"ST_MissileGatling_PF_Shoot.ogg", CSound::SOUND_BGM, 1.f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::HeavyCrossbow, CWeapon::WS_SHOOT, 3, [this]()
                                    { 
                                      m_pGameInstance->Play_Sound(L"ST_HeavyCrossbow_SF_Shoot_A.ogg", CSound::SOUND_BGM, 1.f);
                                    });
}

_bool CPlayer_Shoot::IsActive(_uint stateFlags) const
{
    return (stateFlags & BEH_SHOOT) != 0;
}

void CPlayer_Shoot::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= BEH_SHOOT;
    else
        *pState &= ~BEH_SHOOT; 
}

_bool CPlayer_Shoot::CanEnter(_uint* pState) 
{
    if ((*pState & (BEH_SHOOT | BEH_RELOAD | MOV_STURN)) != 0)
        return false;

    // 탄약이 없으면 발사 불가
    if (m_pParentObject->Get_Weapon_Info().iCurBullet <= 0)
        return false;


    // 마우스 좌클릭 입력 처리
    if (m_pGameInstance->Get_DIMouseDown(MOUSEKEYSTATE::DIM_LB))
    {
        // 단발 
        m_bAutoFire = false;
        return true;
    }
    
    if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB))
    {
        // 연사 
        m_bAutoFire = true;
        return true;
    }
    
    return false;
}

_bool CPlayer_Shoot::CheckInputCondition(_uint stateFlags) 
{
    return true;
}

HRESULT CPlayer_Shoot::UI_CallBack()
{   
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_SHOOT, 4,[this]()
                                    {  
                                      m_pShootingUI->Set_RandomPos(true, false, false);
                                      m_pShootingUI->Set_Open(true);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon, 0.2f, 0.2f, 0.2f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerHP, 0.2f, -0.2f, 0.2f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon_Aim, 0.2f, 0.2f,-0.2f);
                                      m_pGameInstance->Set_UI_shaking(UIID_BossHP, 0.2f, 0.2f,-0.2f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_SHOOT, 15,[this](){m_pShootingUI->Set_Open(false);});

    m_pParentObject->WeaponCallBack(CWeapon::AssaultRifle, CWeapon::WS_SHOOT, 4,[this]()
                                    {
                                      m_pShootingUI->Set_RandomPos(true, false, false);
                                      m_pShootingUI->Set_Open(true);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon, 0.2f, 0.3f, 0.3f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerHP, 0.2f, -0.3f, 0.3f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon_Aim, 0.2f, 0.3f,-0.3f);
                                      m_pGameInstance->Set_UI_shaking(UIID_BossHP, 0.2f, 0.3f,-0.3f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::AssaultRifle, CWeapon::WS_SHOOT, 15,[this](){m_pShootingUI->Set_Open(false);});

    m_pParentObject->WeaponCallBack(CWeapon::MissileGatling, CWeapon::WS_SHOOT, 4,[this]()
                                    {
                                      m_pShootingUI->Set_RandomPos(true, false, false);
                                      m_pShootingUI->Set_Open(true);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon, 0.2f, 0.6f, 0.6f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerHP, 0.2f, -0.6f, 0.6f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon_Aim, 0.2f, 0.6f,-0.6f);
                                      m_pGameInstance->Set_UI_shaking(UIID_BossHP, 0.2f, 0.6f,-0.6f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::MissileGatling, CWeapon::WS_SHOOT, 15,[this]() { m_pShootingUI->Set_Open(false); });

    m_pParentObject->WeaponCallBack(CWeapon::HeavyCrossbow, CWeapon::WS_SHOOT, 4,[this]()
                                    {
                                      m_pShootingUI->Set_RandomPos(true, false, false);
                                      m_pShootingUI->Set_Open(true);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon, 0.2f, 1.f, 1.f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerHP, 0.2f, -1.f, 1.f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon_Aim, 0.2f, 1.f,-1.f); 
                                      m_pGameInstance->Set_UI_shaking(UIID_BossHP, 0.2f, 1.f,-1.f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::HeavyCrossbow, CWeapon::WS_SHOOT, 15,[this]() { m_pShootingUI->Set_Open(false); });

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
}
