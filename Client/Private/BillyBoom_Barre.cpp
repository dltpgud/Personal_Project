#include"stdafx.h"
#include "BillyBoom_Barre.h"
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

CStateMachine::Result CBillyBoom_Barre::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    if (*pRim->eState == RIM_LIGHT_DESC::STATE_RIM)
    {
        pRim->iPower = 0.3f;
        pRim->fcolor = {1.f, 1.f, 1.f, 1.f};
        m_fRimTimeSum += fTimeDelta;
    }
    
    if (m_fRimTimeSum > 0.5f)
    {
        m_fRimTimeSum = 0.f;
        *pRim->eState = RIM_LIGHT_DESC::STATE_NORIM;
        pRim->fcolor = {0.f, 0.f, 0.f, 0.f};
    }
    return __super::StateMachine_Playing(fTimeDelta, pRim);
}

void CBillyBoom_Barre::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    m_fRimTimeSum = 0.f;
    m_pGameInstance->StopSound(&m_pChannel);
    __super::Reset_StateMachine(pRim);
}

void CBillyBoom_Barre::Init_CallBack_Func()
{
    m_pParentModel->Callback(8, 1, [&](){ m_pGameInstance->Play_Sound(L"ST_BillyBoom_Cast_Hand_Blast.ogg",  &m_pChannel, 0.5f); });

    m_pParentModel->Callback(10, 5, [&]() { Make_Barre(); });
}

HRESULT CBillyBoom_Barre::Make_Barre()
{
    _vector BHend_Local_Pos = {m_pPerantPartBonMatrix->_41, m_pPerantPartBonMatrix->_42, m_pPerantPartBonMatrix->_43,
                               m_pPerantPartBonMatrix->_44};

    _vector vHPos = XMVector3TransformCoord(BHend_Local_Pos, XMLoadFloat4x4(m_pPerantWorldMat));


  _vector vTagetPos =  XMVectorSet(m_pPerantWorldMat->_41, m_pPerantWorldMat->_42, m_pPerantWorldMat->_43, 1.f) +
        XMVectorSet(m_pPerantWorldMat->_31, m_pPerantWorldMat->_32, m_pPerantWorldMat->_33, 0.f) * 100.f;


    m_pGameInstance->Play_Sound(L"ST_BillyBoom_Hand_Blast_Shoot.ogg", &m_pChannel, 0.5f);
    CBossBullet_Berrle::CBossBullet_Berrle_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = vTagetPos;
    Desc.vPos = vHPos;
    Desc.iActorType = CSkill::BOSS_MONSTER;
    Desc.iSkillType = CSkill::SKill::STYPE_BERRLE;
    Desc.fClolor[CSkill::COLOR::CSTART] = {0.f, 0.f, 0.f, 0.5f};
    Desc.fClolor[CSkill::COLOR::CEND]   ={1.f, 0.f, 0.f, 1.f};
    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                             L"Prototype GameObject_BossBullet_Berrle", &Desc);

    CBullet::CBULLET_DESC BDesc{};
    BDesc.fSpeedPerSec = 20.f;
    BDesc.pTagetPos = vTagetPos;
    BDesc.vPos = vHPos;
    BDesc.iDamage = 12;
    BDesc.iActorType = CSkill::BOSS_MONSTER;
    BDesc.iSkillType = CSkill::STYPE_BERRLE;
    BDesc.fScale = _float2{1.f, 1.f};
    BDesc.fClolor[CSkill::COLOR::CSTART] = _float4(1.f, 0.f, 0.f, 1.f);
    BDesc.fClolor[CSkill::COLOR::CEND]   = _float4(1.f, 1.f, 0.f, 1.f);
    BDesc.fRadius = 1.25f;
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