#include"stdafx.h"
#include "BoomBot_Idle.h"
#include "BoomBot.h"
#include "GameInstance.h"

CBoomBot_Idle::CBoomBot_Idle()
{
}

HRESULT CBoomBot_Idle::Initialize(void* pArg)
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

CStateMachine::Result CBoomBot_Idle::StateMachine_Playing(_float fTimeDelta)
{
     return  __super::StateMachine_Playing(fTimeDelta);
}      
void CBoomBot_Idle::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

CBoomBot_Idle* CBoomBot_Idle::Create(void* pArg)
{
    CBoomBot_Idle* pInstance = new CBoomBot_Idle();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBoomBot_IDLE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoomBot_Idle::Free()
{
	__super::Free();
}