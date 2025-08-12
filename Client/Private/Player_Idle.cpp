#include "stdafx.h"
#include "Player_Idle.h"
#include "Player_StateMachine.h"
#include "Weapon.h"
#include "Body_Player.h"
CPlayer_Idle::CPlayer_Idle()
{
}

HRESULT CPlayer_Idle::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Idle::State_Enter(_uint* pState)
{
    // hit 플래그가 켜져 있으면 끄기
    *pState &= ~MOV_HIT;
    
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_IDLE;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;
    m_StateDesc.fPlayTime = 1.f; // 애니메이션 속도 설정
    __super::State_Enter(pState);
}

_bool CPlayer_Idle::State_Processing(_float fTimedelta, _uint* pState)
{
    // hit 상태일 때도 Idle 애니메이션 재생
    return __super::State_Processing(fTimedelta, pState);
}

_bool CPlayer_Idle::State_Exit(_uint* iState)
{

    return true;
}

void CPlayer_Idle::Init_CallBack_Func()
{

}

_bool CPlayer_Idle::IsActive(_uint stateFlags) const
{   
    return (stateFlags & MOV_IDLE) != 0;
}

void CPlayer_Idle::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= MOV_IDLE; 
    else
        *pState &= ~MOV_IDLE; 
}

_bool CPlayer_Idle::CanEnter(_uint* pState)
{
    // 점프 상태가 활성화되면 Idle 상태로 진입하지 않음
    if ((*pState & MOV_JUMP) != 0)
        return false;
    
    _bool stateinput = !(*pState & ( MOV_RUN | MOV_SPRINT | BEH_RELOAD | BEH_SWICH));
    _bool KeyInput = !Move_KeyFlage(pState);
    
    return KeyInput && stateinput;
}

_bool CPlayer_Idle::CheckInputCondition(_uint stateFlags) 
{
    // hit 상태가 활성화되어도 이동 상태 유지
    return !(stateFlags & ( MOV_RUN | MOV_SPRINT | MOV_JUMP |
                           BEH_RELOAD | BEH_SWICH));
}

CPlayer_Idle* CPlayer_Idle::Create(void* pArg)
{
    CPlayer_Idle* pInstance = new CPlayer_Idle();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Idle::Free()
{
	__super::Free();
}
