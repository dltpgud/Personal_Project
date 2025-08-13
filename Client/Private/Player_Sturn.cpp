#include "stdafx.h"
#include "Player_StateMachine.h"
#include "Player_Sturn.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "ShootingUI.h"
CPlayer_Sturn::CPlayer_Sturn()
{
}

HRESULT CPlayer_Sturn::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Sturn::State_Enter(_uint* pState)
{
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_COOLING_LONG;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;
    static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingReload"))->Set_Open(false);
    static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingShoot"))->Set_Open(false);
    m_fSturnTime = 2.f;
    __super::State_Enter(pState);
}

_bool CPlayer_Sturn::State_Processing(_float fTimedelta, _uint* pState)
{
    __super::State_Processing(fTimedelta, pState);
    m_fSturnTimeSum += fTimedelta;
  
    if (m_fSturnTimeSum > m_fSturnTime)
    {
        m_pParentObject->Set_bStun(false);
        return true;
    }
    return false;
}

_bool CPlayer_Sturn::State_Exit(_uint* pState)
{
    m_fSturnTimeSum = 0;

    return true;
}

void CPlayer_Sturn::Init_CallBack_Func()
{

}

_bool CPlayer_Sturn::IsActive(_uint stateFlags) const
{
    return (stateFlags & MOV_STURN) != 0;
}

void CPlayer_Sturn::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= MOV_STURN; 
    else
        *pState &= ~MOV_STURN; 
}

_bool CPlayer_Sturn::CanEnter(_uint* pState)
{
    if (*pState & (BEH_SHOOT | MOV_STURN))
        return false;

    return m_pParentObject->Get_bStun();
}

_bool CPlayer_Sturn::CheckInputCondition(_uint stateFlags)
{
    return false;
}

CPlayer_Sturn* CPlayer_Sturn::Create(void* pArg)
{
    CPlayer_Sturn* pInstance = new CPlayer_Sturn();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Sturn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Sturn::Free()
{
	__super::Free();
}
