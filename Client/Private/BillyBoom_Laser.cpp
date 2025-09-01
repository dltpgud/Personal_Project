#include"stdafx.h"
#include "BillyBoom_Laser.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "BossBullet_Berrle.h"

CBillyBoom_Laser::CBillyBoom_Laser()
{
}

HRESULT CBillyBoom_Laser::Initialize(void* pArg)
{
    ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);
    m_pPerantPartBonMatrix[LEFT_BONE] = pDesc->pPerantPartBonMatrix[LEFT_BONE];
    m_pPerantPartBonMatrix[RIGHT_BONE] = pDesc->pPerantPartBonMatrix[RIGHT_BONE];
    m_pPerantWorldMat = pDesc->pPerantWorldMat;
	__super::Initialize(pDesc);

    m_StateNodes.resize(ANIM_END);

	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 15;
    pNodeDesc.iNextStateIdx = LASER_PRESHOOT;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[LASER_IN] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 16;
    pNodeDesc.iNextStateIdx = LASER_SHOOTLOOP;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[LASER_PRESHOOT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 17;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.fPlayAniTime = 0.25;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[LASER_SHOOTLOOP] = CStateNode::Create(&pNodeDesc);

    m_iBoneIndex[RIGHT_BONE] = m_pParentModel->Get_BoneIndex("R_TopArm_04");
    m_iBoneIndex[LEFT_BONE] = m_pParentModel->Get_BoneIndex("L_TopArm_04");
   
	return S_OK;
}

CStateMachine::Result CBillyBoom_Laser::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    if (HasFlag(PAUSED))
        return Result::None;

    if (m_StateNodes.empty())
        return Result::None;

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
    
    if (m_iNextIndex < 0 || m_iNextIndex >= static_cast<_int>(m_StateNodes.size()))
    {
        Reset_StateMachine(pRim);
        SetFlag(FINISHED);
        return Result::Finished;
    }

    _matrix LaserMatrix = XMMatrixRotationZ(XMConvertToRadians(m_BeamZ)) * XMMatrixRotationY(XMConvertToRadians(-m_BeamY));

    if (m_iCurIndex == LASER_SHOOTLOOP)
    {
        m_BeamZ += fTimeDelta * 2.f;
        m_BeamY += fTimeDelta*50.f;
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
            Reset_StateMachine(pRim);
            SetFlag(FINISHED);
            return Result::Finished;
        }
    }

    m_pParentModel->Set_BoneUpdateMatrix(m_iBoneIndex[RIGHT_BONE], LaserMatrix);
    m_pParentModel->Set_BoneUpdateMatrix(m_iBoneIndex[LEFT_BONE], LaserMatrix);
    
    return Result::Running;
}

void CBillyBoom_Laser::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    m_fRimTimeSum = 0.f;
    m_pGameInstance->StopSound(&m_pChannel);
    m_pGameInstance->Play_Sound(L"ST_Enemy_Laser_Loop_Stop.ogg", nullptr, 1.f);
    m_BeamY  = {-45.f};
    m_BeamZ  = {20.f};
    __super::Reset_StateMachine(pRim);
}

void CBillyBoom_Laser::Init_CallBack_Func()
{
    m_pParentModel->Callback(16, 10, [&]() { Make_Laser(); });
}

HRESULT CBillyBoom_Laser::Make_Laser()
{
    m_pGameInstance->Play_Sound(L"ST_Enemy_Laser_Loop_Start.ogg", &m_pChannel, 1.f);
    m_pGameInstance->PlayBGM(&m_pChannel, L"ST_Enemy_Laser_Loop.ogg", 1.7f);

    _vector RHend_Local_Pos = {m_pPerantPartBonMatrix[RIGHT_BONE]->_41, m_pPerantPartBonMatrix[RIGHT_BONE]->_42 + 0.5f,
                               m_pPerantPartBonMatrix[RIGHT_BONE]->_43 + 3.f, m_pPerantPartBonMatrix[RIGHT_BONE]->_44};
    _vector LHend_Local_Pos = {m_pPerantPartBonMatrix[LEFT_BONE]->_41, m_pPerantPartBonMatrix[LEFT_BONE]->_42 + 0.5f,
                               m_pPerantPartBonMatrix[LEFT_BONE]->_43 + 3.f, m_pPerantPartBonMatrix[LEFT_BONE]->_44};

    _vector RHPos = XMVector3TransformCoord(RHend_Local_Pos, XMLoadFloat4x4(m_pPerantWorldMat));
    _vector LHPos = XMVector3TransformCoord(LHend_Local_Pos, XMLoadFloat4x4(m_pPerantWorldMat));

    CBossBullet_Berrle::CBossBullet_Berrle_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.iDamage = 10;
    Desc.vPos = RHPos;
    Desc.LaserRightLeft = true;
    Desc.LaserpParentMatrix = m_pPerantWorldMat;
    Desc.LaserpSocketMatrix = m_pPerantPartBonMatrix[RIGHT_BONE];
    Desc.iSkillType = CSkill::STYPE_LASER;
    Desc.iActorType = CSkill::BOSS_MONSTER;
    Desc.fClolor[CSkill::COLOR::CSTART] = {0.5f, 0.8f, 0.f, 0.5f};
    Desc.fClolor[CSkill::COLOR::CEND] = {1.f, 1.f, 1.f, 1.f};

    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                             L"Prototype GameObject_BossBullet_Berrle", &Desc);

    CBossBullet_Berrle::CBossBullet_Berrle_DESC Desc2{};
    Desc2.fSpeedPerSec = 20.f;
    Desc2.iDamage = 10;
    Desc2.vPos = LHPos;
    Desc2.LaserRightLeft = false;
    Desc2.LaserpParentMatrix = m_pPerantWorldMat;
    Desc2.LaserpSocketMatrix = m_pPerantPartBonMatrix[LEFT_BONE];
    Desc2.iSkillType = CSkill::STYPE_LASER;
    Desc2.iActorType = CSkill::BOSS_MONSTER;
    Desc2.fClolor[CSkill::COLOR::CSTART] = {0.5f, 0.8f, 0.f, 0.5f};
    Desc2.fClolor[CSkill::COLOR::CEND] = {1.f, 1.f, 1.f, 1.f};
    
    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                             L"Prototype GameObject_BossBullet_Berrle", &Desc2);
    return S_OK;
}

CBillyBoom_Laser* CBillyBoom_Laser::Create(void* pArg)
{
    CBillyBoom_Laser* pInstance = new CBillyBoom_Laser();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBillyBoom_Laser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBillyBoom_Laser::Free()
{
	__super::Free();
}