#include"stdafx.h"
#include "MecanoBot_Idle.h"
#include "BoomBot.h"
#include "GameInstance.h"

CMecanoBot_Idle::CMecanoBot_Idle()
{
}

HRESULT CMecanoBot_Idle::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.iCurrentState = 0;
    pNodeDesc.bIsLoop = true;
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iNextStateIdx = 0;
    m_StateNodes.push_back( CStateNode::Create(&pNodeDesc));
	return S_OK;
}

CStateMachine::Result CMecanoBot_Idle::StateMachine_Playing(_float fTimeDelta)
{
     return  __super::StateMachine_Playing(fTimeDelta);
}  

void CMecanoBot_Idle::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

CMecanoBot_Idle* CMecanoBot_Idle::Create(void* pArg)
{
    CMecanoBot_Idle* pInstance = new CMecanoBot_Idle();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMecanoBot_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMecanoBot_Idle::Free()
{
	__super::Free();
}