#include "stdafx.h"
#include "Player_StateMachine.h"
#include "Player_Hit.h"
#include "Weapon.h"
#include "Body_Player.h"
CPlayer_Hit::CPlayer_Hit()
{
}

HRESULT CPlayer_Hit::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Hit::State_Enter(_uint* pState, _uint* pPreState)
{
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_STUN;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = false;
    m_pGameInstance->Set_OpenUI(true, TEXT("PlayerState"));
    __super::State_Enter(pState, pPreState);
}

_bool CPlayer_Hit::State_Processing(_float fTimedelta, _uint* pState, _uint* pPreState)
{
    if (Move_KeyFlage(pState))
        return true;

    if (*pState & CPlayer:: MOV_STURN)
    {
        m_pParentObject->Set_State(CPlayer::FLAG_KEYLOCK, true); 
        return true;
    }
    _bool bBodyAnimFinished = m_pParentObject->Set_PartObj_Play_Anim(CPlayer::PART_BODY, fTimedelta * m_StateDesc.fPlayTime);
    _bool bWeaponAnimFinished = m_pParentObject->Set_PartObj_Play_Anim(CPlayer::PART_WEAPON, fTimedelta * m_StateDesc.fPlayTime);
    
   return bBodyAnimFinished && bWeaponAnimFinished;
}

_bool CPlayer_Hit::State_Exit(_uint* pState)
{
    return true;
}

void CPlayer_Hit::Init_CallBack_Func()
{
   for(_int i = 0; i <BODY_TYPE::T_END; i++){ m_pParentObject->BodyCallBack(i, CBody_Player::BODY_STUN,
        0, [this]() { m_pGameInstance->Play_Sound(L"ST_Player_Flash_Stop.ogg",  &m_pChannel, 1.f); });
   }
}

_bool CPlayer_Hit::IsActive(_uint stateFlags) const
{
    return (stateFlags & CPlayer::MOV_HIT) != 0;
}

void CPlayer_Hit::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= CPlayer:: MOV_HIT; 
    else
        *pState &= ~CPlayer::MOV_HIT;
}

_bool CPlayer_Hit::CanEnter(_uint* pState)
{
    if (*pState & (CPlayer::BEH_SHOOT  | CPlayer::MOV_HEALTH))
        return false;

    return (*pState & CPlayer::MOV_HIT) && !(*pState & CPlayer::MOV_STURN);
}

_bool CPlayer_Hit::CheckInputCondition(_uint stateFlags)
{
    if (stateFlags & (CPlayer::BEH_SHOOT | CPlayer::MOV_HIT | CPlayer::MOV_STURN))
          return false;
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
