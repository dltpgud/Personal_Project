#include"stdafx.h"
#include "BillyBoom_Barre.h"
#include "BillyBoom.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Body_BillyBoom.h"
#include "BossBullet_Berrle.h"
CBillyBoom_Barre::CBillyBoom_Barre()
{
}

HRESULT CBillyBoom_Barre::Initialize(void* pArg)
{
    ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);
    m_pPerantPartBonMatrix = pDesc->pPerantPartBonMatrix;
    m_pPerantWorldMat = pDesc->pPerantWorldMat;
	__super::Initialize(pDesc);

    m_StateNodes.resize(ANIM_END);

	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 8;
    pNodeDesc.iNextStateIdx = BARRE_PRESHOOT;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[BARRE_IN] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 9;
    pNodeDesc.iNextStateIdx = BARRE_SHOOT;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[BARRE_PRESHOOT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 10;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[BARRE_SHOOT] = CStateNode::Create(&pNodeDesc);

	return S_OK;
}

CStateMachine::Result CBillyBoom_Barre::StateMachine_Playing(_float fTimeDelta)
{
   return __super::StateMachine_Playing(fTimeDelta);
}

void CBillyBoom_Barre::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

void CBillyBoom_Barre::Init_CallBack_Func()
{
    m_pParentModel->Callback(8, 1, [this](){ m_pGameInstance->Play_Sound(L"ST_BillyBoom_Cast_Hand_Blast.ogg", CSound::SOUND_EFFECT, 0.5f);
                                             dynamic_cast<CBody_BillyBoom*>(m_pParentPartObject)->SetDir(); });

    m_pParentModel->Callback(10, 5,[this](){ Make_Barre();});
}

HRESULT CBillyBoom_Barre::Make_Barre()
{
    _vector BHend_Local_Pos = {m_pPerantPartBonMatrix->_41, m_pPerantPartBonMatrix->_42, m_pPerantPartBonMatrix->_43,
                               m_pPerantPartBonMatrix->_44};

    _vector vHPos = XMVector3TransformCoord(BHend_Local_Pos, XMLoadFloat4x4(m_pPerantWorldMat));

    m_pGameInstance->Play_Sound(L"ST_BillyBoom_Hand_Blast_Shoot.ogg", CSound::SOUND_EFFECT, 0.5f);
    CBossBullet_Berrle::CBossBullet_Berrle_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = static_cast<CBody_BillyBoom*>(m_pParentPartObject)->Get_Dir();
    Desc.vPos = vHPos;
    // Desc.state = &m_iCurMotion;
    Desc.iSkillType = CSkill::SKill::STYPE_BERRLE;
    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                             L"Prototype GameObject_BossBullet_Berrle", &Desc);

    CBullet::CBULLET_DESC BDesc{};
    BDesc.fSpeedPerSec = 20.f;
    BDesc.pTagetPos = dynamic_cast<CBody_BillyBoom*>(m_pParentPartObject)->Get_Dir();
    BDesc.vPos = vHPos;
    // BDesc.state = m_iCurMotion;
    BDesc.fDamage = &m_pDamage;
    BDesc.iActorType = CSkill::MONSTER::TYPE_BILLYBOOM;

    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                             L"Prototype GameObject_Bullet", &BDesc);

    return S_OK;
}

CBillyBoom_Barre* CBillyBoom_Barre::Create(void* pArg)
{
    CBillyBoom_Barre* pInstance = new CBillyBoom_Barre();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBillyBoom_Barre");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBillyBoom_Barre::Free()
{
	__super::Free();
}