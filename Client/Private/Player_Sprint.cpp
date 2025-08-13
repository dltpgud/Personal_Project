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
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_SPRINT;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;
  
    m_pParentObject->Get_Transform()->Set_MoveSpeed(8.f);
    __super::State_Enter(pState);
}

_bool CPlayer_Sprint::State_Processing(_float fTimedelta, _uint* pState)
{
    return __super::State_Processing(fTimedelta,pState);
}

_bool CPlayer_Sprint::State_Exit(_uint* pState)
{
 //   *pState &= ~BEH_SHOOT;
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
    if ((*pState & (MOV_JUMP | MOV_SPRINT | BEH_RELOAD | MOV_STURN)) != 0)
        return false;

    _bool isShiftDown = m_pGameInstance->Get_DIKeyState(DIK_LSHIFT);

    _bool isMoving = Move_KeyFlage(pState);

    if (isShiftDown && isMoving)
    {
        *pState &= ~MOV_RUN;
        *pState &= ~MOV_HIT;
        return true;
    }

    return false;
}

_bool CPlayer_Sprint::CheckInputCondition(_uint stateFlags) 
{
    if (m_pGameInstance->Get_DIKeyDown(DIK_R))
        return false;

    _bool Moving = Move_KeyFlage(&stateFlags) && m_pGameInstance->Get_DIKeyState(DIK_LSHIFT);

    _bool isOtherAction = (stateFlags & (BEH_RELOAD | BEH_SWICH)) != 0;
  
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
