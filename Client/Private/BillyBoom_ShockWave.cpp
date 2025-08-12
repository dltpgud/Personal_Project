#include"stdafx.h"
#include "BillyBoom_ShockWave.h"
#include "BillyBoom.h"
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

CStateMachine::Result CBillyBoom_ShockWave::StateMachine_Playing(_float fTimeDelta)
{
    return __super::StateMachine_Playing(fTimeDelta);
}

void CBillyBoom_ShockWave::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

void CBillyBoom_ShockWave::Init_CallBack_Func()
{
    m_pParentModel->Callback(22, 1,
                             [this]()
                             {
                                 m_pGameInstance->Play_Sound(L"ST_BillyBoom_From_Two_Blaster_To_One.ogg",CSound::SOUND_EFFECT, 0.5f);
                                 dynamic_cast<CBody_BillyBoom*>(m_pParentPartObject)->SetDir();
                             });

    m_pParentModel->Callback(25, 1, [this](){Make_ShockWave();});
}

HRESULT CBillyBoom_ShockWave::Make_ShockWave()
{
    m_pGameInstance->Play_Sound(L"ST_BillyBoom_Shockwave_Shoot.ogg", CSound::SOUND_EFFECT, 0.5f);
    _vector SHend_Local_Pos = {m_pPerantPartBonMatrix->_41, m_pPerantPartBonMatrix->_42 + 1.f,
                               m_pPerantPartBonMatrix->_43, m_pPerantPartBonMatrix->_44};
    _vector vHPos = XMVector3TransformCoord(SHend_Local_Pos, XMLoadFloat4x4(m_pPerantWorldMat));

    m_pDamage = 10.f;
    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = dynamic_cast<CBody_BillyBoom*>(m_pParentPartObject)->Get_Dir();
    Desc.vPos = vHPos;
 
    Desc.fDamage = &m_pDamage;
    Desc.iSkillType = CSkill::SKill::STYPE_SHOCKWAVE;

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