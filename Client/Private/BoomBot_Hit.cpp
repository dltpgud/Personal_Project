#include"stdafx.h"
#include "BoomBot_Hit.h"
#include "BoomBot.h"
#include "GameInstance.h"
#include "Body_BoomBot.h"

CBoomBot_Hit::CBoomBot_Hit()
{
}

HRESULT CBoomBot_Hit::Initialize(void* pArg)
{
    HIT_DESC* pDesc = static_cast<HIT_DESC*>(pArg);
    m_HitType = pDesc->HitType;
	__super::Initialize(pDesc);

	m_StateNodes.resize(ANIM_END);

    CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iCurrentState = 21;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[FrontHit] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 20;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[BackHit] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 22;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[LeftHit] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 23;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[RightHit] = CStateNode::Create(&pNodeDesc);

	return S_OK;
}

CStateMachine::Result CBoomBot_Hit::StateMachine_Playing(_float fTimeDelta)
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
    
     return __super::StateMachine_Playing(fTimeDelta);
}      
void CBoomBot_Hit::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

CBoomBot_Hit* CBoomBot_Hit::Create(void* pArg)
{
    CBoomBot_Hit* pInstance = new CBoomBot_Hit();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBoomBot_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoomBot_Hit::Free()
{
	__super::Free();
}