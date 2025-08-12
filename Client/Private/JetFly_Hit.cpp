#include"stdafx.h"
#include "JetFly_Hit.h"
#include "Body_JetFly.h"
#include "GameInstance.h"
#include "JetFly.h"
CJetFly_Hit::CJetFly_Hit()
{
}

HRESULT CJetFly_Hit::Initialize(void* pArg)
{
    HIT_DESC* pDesc = static_cast<HIT_DESC*>(pArg);
	__super::Initialize(pDesc);

	//애니메이션 하나뺴고 다 망가졌네..
    CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iCurrentState = 4;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CJetFly_Hit::StateMachine_Playing(_float fTimeDelta)
{   
     return __super::StateMachine_Playing(fTimeDelta);
}      
void CJetFly_Hit::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

CJetFly_Hit* CJetFly_Hit::Create(void* pArg)
{
    CJetFly_Hit* pInstance = new CJetFly_Hit();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CJetFly_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetFly_Hit::Free()
{
	__super::Free();
}