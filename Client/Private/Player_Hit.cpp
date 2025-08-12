#include "stdafx.h"
#include "Player_StateMachine.h"
#include "Player_Hit.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "ShootingUI.h"
CPlayer_Hit::CPlayer_Hit()
{
}

HRESULT CPlayer_Hit::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Hit::State_Enter(_uint* pState)
{
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_STUN;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = false;
    static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingReload"))->Set_Open(false);
    static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingShoot"))->Set_Open(false);
    __super::State_Enter(pState);
}

_bool CPlayer_Hit::State_Processing(_float fTimedelta, _uint* pState)
{
    __super::State_Processing(fTimedelta, pState);
    return true;
}

_bool CPlayer_Hit::State_Exit(_uint* pState)
{
    m_pParentObject->Set_hit();
    SetActive(false, pState);
    return true;
}

void CPlayer_Hit::Init_CallBack_Func()
{

}

_bool CPlayer_Hit::IsActive(_uint stateFlags) const
{
    return (stateFlags & MOV_HIT) != 0;
}

void CPlayer_Hit::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= MOV_HIT; // hit 플래그 켜기
    else
        *pState &= ~MOV_HIT; // hit 플래그 끄기
}

_bool CPlayer_Hit::CanEnter(_uint* pState)
{
    if (*pState & BEH_SHOOT)
        return false;

    if (*pState & MOV_HIT)
        return false;
        
    return m_pParentObject->get_hit();
}

_bool CPlayer_Hit::CheckInputCondition(_uint stateFlags)
{
    return true;
}

CPlayer_Hit* CPlayer_Hit::Create(void* pArg)
{
    CPlayer_Hit* pInstance = new CPlayer_Hit();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Hit::Free()
{
	__super::Free();
}
