#include"stdafx.h"
#include "JetFly_Idle.h"
#include "JetFly.h"
#include "GameInstance.h"

CJetFly_Idle::CJetFly_Idle()
{
}

HRESULT CJetFly_Idle::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.iCurrentState = 2;
    pNodeDesc.bIsLoop = true;
    pNodeDesc.pParentModel = m_pParentModel;
    m_StateNodes.push_back( CStateNode::Create(&pNodeDesc));
	return S_OK;
}

CStateMachine::Result CJetFly_Idle::StateMachine_Playing(_float fTimeDelta)
{
     return  __super::StateMachine_Playing(fTimeDelta);
}  

void CJetFly_Idle::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

CJetFly_Idle* CJetFly_Idle::Create(void* pArg)
{
    CJetFly_Idle* pInstance = new CJetFly_Idle();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CJetFly_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetFly_Idle::Free()
{
	__super::Free();
}