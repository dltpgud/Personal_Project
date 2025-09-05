#include "stdafx.h"
#include "Player_StateNode.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "Body_Player.h"

CPlayer_StateNode::CPlayer_StateNode()
	:	m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_StateNode::Initialize(void* pArg)
{
    PLAYER_INFO_DESC* pDesc = static_cast<PLAYER_INFO_DESC*>(pArg);
    m_pCurWeapon    = pDesc->pCurWeapon;
    m_pParentObject = pDesc->pParentObject;
     Init_CallBack_Func();
	return S_OK;
}

void CPlayer_StateNode::State_Enter(_uint* pState, _uint* pPreState)
{
    for (_int i = 0; i < CPlayer::PART_END; i++)
        m_pParentObject->Set_PartObj_Set_Anim(i, m_StateDesc.iCurMotion[i], m_StateDesc.bLoop); 
}

_bool CPlayer_StateNode::State_Processing(_float fTimedelta, _uint* pState, _uint* pPreState)
{
	if (false == CheckInputCondition(*pState))
    return true; 

    m_pParentObject->Set_PartObj_Play_Anim(CPlayer::PART_WEAPON, fTimedelta * m_StateDesc.fPlayTime);
    
    return m_pParentObject->Set_PartObj_Play_Anim(CPlayer::PART_BODY, fTimedelta * m_StateDesc.fPlayTime);
}

_bool CPlayer_StateNode::State_Exit(_uint* pState)
{
	 return true;
}

void CPlayer_StateNode::Init_CallBack_Func()
{
}

_bool CPlayer_StateNode::Move_KeyFlage(_uint* pState)
{
    if (m_pParentObject->HasState(CPlayer::FLAG_KEYLOCK) == true)
    {
        *pState &= ~(CPlayer::DIR_FORWARD | CPlayer::DIR_BACK | CPlayer::DIR_LEFT | CPlayer::DIR_RIGHT);
        return false;
    }
   
    _bool bMove{};

    if (m_pGameInstance->Get_DIKeyDown(DIK_A) || m_pGameInstance->Get_DIKeyState(DIK_A)) 
	{
	  bMove = true;
            *pState |= CPlayer::DIR_LEFT;
	}
    if (m_pGameInstance->Get_DIKeyDown(DIK_D) || m_pGameInstance->Get_DIKeyState(DIK_D))
	{
      bMove = true;
            *pState |= CPlayer::DIR_RIGHT;
	}
	if (m_pGameInstance->Get_DIKeyDown(DIK_W) || m_pGameInstance->Get_DIKeyState(DIK_W))
	{
      bMove = true;
            *pState |= CPlayer::DIR_FORWARD;
	}
	if (m_pGameInstance->Get_DIKeyDown(DIK_S) || m_pGameInstance->Get_DIKeyState(DIK_S))
	{
      bMove = true;
            *pState |= CPlayer::DIR_BACK;
	}

	if (false == bMove)
            *pState &= ~(CPlayer::DIR_FORWARD | CPlayer::DIR_BACK | CPlayer::DIR_LEFT | CPlayer::DIR_RIGHT);

    return bMove;
}
#ifdef _DEBUG
void CPlayer_StateNode::CheckState(_uint* State) const
{


    if (IsFlagOn(State, CPlayer::DIR_FORWARD))
        std::cout << "앞으로 이동 중\n";

    if (IsFlagOn(State, CPlayer::DIR_BACK))
        std::cout << "뒤으로 이동 중\n";

    if (IsFlagOn(State, CPlayer::DIR_LEFT))
        std::cout << "왼쪽으로 이동 중\n";

    if (IsFlagOn(State, CPlayer::DIR_RIGHT))
        std::cout << "오른쪽으로 이동 중\n";

    if (IsFlagOn(State, CPlayer::BEH_RELOAD))
        std::cout << "장전 중\n";

    if (IsFlagOn(State, CPlayer::BEH_SHOOT))
        std::cout << "쏘는 중\n";

    if (IsFlagOn(State, CPlayer::BEH_SWICH))
        std::cout << "바꾸는 중\n";

    if (IsFlagOn(State, CPlayer::MOV_IDLE))
        std::cout << "아이들\n";

    if (IsFlagOn(State, CPlayer::MOV_RUN))
        std::cout << "달리 중\n";

    if (IsFlagOn(State, CPlayer::MOV_SPRINT))
        std::cout << "스프린트 중\n";

    if (IsFlagOn(State, CPlayer::MOV_JUMP))
        std::cout << "뛰는 중\n";
    if (IsFlagOn(State, CPlayer::MOV_HIT))
        std::cout << "아픈 중\n";

    if (IsFlagOn(State, CPlayer::MOV_FALL))
        std::cout << "떨어지는중 중\n";

    if (IsFlagOn(State, CPlayer::MOV_STURN))
        std::cout << "스턴 중\n";
    if (IsFlagOn(State, CPlayer::MOV_HEALTH))
        std::cout << "회복 중\n";
}
#endif

CPlayer_StateNode* CPlayer_StateNode::Create(void* pArg)
{
	CPlayer_StateNode* pInstance = new CPlayer_StateNode();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_StateNode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_StateNode::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
