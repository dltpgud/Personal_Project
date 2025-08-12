#include"stdafx.h"
#include "MecanoBot_Dead.h"
#include "MecanoBot.h"
#include "GameInstance.h"
#include "Body_MecanoBot.h"

CMecanoBot_Dead::CMecanoBot_Dead()
{
}

HRESULT CMecanoBot_Dead::Initialize(void* pArg)
{
    DEAD_DESC* pDesc = static_cast<DEAD_DESC*>(pArg);
	__super::Initialize(pDesc);

	CStateNode::STATENODE_DESC pNodeDesc{};
        pNodeDesc.iCurrentState = 10; // 16;
    pNodeDesc.bIsLoop = true;
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iNextStateIdx = 0;
    pNodeDesc.fPlayAniTime = 0.5f;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CMecanoBot_Dead::StateMachine_Playing(_float fTimeDelta)
{
    dynamic_cast<CBody_MecanoBot*>(m_pParentPartObject)->Set_DeadState(true);

    if (1.f == dynamic_cast<CBody_MecanoBot*>(m_pParentPartObject)->Get_threshold())
        m_pParentObject->Set_Dead(true);

     return  __super::StateMachine_Playing(fTimeDelta);
}      
void CMecanoBot_Dead::Reset_StateMachine()
{
   __super::Reset_StateMachine();
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