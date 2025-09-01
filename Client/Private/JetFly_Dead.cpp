#include"stdafx.h"
#include "JetFly_Dead.h"
#include "GameInstance.h"

CJetFly_Dead::CJetFly_Dead()
{
}

HRESULT CJetFly_Dead::Initialize(void* pArg)
{
    DEAD_DESC* pDesc = static_cast<DEAD_DESC*>(pArg);
	__super::Initialize(pDesc);

	CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.iCurrentState = 10; 
    pNodeDesc.bIsLoop = false;
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iNextStateIdx = 0;
    pNodeDesc.fPlayAniTime = 0.5f;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CJetFly_Dead::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    m_pParentPartObject->Set_DeadState(true);

    _vector vPos =  m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

    _bool isFall = m_pParentObject->Get_Navigation()->Get_bFalling();

    _float3 fPos ={ XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };

    _float Y = m_pParentObject->Get_Navigation()->Compute_HeightOnCell(&fPos);

    m_pParentObject->Get_Transform()->Set_MoveSpeed(8.f);
    m_pParentObject->Set_onCell(false);
    
    if (true == isFall && false == m_pParentObject->GetTriggerFlag(CActor::Trigger_Terrain))
    {
        m_pParentObject->Get_Transform()->Go_Move(CTransform::DOWN, fTimeDelta);
    }

    if (XMVectorGetY(vPos) > Y && false ==isFall)
    {
       m_pParentObject->Get_Transform()->Go_Move(CTransform::DOWN, fTimeDelta);
    }
    
    if (1.f == m_pParentPartObject->Get_threshold())
        m_pParentObject->Set_Dead(true);

     return __super::StateMachine_Playing(fTimeDelta,pRim);
}      

void CJetFly_Dead::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
}

CJetFly_Dead* CJetFly_Dead::Create(void* pArg)
{
    CJetFly_Dead* pInstance = new CJetFly_Dead();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CJetFly_Dead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetFly_Dead::Free()
{
	__super::Free();
}