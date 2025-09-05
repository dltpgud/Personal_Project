#include"stdafx.h"
#include "MecanoBot_Dead.h"
#include "GameInstance.h"
#include "Trigger.h"
CMecanoBot_Dead::CMecanoBot_Dead()
{
}

HRESULT CMecanoBot_Dead::Initialize(void* pArg)
{
    DEAD_DESC* pDesc = static_cast<DEAD_DESC*>(pArg);
	__super::Initialize(pDesc);

	CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.iCurrentState = 10; 
    pNodeDesc.bIsLoop = true;
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iNextStateIdx = 0;
    pNodeDesc.fPlayAniTime = 0.5f;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CMecanoBot_Dead::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    m_pParentPartObject->Set_DeadState(true);

     _bool isFall = m_pParentObject->Get_Navigation()->Get_bFalling();
    if (true == isFall && false == m_pParentObject->GetTriggerFlag(CTrigger::FLAG_LAVA))
    {
        m_pParentObject->Get_Transform()->Set_MoveSpeed(8.f);
        m_pParentObject->Set_onCell(false);
        m_pParentObject->Get_Transform()->Go_Move(CTransform::DOWN, fTimeDelta);
    }
    if (1.f == m_pParentPartObject->Get_threshold())
        m_pParentObject->Set_Dead(true);

   return __super::StateMachine_Playing(fTimeDelta, pRim);
}      
void CMecanoBot_Dead::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
}

CMecanoBot_Dead* CMecanoBot_Dead::Create(void* pArg)
{
    CMecanoBot_Dead* pInstance = new CMecanoBot_Dead();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMecanoBot_Dead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMecanoBot_Dead::Free()
{
	__super::Free();
}