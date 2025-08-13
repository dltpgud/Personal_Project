#include "stdafx.h"
#include "Player_Reload.h"
#include "Player_StateMachine.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "ShootingUI.h"
CPlayer_Reload::CPlayer_Reload()
{
}

HRESULT CPlayer_Reload::Initialize(void* pDesc)
{
    PLAYER_INFO_DESC* Desc = static_cast<PLAYER_INFO_DESC*>(pDesc);
    if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

    CShootingUI::CShootingUI_DESC SDesc{};
    SDesc.iWeaPonTYPE = m_pCurWeapon;
    SDesc.iWeaPonState = CWeapon::WS_RELOAD;
    SDesc.fX = g_iWinSizeX * 0.7f;
    SDesc.fY = g_iWinSizeY * 0.57f;
    SDesc.fZ = 0.001f;
    SDesc.fSizeX = 100.f;
    SDesc.fSizeY = 100.f;
    SDesc.UID = UIID_PlayerShooting;
    SDesc.Update = false;
    m_pGameInstance->Add_UI_To_CLone(L"ShootingReload", L"Prototype GameObject_ShootingUI", &SDesc);
    m_pShootingUI = static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingReload"));
    Safe_AddRef(m_pShootingUI);

    if (FAILED(UI_CallBack()))
        return E_FAIL;

	return S_OK;
}

void CPlayer_Reload::State_Enter(_uint* pState)
{
    static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingReload"))->Set_Open(false);
    static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingShoot"))->Set_Open(false);

    *pState &= ~MOV_HIT;

    _int CurMotion{};

    switch (*m_pCurWeapon)
    {
    case CWeapon::HendGun: 
        CurMotion = CBody_Player::BODY_HENDGUN_RELOAD;
        m_StateDesc.fPlayTime = 1.2f;
        break;
    case CWeapon::AssaultRifle: 
        CurMotion = CBody_Player::BODY_ASSAULTRIFlLE_RELOAD;
        m_StateDesc.fPlayTime = 1.2f;
        break;
    case CWeapon::MissileGatling: 
        CurMotion = CBody_Player::BODY_MissileGatling_RELOAD;
        m_StateDesc.fPlayTime = 1.2f;
        break;
    case CWeapon::HeavyCrossbow: 
        CurMotion = CBody_Player::BODY_HEAVYCROSSBOW_RELOAD;
        m_StateDesc.fPlayTime = 1.f;
        break;
    default: break;
    }

    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CurMotion;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_RELOAD;

    __super::State_Enter(pState);
}

_bool CPlayer_Reload::State_Processing(_float fTimedelta, _uint* pState)
{
    return __super:: State_Processing( fTimedelta,pState);
}

_bool CPlayer_Reload::State_Exit(_uint* pState)
{
    m_pShootingUI->Set_Open(false); 
   return true;
}

void CPlayer_Reload::Init_CallBack_Func()
{
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_RELOAD, 15, [this]()
                                    {m_pGameInstance->Play_Sound(L"ST_Handgun_Reload.ogg", CSound::SOUND_BGM, 1.f);});
    m_pParentObject->WeaponCallBack(CWeapon::AssaultRifle, CWeapon::WS_RELOAD, 19, [this]()
                                    { m_pGameInstance->Play_Sound(L"ST_AssaultRifle_Reload.ogg", CSound::SOUND_BGM, 1.f); });
    m_pParentObject->WeaponCallBack(CWeapon::MissileGatling, CWeapon::WS_RELOAD, 19, [this]()
                                    { m_pGameInstance->Play_Sound(L"ST_MissileGatling_Reload.ogg", CSound::SOUND_BGM, 1.f); });
    m_pParentObject->WeaponCallBack(CWeapon::HeavyCrossbow, CWeapon::WS_RELOAD, 30, [this]()
                                    { m_pGameInstance->Play_Sound(L"ST_HeavyCrossbow_Reload.ogg", CSound::SOUND_BGM, 1.f); });    
}

_bool CPlayer_Reload::IsActive(_uint stateFlags) const
{
    return (stateFlags & BEH_RELOAD) != 0;
}

void CPlayer_Reload::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= BEH_RELOAD; 
    else
        *pState &= ~BEH_RELOAD; 
}

_bool CPlayer_Reload::CanEnter(_uint* pState) 
{   
     if (*pState & (BEH_SHOOT | MOV_JUMP | BEH_RELOAD | MOV_STURN))
         return false;

    _bool bkey = m_pGameInstance->Get_DIKeyDown(DIK_R);
  
    _bool IsBullet = 0 >= m_pParentObject->Get_Weapon_Info().iCurBullet;
     
    return bkey || IsBullet;
}

_bool CPlayer_Reload::CheckInputCondition(_uint stateFlags) 
{
    return true;
}

HRESULT CPlayer_Reload::UI_CallBack()
{
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_RELOAD, 20,[this]()
                                    {
                                        m_pShootingUI->Set_RandomPos(false, true, false);
                                        m_pShootingUI->Set_Open(true);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_RELOAD, 40,[this]()
                                    {
                                        m_pShootingUI->Set_RandomPos(false, false, false);
                                        m_pShootingUI->Set_PosClack(-200.f, 80.f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::HendGun, CWeapon::WS_RELOAD, 50, [this]() { m_pShootingUI->Set_Open(false); });


    m_pParentObject->WeaponCallBack(CWeapon::AssaultRifle, CWeapon::WS_RELOAD, 20,[this]()
                                    {
                                        m_pShootingUI->Set_RandomPos(false, true, false);
                                        m_pShootingUI->Set_Open(true);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::AssaultRifle, CWeapon::WS_RELOAD, 78,[this]()
                                    {
                                        m_pShootingUI->Set_RandomPos(false, false, false);
                                        m_pShootingUI->Set_PosClack(-250.f, 60.f);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::AssaultRifle, CWeapon::WS_RELOAD, 88, [this]() { m_pShootingUI->Set_Open(false); });

    m_pParentObject->WeaponCallBack(CWeapon::MissileGatling, CWeapon::WS_RELOAD, 25,[this]()
                                    {
                                        m_pShootingUI->Set_RandomPos(false, true, false);
                                        m_pShootingUI->Set_Open(true);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::MissileGatling, CWeapon::WS_RELOAD, 53, [this]() { m_pShootingUI->Set_Open(false); });

    m_pParentObject->WeaponCallBack(CWeapon::HeavyCrossbow, CWeapon::WS_RELOAD, 25,[this]()
                                    {
                                        m_pShootingUI->Set_RandomPos(false, true, false);
                                        m_pShootingUI->Set_Open(true);
                                    });
    m_pParentObject->WeaponCallBack(CWeapon::HeavyCrossbow, CWeapon::WS_RELOAD, 53, [this]() { m_pShootingUI->Set_Open(false); });

    return S_OK;
}

CPlayer_Reload* CPlayer_Reload::Create(void* pArg)
{
    CPlayer_Reload* pInstance = new CPlayer_Reload();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Reload");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Reload::Free()
{
	__super::Free();
    Safe_Release(m_pShootingUI);
}
