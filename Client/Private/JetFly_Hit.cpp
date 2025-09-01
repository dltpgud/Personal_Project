#include"stdafx.h"
#include "JetFly_Hit.h"
#include "GameInstance.h"

CJetFly_Hit::CJetFly_Hit()
{
}

HRESULT CJetFly_Hit::Initialize(void* pArg)
{
    HIT_DESC* pDesc = static_cast<HIT_DESC*>(pArg);
	__super::Initialize(pDesc);

    CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iCurrentState = 4;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CJetFly_Hit::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{   
    *pRim->eState = RIM_LIGHT_DESC::STATE_RIM;
    pRim->fcolor = {1.f, 1.f, 1.f, 1.f};
    pRim->iPower = 0.5f;
    return __super::StateMachine_Playing(fTimeDelta, pRim);
}      
void CJetFly_Hit::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
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