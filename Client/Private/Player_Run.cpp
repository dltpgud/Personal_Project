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
    // hit 플래그가 켜져 있으면 끄기
    *pState &= ~MOV_HIT;
    
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_RUN;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;
    m_StateDesc.fPlayTime = 1.f; // 애니메이션 속도 설정
    __super::State_Enter(pState);
}

_bool CPlayer_Run::State_Processing(_float fTimedelta, _uint* pState)
{

    return __super::State_Processing(fTimedelta, pState);
}

_bool CPlayer_Run::State_Exit(_uint* pState)
{
    SetActive(false, pState);
    //    *Next = CPlayer_StateMachine::NODE_IDLE;
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
    // 점프 상태가 활성화되면 Run 상태로 진입하지 않음
    if ((*pState & MOV_JUMP) != 0)
        return false;
        
    _bool isRunningInput = Move_KeyFlage(pState) && !m_pGameInstance->Get_DIKeyState(DIK_LSHIFT);

    _uint stateFlags = *pState;

    _bool isOtherAction = (stateFlags & (BEH_RELOAD | BEH_SWICH)) != 0;

    return isRunningInput && !isOtherAction;
}

_bool CPlayer_Run::CheckInputCondition(_uint stateFlags) 
{
    _uint Flags = stateFlags;

    _bool Moving = Move_KeyFlage(&Flags) || m_pGameInstance->Get_DIKeyState(DIK_LSHIFT);

    _bool isOtherAction = (stateFlags & (BEH_RELOAD | BEH_SWICH)) != 0;
    // hit 상태가 활성화되어도 이동 상태 유지
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
