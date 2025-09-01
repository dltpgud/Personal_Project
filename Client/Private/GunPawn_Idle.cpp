#include"stdafx.h"
#include "GunPawn_Idle.h"
#include "GameInstance.h"

CGunPawn_Idle::CGunPawn_Idle()
{
}

HRESULT CGunPawn_Idle::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.iCurrentState = 4;
    pNodeDesc.bIsLoop = true;
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iNextStateIdx = 0;
    m_StateNodes.push_back( CStateNode::Create(&pNodeDesc));
	return S_OK;
}

CStateMachine::Result CGunPawn_Idle::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    return __super::StateMachine_Playing(fTimeDelta,pRim);
}  

void CGunPawn_Idle::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
}

CGunPawn_Idle* CGunPawn_Idle::Create(void* pArg)
{
    CGunPawn_Idle* pInstance = new CGunPawn_Idle();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CGunPawn_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunPawn_Idle::Free()
{
	__super::Free();
}