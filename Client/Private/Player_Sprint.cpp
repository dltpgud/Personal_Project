#include "stdafx.h"
#include "Player_StateMachine.h"
#include "Player_Sprint.h"
#include "Weapon.h"
#include "Body_Player.h"
CPlayer_Sprint::CPlayer_Sprint()
{
}

HRESULT CPlayer_Sprint::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Sprint::State_Enter(_uint* pState)
{
    // hit 플래그가 켜져 있으면 끄기
    *pState &= ~MOV_HIT;
    
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_SPRINT;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;
    m_StateDesc.fPlayTime = 1.f; // 애니메이션 속도 설정
    m_pParentObject->Get_Transform()->Set_MoveSpeed(8.f);
    __super::State_Enter(pState);
}

_bool CPlayer_Sprint::State_Processing(_float fTimedelta, _uint* pState)
{
    return __super::State_Processing(fTimedelta,pState);
}

_bool CPlayer_Sprint::State_Exit(_uint* pState)
{
    SetActive(false, pState);
    m_pParentObject->Get_Transform()->Set_MoveSpeed(4.f);
    return true;
}

void CPlayer_Sprint::Init_CallBack_Func()
{

}

_bool CPlayer_Sprint::IsActive(_uint stateFlags) const
{
    return (stateFlags & MOV_SPRINT) != 0;
}

void CPlayer_Sprint::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= MOV_SPRINT; 
    else
        *pState &= ~MOV_SPRINT; 
}

_bool CPlayer_Sprint::CanEnter(_uint* pState) 
{
    // 점프 상태가 활성화되면 Sprint 상태로 진입하지 않음
    if ((*pState & MOV_JUMP) != 0)
        return false;
        
    // 이미 스프린트 상태면 진입 불가
    if ((*pState & MOV_SPRINT) != 0)
        return false; 

    // 장전 중이면 스프린트 진입 불가
    if ((*pState & BEH_RELOAD) != 0)
        return false;

    _bool isShiftDown = m_pGameInstance->Get_DIKeyState(DIK_LSHIFT); 
    _bool isMoving = Move_KeyFlage(pState);

    if (isShiftDown && isMoving)
    {
        // Run 상태 플래그 끄기
        *pState &= ~MOV_RUN;
        return true;
    }

    return false;
}

_bool CPlayer_Sprint::CheckInputCondition(_uint stateFlags) 
{
    _uint Flags = stateFlags;

    // R키가 눌렸으면 즉시 스프린트 상태 종료
    if (m_pGameInstance->Get_DIKeyDown(DIK_R))
        return false;

    _bool Moving = Move_KeyFlage(&Flags) && m_pGameInstance->Get_DIKeyState(DIK_LSHIFT);

    _bool isOtherAction = (stateFlags & (BEH_RELOAD | BEH_SWICH)) != 0;
    // hit 상태가 활성화되어도 이동 상태 유지
    return Moving && !isOtherAction;
}

CPlayer_Sprint* CPlayer_Sprint::Create(void* pArg)
{
    CPlayer_Sprint* pInstance = new CPlayer_Sprint();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Sprint");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Sprint::Free()
{
	__super::Free();
}
