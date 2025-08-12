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
    // hit 플래그가 켜져 있으면 끄기
    *pState &= ~MOV_HIT;
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CurMotion;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_SHOOT;

    __super::State_Enter(pState);
}

_bool CPlayer_Shoot::State_Processing(_float fTimedelta, _uint* pState)
{
    // 애니메이션 재생
    if (__super::State_Processing(fTimedelta, pState))
    {
        // 애니메이션이 끝나면 발사 종료
        return true;
    }

    // 탄약이 없으면 발사 종료
    if (m_pParentObject->Get_Bullet() <= 0)
        return true;

    // 애니메이션 재생 중
    return false;
}

_bool CPlayer_Shoot::State_Exit(_uint* pState)
{
    m_pShootingUI->Set_Open(false);
    SetActive(false, pState);

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
    // 이미 발사 중이면 진입 불가
    if ((*pState & BEH_SHOOT) != 0)
        return false;

    // 장전 중이면 발사 불가
    if ((*pState & BEH_RELOAD) != 0)
        return false;

        if ((*pState & MOV_JUMP) != 0)
        return false;

    // 마우스 좌클릭을 한 번 누르면 바로 발사 상태로 진입
    if (m_pGameInstance->Get_DIMouseDown(MOUSEKEYSTATE::DIM_LB))
    {
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
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_SHOOT, 2,[this]()
                                    {  
                                      m_pShootingUI->Set_RandomPos(true, false, false);
                                      m_pShootingUI->Set_Open(true);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon, 0.2f, 0.2f, 0.2f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerHP, 0.2f, -0.2f, 0.2f);
                                      m_pGameInstance->Set_UI_shaking(UIID_PlayerWeaPon_Aim, 0.2f, 0.2f,-0.2f);
                                      m_pGameInstance->Set_UI_shaking(UIID_BossHP, 0.2f, 0.2f,-0.2f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_SHOOT, 10,[this](){m_pShootingUI->Set_Open(false);});

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
