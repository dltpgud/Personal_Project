#include"stdafx.h"
#include "BoomBot_Dead.h"
#include "GameInstance.h"

CBoomBot_Dead::CBoomBot_Dead()
{
}

HRESULT CBoomBot_Dead::Initialize(void* pArg)
{
    DEAD_DESC* pDesc = static_cast<DEAD_DESC*>(pArg);

	__super::Initialize(pDesc);

	CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.iCurrentState =  11;
    pNodeDesc.bIsLoop = true;
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iNextStateIdx = 0;
    pNodeDesc.fPlayAniTime = 0.5f;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CBoomBot_Dead::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    m_pParentPartObject->Set_DeadState(true);
    
    if (1.f == m_pParentPartObject->Get_threshold())
        m_pParentObject->Set_Dead(true);

     return __super::StateMachine_Playing(fTimeDelta,pRim);
}      
void CBoomBot_Dead::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
}

CBoomBot_Dead* CBoomBot_Dead::Create(void* pArg)
{
    CBoomBot_Dead* pInstance = new CBoomBot_Dead();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBoomBot_Dead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoomBot_Dead::Free()
{
	__super::Free();
}