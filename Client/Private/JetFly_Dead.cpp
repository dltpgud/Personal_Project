#include"stdafx.h"
#include "JetFly_Dead.h"
#include "JetFly.h"
#include "GameInstance.h"
#include "Body_JetFly.h"

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

CStateMachine::Result CJetFly_Dead::StateMachine_Playing(_float fTimeDelta)
{
    dynamic_cast<CBody_JetFly*>(m_pParentPartObject)->Set_DeadState(true);

    _vector vPos =  m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

    _bool isFall = dynamic_cast<CJetFly*>(m_pParentObject)->Get_Navi()->ISFall();

    _float3 fPos ={ XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };

    _float Y = dynamic_cast<CJetFly*>(m_pParentObject)->Get_Navi()->Compute_HeightOnCell(&fPos);

    isFall == true ? Y = -7 : Y;

    if (XMVectorGetY(vPos) >=Y)
    {
        dynamic_cast<CJetFly*>(m_pParentObject)->Set_onCell(false);
        m_pParentObject->Get_Transform()->Set_MoveSpeed(10.f);
        m_pParentObject->Get_Transform()->Go_Move(CTransform::DOWN, fTimeDelta);
    }

    if (1.f == dynamic_cast<CBody_JetFly*>(m_pParentPartObject)->Get_threshold())
        m_pParentObject->Set_Dead(true);

     return  __super::StateMachine_Playing(fTimeDelta);
}      

void CJetFly_Dead::Reset_StateMachine()
{
   __super::Reset_StateMachine();
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