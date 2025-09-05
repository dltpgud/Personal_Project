#include "stdafx.h"
#include "Player_StateMachine.h"
#include "Player_Swich.h"
#include "Weapon.h"
#include "Body_Player.h"
CPlayer_Swich::CPlayer_Swich()
{
}

HRESULT CPlayer_Swich::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Swich::State_Enter(_uint* pState, _uint* pPreState)
{
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_SWICH;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;

    __super::State_Enter(pState, pPreState);
}

_bool CPlayer_Swich::State_Processing(_float fTimedelta, _uint* pState, _uint* pPreState)
{
    
    return __super::State_Processing(fTimedelta, pState, pPreState);
}

_bool CPlayer_Swich::State_Exit(_uint* pState)
{
    m_pParentObject->Set_State(CPlayer::BRH_CHANGE, false);
   return true;
}

void CPlayer_Swich::Init_CallBack_Func()
{
    for (_int i = 0; i < BODY_TYPE::T_END; i++)
    {
        m_pParentObject->BodyCallBack(i, CBody_Player::BODY_SWICH, 0,[this]() { m_pGameInstance->Play_Sound(L"ST_Player_Switch_T01.ogg",  &m_pChannel, 1.f); });
    }
}

_bool CPlayer_Swich::IsActive(_uint stateFlags) const
{
    return (stateFlags & CPlayer::BEH_SWICH) != 0;
}

void CPlayer_Swich::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= CPlayer::BEH_SWICH;
    else
        *pState &= ~CPlayer::BEH_SWICH;
}

_bool CPlayer_Swich::CanEnter(_uint* pState)
{
    if ((*pState & (CPlayer::MOV_JUMP | CPlayer::BEH_SWICH)) != 0)
        return false;

    return m_pParentObject->HasState(CPlayer::BRH_CHANGE);
}

_bool CPlayer_Swich::CheckInputCondition(_uint stateFlags)
{
    return true;
}

CPlayer_Swich* CPlayer_Swich::Create(void* pArg)
{
    CPlayer_Swich* pInstance = new CPlayer_Swich();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Swich");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Swich::Free()
{
	__super::Free();
}
