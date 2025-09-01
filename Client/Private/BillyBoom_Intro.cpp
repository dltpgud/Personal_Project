#include"stdafx.h"
#include "BillyBoom_Intro.h"
#include "BillyBoom.h"
#include "GameInstance.h"
#include "BossIntroBG.h"
#include "SceneCamera.h"
#include "Camera_Free.h"
#include "Fade.h"
CBillyBoom_Intro::CBillyBoom_Intro()
{
}

HRESULT CBillyBoom_Intro::Initialize(void* pArg)
{
    ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);
    m_iEmissiveMashNum = pDesc->iEmissiveMashNum;
    m_fEmissivePower = pDesc->fEmissivePower;
    m_fEmissiveColor = pDesc->fEmissiveColor;
	__super::Initialize(pDesc);

	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 14;
    pNodeDesc.bIsLoop = false;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CBillyBoom_Intro::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
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
             if ( m_pGameInstance->Find_Clone_UIObj(TEXT("Boss_IntroBG"))==nullptr)
             {
                 Set_Setting();

                 Reset_StateMachine(pRim);
                 SetFlag(FINISHED);
                 return Result::Finished;
             }
         }
    }

   return Result::Running;;
}

void CBillyBoom_Intro::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    *m_iEmissiveMashNum = 0;
    *m_fEmissivePower = 0.f;
    *m_fEmissiveColor = {0.f, 0.f, 0.f}; 
    static_cast<CBillyBoom*>(m_pParentObject)->Set_bFinishIntro(true);
    __super::Reset_StateMachine(pRim);
}

void CBillyBoom_Intro::Init_CallBack_Func()
{
    m_pParentModel->Callback(14, 1, [&]() {
          
        _vector vWorldPos = m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
        CBossIntroBG::CBossIntroBG_DESC IntroDesc{};
        IntroDesc.fX = XMVectorGetX(vWorldPos);
        IntroDesc.fY = XMVectorGetY(vWorldPos);
        IntroDesc.fZ = XMVectorGetZ(vWorldPos);

        if (FAILED(m_pGameInstance->Add_UI_To_CLone( TEXT("Boss_IntroBG"),L"Prototype_GameObject_Boss_InstroUI", &IntroDesc)))
            return;
    });

    m_pParentModel->Callback(14, 29, [this]() { 
         *m_iEmissiveMashNum = 1;
         *m_fEmissivePower = 5.f;
         *m_fEmissiveColor = {1.f, 0.5f, 0.0}; 
    });
}

void CBillyBoom_Intro::Set_Setting()
{
    m_pGameInstance->Set_OpenUI(true, TEXT("PlayerHP"));

    m_pGameInstance->Set_OpenUI(true, TEXT("WeaponUI"));

    m_pGameInstance->Set_OpenUI(true, TEXT("WeaPon_Aim"));

    m_pGameInstance->Set_OpenUI(true, TEXT("BossHP"));
  
    dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_CloneGameObject(LEVEL_STATIC, TEXT("Layer_Camera"), GAMEOBJ_TYPE:: CAMERA))
        ->Set_Update(true);

    dynamic_cast<CSceneCamera*>(m_pGameInstance->Find_CloneGameObject(LEVEL_BOSS, TEXT("Layer_Camera"), GAMEOBJ_TYPE::CAMERA))
        ->Set_Dead(true);

    dynamic_cast<CFade*>(m_pGameInstance->Find_Clone_UIObj(L"Fade"))->Set_Fade(false);
}

CBillyBoom_Intro* CBillyBoom_Intro::Create(void* pArg)
{
    CBillyBoom_Intro* pInstance = new CBillyBoom_Intro();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBillyBoom_Intro");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBillyBoom_Intro::Free()
{
	__super::Free();
}