#include"stdafx.h"
#include "BillyBoom_ShockWave.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Body_BillyBoom.h"
CBillyBoom_ShockWave::CBillyBoom_ShockWave()
{
}

HRESULT CBillyBoom_ShockWave::Initialize(void* pArg)
{
    ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);
    m_pPerantPartBonMatrix = pDesc->pPerantPartBonMatrix;
    m_pPerantWorldMat = pDesc->pPerantWorldMat;

	__super::Initialize(pDesc);
    m_StateNodes.resize(ANIM_END);
	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 22;
    pNodeDesc.iNextStateIdx = SHOCKWAVE_PRESHOOT;
    pNodeDesc.bIsLoop = false;
    m_StateNodes [SHOCKWAVE_IN] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 24;
    pNodeDesc.iNextStateIdx = SHOCKWAVE_SHOOT;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[SHOCKWAVE_PRESHOOT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 25;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.fPlayAniTime = 0.5;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[SHOCKWAVE_SHOOT] = CStateNode::Create(&pNodeDesc);

	return S_OK;
}

CStateMachine::Result CBillyBoom_ShockWave::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
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

void CBillyBoom_ShockWave::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    m_fRimTimeSum = 0.f;
    m_pGameInstance->StopSound(&m_pChannel);
    __super::Reset_StateMachine(pRim);
}

void CBillyBoom_ShockWave::Init_CallBack_Func()
{
    m_pParentModel->Callback(22, 1, [&]()  { m_pGameInstance->Play_Sound(L"ST_BillyBoom_From_Two_Blaster_To_One.ogg", &m_pChannel, 0.5f); });

    m_pParentModel->Callback(25, 1, [&]() { Make_ShockWave(); });
}

HRESULT CBillyBoom_ShockWave::Make_ShockWave()
{
    m_pGameInstance->Play_Sound(L"ST_BillyBoom_Shockwave_Shoot.ogg", &m_pChannel, 0.5f);
    _vector SHend_Local_Pos = {m_pPerantPartBonMatrix->_41, m_pPerantPartBonMatrix->_42 + 1.f,
                               m_pPerantPartBonMatrix->_43, m_pPerantPartBonMatrix->_44};
    _vector vHPos = XMVector3TransformCoord(SHend_Local_Pos, XMLoadFloat4x4(m_pPerantWorldMat));

      _vector vTagetPos =  XMVectorSet(m_pPerantWorldMat->_41, m_pPerantWorldMat->_42, m_pPerantWorldMat->_43, 1.f) +
        XMVectorSet(m_pPerantWorldMat->_31, m_pPerantWorldMat->_32, m_pPerantWorldMat->_33, 0.f) * 25.f;

    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = vTagetPos;
    Desc.vPos = vHPos;
    Desc.iDamage = 10;
    Desc.iSkillType = CSkill::SKill::STYPE_SHOCKWAVE;
    Desc.iActorType = CSkill::BOSS_MONSTER;
    Desc.fScale = _float2{0.6f, 0.6f};
    Desc.fClolor[CSkill::COLOR::CSTART] = _float4(1.f, 1.f, 0.f, 0.f);
    Desc.fClolor[CSkill::COLOR::CEND]   = _float4(1.f, 0.f, 0.f, 1.f);
    Desc.fRadius = 0.5f;
    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                             L"Prototype GameObject_Bullet", &Desc);
    return S_OK;
}

CBillyBoom_ShockWave* CBillyBoom_ShockWave::Create(void* pArg)
{
    CBillyBoom_ShockWave* pInstance = new CBillyBoom_ShockWave();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBillyBoom_ShockWave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBillyBoom_ShockWave::Free()
{
	__super::Free();
}