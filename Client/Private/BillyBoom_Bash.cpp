#include"stdafx.h"
#include "BillyBoom_Bash.h"
#include "BillyBoom.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Body_BillyBoom.h"
#include "BossBullet_Berrle.h"

CBillyBoom_Bash::CBillyBoom_Bash()
{
}

HRESULT CBillyBoom_Bash::Initialize(void* pArg)
{
    ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);
	__super::Initialize(pDesc);
    m_StateNodes.resize(ANIM_END);
	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 11;
    pNodeDesc.iNextStateIdx = BASH_SHOOT;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[BASH_PRESHOOT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 12;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[BASH_SHOOT] = CStateNode::Create(&pNodeDesc);

	return S_OK;
}

CStateMachine::Result CBillyBoom_Bash::StateMachine_Playing(_float fTimeDelta)
{
   return  __super::StateMachine_Playing(fTimeDelta);
}

void CBillyBoom_Bash::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

CBillyBoom_Bash* CBillyBoom_Bash::Create(void* pArg)
{
    CBillyBoom_Bash* pInstance = new CBillyBoom_Bash();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBillyBoom_Barre");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBillyBoom_Bash::Free()
{
	__super::Free();
}