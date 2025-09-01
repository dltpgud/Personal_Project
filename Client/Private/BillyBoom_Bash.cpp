#include"stdafx.h"
#include "BillyBoom_Bash.h"
#include "GameInstance.h"
#include "Bullet.h"

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

CStateMachine::Result CBillyBoom_Bash::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    if (*pRim->eState == RIM_LIGHT_DESC::STATE_RIM)
    {
        pRim->iPower = 0.3f;
        pRim->fcolor = {1.f, 1.f, 1.f, 1.f};
        m_fRimTimeSum += fTimeDelta;
    }

    if (m_fRimTimeSum > 0.5f)
    {
        m_fRimTimeSum = 0.f;
        *pRim->eState = RIM_LIGHT_DESC::STATE_NORIM;
        pRim->fcolor = {0.f, 0.f, 0.f, 0.f};
    }
    return __super::StateMachine_Playing(fTimeDelta, pRim);
}

void CBillyBoom_Bash::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    m_fRimTimeSum = 0.f;
    m_pGameInstance->StopSound(&m_pChannel);
    __super::Reset_StateMachine(pRim);
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