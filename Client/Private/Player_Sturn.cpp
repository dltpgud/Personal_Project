#include "stdafx.h"
#include "Player_StateMachine.h"
#include "Player_Sturn.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "Player_StateUI.h"
CPlayer_Sturn::CPlayer_Sturn()
{
}

HRESULT CPlayer_Sturn::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Sturn::State_Enter(_uint* pState, _uint* pPreState)
{
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_COOLING_LONG;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = false; 
    static_cast<CPlayer_StateUI*>(m_pGameInstance->Find_Clone_UIObj(L"PlayerState"))->Set_Open(true);
    __super::State_Enter(pState, pPreState);
}

_bool CPlayer_Sturn::State_Processing(_float fTimedelta, _uint* pState, _uint* pPreState)
{
    _bool bAnimFinished = __super::State_Processing(fTimedelta, pState, pPreState);
    
    if (bAnimFinished || !m_pParentObject->GetFlag(CPlayer::FLAG_STURN))
    {
        return true;
    }
    return false;
}

_bool CPlayer_Sturn::State_Exit(_uint* pState)
{
    m_pParentObject->SetFlag(CPlayer::FLAG_STURN, false);
    m_pParentObject->SetFlag(CPlayer::FLAG_KEYLOCK, false);
 
    return true;
}

void CPlayer_Sturn::Init_CallBack_Func()
{
     for(_int i = 0; i <BODY_TYPE::T_END; i++){ m_pParentObject->BodyCallBack(i, CBody_Player::BODY_COOLING_LONG, 0,
             [this]() { m_pGameInstance->Play_Sound(L"ST_Player_Stun_Loop.wav",  &m_pChannel, 0.5f); });
     }
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
    if (*pState & MOV_STURN)
        return false;

    return m_pParentObject->GetFlag(CPlayer::FLAG_STURN);
}

_bool CPlayer_Sturn::CheckInputCondition(_uint stateFlags)
{
    return true;
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
