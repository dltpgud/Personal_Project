#include "stdafx.h"
#include "Player_Run.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "Player_StateMachine.h"
CPlayer_Run::CPlayer_Run()
{
}

HRESULT CPlayer_Run::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Run::State_Enter(_uint* pState, _uint* pPreState)
{
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_RUN;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;

    __super::State_Enter(pState, pPreState);
}

_bool CPlayer_Run::State_Processing(_float fTimedelta, _uint* pState,_uint* pPreState)
{
    return __super::State_Processing(fTimedelta, pState, pPreState);
}

_bool CPlayer_Run::State_Exit(_uint* pState)
{
    return true;
}

void CPlayer_Run::Init_CallBack_Func()
{
    for(_int i = 0; i <BODY_TYPE::T_END; i++){ m_pParentObject->BodyCallBack(i, CBody_Player::BODY_RUN, 0,
            [this]() { m_pGameInstance->Play_Sound(L"ST_Player_Footstep_Scuff_Sand.ogg", &m_pChannel, 1.f); });
    }
}

_bool CPlayer_Run::IsActive(_uint stateFlags) const
{
    return (stateFlags & CPlayer::MOV_RUN) != 0;
}

void CPlayer_Run::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= CPlayer::MOV_RUN;
    else
        *pState &= ~CPlayer::MOV_RUN;
}

_bool CPlayer_Run::CanEnter(_uint* pState)
{
    if (*pState & CPlayer::MOV_FALL)
        return false;
        
    _bool isOtherAction = !(*pState & (CPlayer::MOV_JUMP | CPlayer::BEH_RELOAD | CPlayer::BEH_SWICH |
                                       CPlayer::BEH_SHOOT | CPlayer::MOV_STURN | CPlayer::MOV_HIT));
    _bool isRunningInput = Move_KeyFlage(pState) && !m_pGameInstance->Get_DIKeyState(DIK_LSHIFT);

    return isRunningInput && isOtherAction;
}

_bool CPlayer_Run::CheckInputCondition(_uint stateFlags)
{
    _bool Moving = Move_KeyFlage(&stateFlags) || m_pGameInstance->Get_DIKeyState(DIK_LSHIFT);

    _bool isOtherAction = (stateFlags & (CPlayer::BEH_RELOAD | CPlayer::BEH_SWICH | CPlayer::BEH_SHOOT |
                                         CPlayer::MOV_FALL | CPlayer::MOV_STURN | CPlayer::MOV_HIT)) != 0;

    return Moving && !isOtherAction;
}

CPlayer_Run* CPlayer_Run::Create(void* pArg)
{
    CPlayer_Run* pInstance = new CPlayer_Run();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Run");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Run::Free()
{
	__super::Free();
}
