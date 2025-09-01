#include"stdafx.h"
#include "BillyBoom_Idle.h"
#include "BillyBoom.h"
#include "GameInstance.h"

CBillyBoom_Idle::CBillyBoom_Idle()
{
}

HRESULT CBillyBoom_Idle::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.iCurrentState = 1;
    pNodeDesc.bIsLoop = true;
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.fPlayAniTime = 0.5f;
    m_StateNodes.push_back( CStateNode::Create(&pNodeDesc));
	return S_OK;
}

CStateMachine::Result CBillyBoom_Idle::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    if (HasFlag(PAUSED))
        return Result::None;

    if (m_StateNodes.empty())
        return Result::None;

    if (m_iNextIndex < 0 || m_iNextIndex >= static_cast<_int>(m_StateNodes.size()))
    {
        Reset_StateMachine(pRim);
        SetFlag(FINISHED);
        return Result::Finished;
    }

    if (m_iCurIndex != m_iNextIndex)
    {
        static_cast<CBillyBoom*>(m_pParentObject)->Set_bSkill(false);
        m_iCurIndex = m_iNextIndex;
        m_StateNodes[m_iCurIndex]->State_Enter();
    }

    if (m_StateNodes[m_iCurIndex]->State_Processing(fTimeDelta))
    {
        _int iNextIndex = -1;

        if (m_StateNodes[m_iCurIndex]->State_Exit(&iNextIndex))
        {
            m_iNextIndex = iNextIndex;
        }
        else
        {
            Reset_StateMachine(pRim);
            SetFlag(FINISHED);
            return Result::Finished;
        }
    }

    return Result::Running;
}  

void CBillyBoom_Idle::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
}

CBillyBoom_Idle* CBillyBoom_Idle::Create(void* pArg)
{
    CBillyBoom_Idle* pInstance = new CBillyBoom_Idle();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBillyBoom_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBillyBoom_Idle::Free()
{
	__super::Free();
}