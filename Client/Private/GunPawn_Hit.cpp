#include"stdafx.h"
#include "GunPawn_Hit.h"
#include "GameInstance.h"
CGunPawn_Hit::CGunPawn_Hit()
{
}

HRESULT CGunPawn_Hit::Initialize(void* pArg)
{
    HIT_DESC* pDesc = static_cast<HIT_DESC*>(pArg);
    m_HitType = pDesc->HitType;
	__super::Initialize(pDesc);

	m_StateNodes.resize(ANIM_END);

    CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iCurrentState = 15;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[FrontHit] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 14;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[BackHit] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 16;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[LeftHit] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 17;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[RightHit] = CStateNode::Create(&pNodeDesc);

	return S_OK;
}

CStateMachine::Result CGunPawn_Hit::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    switch (*m_HitType)
    {
    case CMonster::AT_FRONT: m_iNextIndex = FrontHit;
        break;
    case CMonster::AT_BACK: m_iNextIndex = BackHit; 
        break;
    case CMonster::AT_LEFT: m_iNextIndex = LeftHit; 
        break;
    case CMonster::AT_RIGHT: m_iNextIndex = RightHit; 
        break;
    default: break;
    }
    
    *pRim->eState = RIM_LIGHT_DESC::STATE_RIM;
    pRim->fcolor = {1.f, 1.f, 1.f, 1.f};
    pRim->iPower = 0.5f;
    return __super::StateMachine_Playing(fTimeDelta,pRim);
}      
void CGunPawn_Hit::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
}

CGunPawn_Hit* CGunPawn_Hit::Create(void* pArg)
{
    CGunPawn_Hit* pInstance = new CGunPawn_Hit();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CGunPawn_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunPawn_Hit::Free()
{
	__super::Free();
}