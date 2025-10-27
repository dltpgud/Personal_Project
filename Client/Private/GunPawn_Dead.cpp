#include"stdafx.h"
#include "GunPawn_Dead.h"
#include "GameInstance.h"
#include "Trigger.h"
#include "HealthBall.h"
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

void CGunPawn_Dead::Init_CallBack_Func()
{
    m_pParentModel->Callback(3, 2, [&]() { 
        
        _int iCount = static_cast<_int>(m_pGameInstance->Compute_Random(0.f, 3.f));

       _vector vPos = m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
        for (_int i = 0; i < iCount; i++)
        {
          CHealthBall::CHealthBall_DESC Desc{};
          Desc.vPos = XMVectorSet(XMVectorGetX(vPos),XMVectorGetY(vPos)+2.f, XMVectorGetZ(vPos),1.f);
          m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                                         TEXT("Prototype GameObject_HealthBall"), &Desc);
        }
    });
}

CStateMachine::Result CGunPawn_Dead::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    m_pParentPartObject->Get_DissolveDesc()->bDissolveState = true;
    _bool isFall = m_pParentObject->Get_Navigation()->Get_bFalling();
    
    if (m_iCurIndex != m_iNextIndex)
    {
        DECAL_DESC DecalDesc{};
        DecalDesc.bOnce = true;
        DecalDesc.bNormal = false;
        DecalDesc.fDepth = 1.f;
        DecalDesc.iTexIndex = 3;
        DecalDesc.fLifeTime = 2.f;
        DecalDesc.fSize = 1.f;
        DecalDesc.iType = DECAL_DESC::TYPE_BOX;
        DecalDesc.Key = TEXT("Base");
        DecalDesc.vPos = m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
        DecalDesc.vDir = XMVectorSet(0.f, -1, 0.f, 0.f);
        DecalDesc.bColActive = true;
        m_pParentObject->Set_DecalDesc(DecalDesc);
    }


    if (true == isFall && false == m_pParentObject->GetTriggerFlag(CTrigger::FLAG_LAVA))
    {
        m_pParentObject->Get_Transform()->Set_MoveSpeed(8.f);
        m_pParentObject->Set_onCell(false);
        m_pParentObject->Get_Transform()->Go_Move(CTransform::DOWN, fTimeDelta);
    }

    if (1.f == m_pParentPartObject->Get_DissolveDesc()->fDissolve_threshold)
        m_pParentObject->Set_LifeState(true);

    m_pParentPartObject->Get_DissolveDesc()->Check_DisslveSt(fTimeDelta);

    return __super::StateMachine_Playing(fTimeDelta,pRim);
}      
void CGunPawn_Dead::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
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