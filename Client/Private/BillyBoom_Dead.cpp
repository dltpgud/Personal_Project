#include"stdafx.h"
#include "BillyBoom_Dead.h"
#include "GameInstance.h"
#include "Body_BillyBoom.h"
#include "Particle_Explosion.h"
CBillyBoom_Dead::CBillyBoom_Dead()
{
}

HRESULT CBillyBoom_Dead::Initialize(void* pArg)
{
    DEAD_DESC* pDesc = static_cast<DEAD_DESC*>(pArg);
    m_iEmissiveMashNum = pDesc->iEmissiveMashNum;
    m_fEmissivePower = pDesc->fEmissivePower;
    m_fEmissiveColor = pDesc->fEmissiveColor;
	__super::Initialize(pDesc);

	CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.iCurrentState = 1;
    pNodeDesc.bIsLoop = true;
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iNextStateIdx = 0;
    pNodeDesc.fPlayAniTime = 0.5f;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CBillyBoom_Dead::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
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

    m_DeadTimeSum += fTimeDelta;

    if (m_DeadTimeSum > 2.f && !m_bEffect)
    {
        *m_iEmissiveMashNum = -1;
        *m_fEmissivePower = 10.f;
        *m_fEmissiveColor = {1.f, 0.2f, 0.0};

        CParticle_Explosion::CParticle_Explosion_Desc Desc{};
        Desc.pParentMatrix = m_pParentObject->Get_Transform()->Get_WorldMatrix();

        m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, TEXT("Layer_Skill"), L"Prototype_GameObject_Particle_Explosion", &Desc);

        m_bEffect = true;
    }

    if (m_bEffect)
    {
        *pRim->eState = RIM_LIGHT_DESC::STATE_RIM;
        pRim->fcolor = {1.f, 0.8f, 0.f, 0.f};
        m_pParentObject->Get_Transform()->Go_Move(CTransform::DOWN, fTimeDelta);
    }

    if (m_iCurIndex != m_iNextIndex)
    {
        m_pParentObject->Get_Transform()->Set_MoveSpeed(1.5f);
        m_pParentObject->Get_Transform()->Set_fGravity(0.f);
        m_pGameInstance->Play_Sound(L"ST_BillyBoom_Outro.ogg", &m_pChannel, 1.f);
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
     if (XMVectorGetY(m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION)) <-20)
        m_pParentObject->Set_Dead(true);

     return Result::Running;
}      
void CBillyBoom_Dead::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
}

CBillyBoom_Dead* CBillyBoom_Dead::Create(void* pArg)
{
    CBillyBoom_Dead* pInstance = new CBillyBoom_Dead();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBillyBoom_Dead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBillyBoom_Dead::Free()
{
	__super::Free();
}