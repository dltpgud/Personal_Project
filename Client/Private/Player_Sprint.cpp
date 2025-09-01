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

void CPlayer_Sprint::State_Enter(_uint* pState, _uint* pPreState)
{    
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_SPRINT;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
 
    m_StateDesc.bLoop = true;
    m_pGameInstance->Set_OpenUI(true, TEXT("PlayerState"));
    m_pParentObject->Get_Transform()->Set_MoveSpeed(6.f);

    __super::State_Enter(pState, pPreState);
}

_bool CPlayer_Sprint::State_Processing(_float fTimedelta, _uint* pState, _uint* pPreState)
{
    if (*pState & BEH_SHOOT)
    {
        return false;
    }

    return __super::State_Processing(fTimedelta, pState, pPreState);
}

_bool CPlayer_Sprint::State_Exit(_uint* pState)
{
    m_pParentObject->Get_Transform()->Set_MoveSpeed(2.f);
    return true;
}

void CPlayer_Sprint::Init_CallBack_Func()
{
    for(_int i = 0; i <BODY_TYPE::T_END; i++){ m_pParentObject->BodyCallBack(i, CBody_Player::BODY_SPRINT, 0,
            [this]() { m_pGameInstance->Play_Sound(L"ST_Player_Footstep_Scuff_Sand.ogg", &m_pChannel, 1.f); });
    }
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
    if ((*pState & (BEH_RELOAD | BEH_SWICH | MOV_STURN | MOV_FALL)) != 0)
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
