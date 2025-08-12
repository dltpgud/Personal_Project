#include"stdafx.h"
#include "GunPawn_Dead.h"
#include "GunPawn.h"
#include "GameInstance.h"
#include "Body_GunPawn.h"

CGunPawn_Dead::CGunPawn_Dead()
{
}

HRESULT CGunPawn_Dead::Initialize(void* pArg)
{
    DEAD_DESC* pDesc = static_cast<DEAD_DESC*>(pArg);

	__super::Initialize(pDesc);

	CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.iCurrentState = 3;
    pNodeDesc.bIsLoop = true;
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iNextStateIdx = 0;
    pNodeDesc.fPlayAniTime = 0.5f;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CGunPawn_Dead::StateMachine_Playing(_float fTimeDelta)
{
    dynamic_cast<CBody_GunPawn*>(m_pParentPartObject)->Set_DeadState(true);

    if (1.f == dynamic_cast<CBody_GunPawn*>(m_pParentPartObject)->Get_threshold())
        m_pParentObject->Set_Dead(true);

     return  __super::StateMachine_Playing(fTimeDelta);
}      
void CGunPawn_Dead::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

CGunPawn_Dead* CGunPawn_Dead::Create(void* pArg)
{
    CGunPawn_Dead* pInstance = new CGunPawn_Dead();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CGunPawn_Dead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunPawn_Dead::Free()
{
	__super::Free();
}