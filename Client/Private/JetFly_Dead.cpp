#include"stdafx.h"
#include "JetFly_Dead.h"
#include "GameInstance.h"
#include "Trigger.h"
#include "HealthBall.h"
CJetFly_Dead::CJetFly_Dead()
{
}

HRESULT CJetFly_Dead::Initialize(void* pArg)
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

void CJetFly_Dead::Init_CallBack_Func()
{
    m_pParentModel->Callback(
        10, 0,
        [&]()
        {
            _int iCount = static_cast<_int>(m_pGameInstance->Compute_Random(0.f, 3.f));

            _vector vPos = m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
            for (_int i = 0; i < iCount; i++)
            {
                CHealthBall::CHealthBall_DESC Desc{};
                Desc.vPos = vPos;
                m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                                         TEXT("Prototype GameObject_HealthBall"), &Desc);
            }
        });
}

CStateMachine::Result CJetFly_Dead::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    m_pParentPartObject->Get_DissolveDesc()->bDissolveState = true;

    _vector vPos =  m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

    _bool isFall = m_pParentObject->Get_Navigation()->Get_bFalling();

    _float3 fPos ={ XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };

    _float Y = m_pParentObject->Get_Navigation()->Compute_HeightOnCell(&fPos);

    m_pParentObject->Get_Transform()->Set_MoveSpeed(8.f);
    m_pParentObject->Set_onCell(false);
    
    if (true == isFall && false == m_pParentObject->GetTriggerFlag(CTrigger::FLAG_LAVA))
    {
        m_pParentObject->Get_Transform()->Go_Move(CTransform::DOWN, fTimeDelta);
    }

    if (XMVectorGetY(vPos) > Y && false ==isFall)
    {
       m_pParentObject->Get_Transform()->Go_Move(CTransform::DOWN, fTimeDelta);
    }
    
    if (1.f == m_pParentPartObject->Get_DissolveDesc()->fDissolve_threshold)
        m_pParentObject->Set_LifeState(OBJ_DEAD);

    m_pParentPartObject->Get_DissolveDesc()->Check_DisslveSt(fTimeDelta);

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