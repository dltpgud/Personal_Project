#include "stdafx.h"
#include "GameInstance.h"
#include "Player_StateMachine.h"
#include "Player_Idle.h"
#include "Player_Swich.h"
#include "Player_Reload.h"
#include "Player_Shoot.h"
#include "Player_Run.h"
#include "Player_Sprint.h"
#include "Player_Jump.h"
#include "Player_Hit.h"
#include "Player_Fall.h"
#include "Player_Sturn.h"
CPlayer_StateMachine::CPlayer_StateMachine() : m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_StateMachine::Initialize(void* pArg)
{
    PLAYER_STATEMACHINE_DESC* pDesc = static_cast<PLAYER_STATEMACHINE_DESC*>(pArg);
    m_pParentObject = pDesc->pParentObject;
 
	m_StateNodes.resize(NODE_END);

    m_StateNodes[NODE_SHOOT]  = CPlayer_Shoot::Create(pDesc);      
    m_StateNodes[NODE_RELOAD] = CPlayer_Reload::Create(pDesc);
    m_StateNodes[NODE_SWICH]  = CPlayer_Swich::Create(pDesc);
    m_StateNodes[NODE_HIT]    = CPlayer_Hit::Create(pDesc);
    m_StateNodes[NODE_JUMP]   = CPlayer_Jump::Create(pDesc);
    m_StateNodes[NODE_IDLE]   = CPlayer_Idle::Create(pDesc);
    m_StateNodes[NODE_RUN]    = CPlayer_Run::Create(pDesc);
    m_StateNodes[NODE_SPRINT] = CPlayer_Sprint::Create(pDesc);
    m_StateNodes[NODE_FALL]   = CPlayer_Fall::Create(pDesc);
    m_StateNodes[NODE_STUN]   = CPlayer_Sturn::Create(pDesc);

    m_iState |= CPlayer_StateNode::MOV_IDLE;
	return S_OK;
}

void CPlayer_StateMachine::StateMachine_Playing(_float fTimeDelta)
{
    if (m_StateNodes.empty())
        return;
    
    for (auto& node : m_StateNodes)
    {
        if (nullptr == node)
            continue;

        _bool bactive = node->IsActive(m_iState);

         progress_Move(fTimeDelta, &m_iState);
        
        if (!bactive)
        {
            if (node->CanEnter(&m_iState))
            {
                node->State_Enter(&m_iState); 
                node->SetActive(true, &m_iState);
            }
        }
        else
        {
            if (node->State_Processing(fTimeDelta, &m_iState))
            {
                node->State_Exit(&m_iState);
                node->SetActive(false, &m_iState);
            }
        }
    }

    return;
}

void CPlayer_StateMachine::progress_Move(_float fTimeDelta, _uint* pState) 
{   
    if ((*pState & CPlayer_StateNode::MOV_STURN) != 0)
        return ;

    if (*pState & CPlayer_StateNode::DIR_LEFT)
        m_pParentObject->Get_Transform()->Go_Move(CTransform::LEFT, fTimeDelta, m_pParentObject->Get_Navi());

    if (*pState & CPlayer_StateNode::DIR_RIGHT)
        m_pParentObject->Get_Transform()->Go_Move(CTransform::RIGHT, fTimeDelta, m_pParentObject->Get_Navi());

    if (*pState & CPlayer_StateNode::DIR_FORWARD)
        m_pParentObject->Get_Transform()->Go_Move(CTransform::GO, fTimeDelta, m_pParentObject->Get_Navi());

    if (*pState & CPlayer_StateNode::DIR_BACK)
        m_pParentObject->Get_Transform()->Go_Move(CTransform::BACK, fTimeDelta, m_pParentObject->Get_Navi());
}

CPlayer_StateMachine* CPlayer_StateMachine::Create(void* pArg)
{
	CPlayer_StateMachine* pInstance = new CPlayer_StateMachine();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_StateMachine::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

    for (auto& Node : m_StateNodes)
            Safe_Release(Node);
       
    m_StateNodes.clear();
    m_StateNodes.shrink_to_fit();
}
