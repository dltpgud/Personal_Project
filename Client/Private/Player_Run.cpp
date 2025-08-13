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

void CPlayer_Run::State_Enter(_uint* pState)
{
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_RUN;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;

    __super::State_Enter(pState);
}

_bool CPlayer_Run::State_Processing(_float fTimedelta, _uint* pState)
{
    return __super::State_Processing(fTimedelta, pState);
}

_bool CPlayer_Run::State_Exit(_uint* pState)
{
    //*pState &= ~BEH_SHOOT;
    return true;
}

void CPlayer_Run::Init_CallBack_Func()
{
}

_bool CPlayer_Run::IsActive(_uint stateFlags) const
{
    return (stateFlags & MOV_RUN) != 0;
}

void CPlayer_Run::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= MOV_RUN;
    else
        *pState &= ~MOV_RUN;
}

_bool CPlayer_Run::CanEnter(_uint* pState)
{
    _bool isOtherAction = (*pState & (MOV_JUMP | BEH_RELOAD | BEH_SWICH | MOV_STURN)) != 0;
    _bool isRunningInput = Move_KeyFlage(pState) && !m_pGameInstance->Get_DIKeyState(DIK_LSHIFT);
    _uint stateFlags = *pState;

    return isRunningInput && !isOtherAction;
}

_bool CPlayer_Run::CheckInputCondition(_uint stateFlags) 
{
    _bool Moving = Move_KeyFlage(&stateFlags) || m_pGameInstance->Get_DIKeyState(DIK_LSHIFT);

    _bool isOtherAction = (stateFlags & (BEH_RELOAD | BEH_SWICH)) != 0;

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
