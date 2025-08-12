#include"stdafx.h"
#include "JetFly_Attack.h"
#include "JetFly.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Body_JetFly.h"
CJetFly_Attack::CJetFly_Attack()
{
}

HRESULT CJetFly_Attack::Initialize(void* pArg)
{
    ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);
    m_fLength = pDesc->fLength;
    m_pPerantPartBonMatrix = pDesc->pPerantPartBonMatrix;
    m_pPerantWorldMat = pDesc->pPerantWorldMat;
	__super::Initialize(pDesc);

    m_StateNodes.resize(ANIM_END);

	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 0;
    pNodeDesc.bIsLoop = false;
    pNodeDesc.fPlayAniTime = 0.5f;
    pNodeDesc.iNextStateIdx = BARRELROLL_RIGHT;
    m_StateNodes[BARRELROLL_LEFT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 1;
    pNodeDesc.bIsLoop = false;
    pNodeDesc.fPlayAniTime = 0.5f;
    pNodeDesc.iNextStateIdx = SHOOT;
    m_StateNodes[BARRELROLL_RIGHT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 3;
    pNodeDesc.bIsLoop = true;
    pNodeDesc.fPlayAniTime = 0.5f;
    m_StateNodes[SHOOT] = CStateNode::Create(&pNodeDesc);
	return S_OK;
}

CStateMachine::Result CJetFly_Attack::StateMachine_Playing(_float fTimeDelta)
{
    if (*m_fLength > 30.f || *m_fLength < 15.f)
    {
       Reset_StateMachine();
       return Result::Finished;
    }

    m_pParentObject->Get_Transform()->Rotation_to_Player(fTimeDelta);
   
    return __super::StateMachine_Playing(fTimeDelta);
}

void CJetFly_Attack::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

void CJetFly_Attack::Init_CallBack_Func()
{
    m_pParentModel->Callback(3, 5, [this]() { Make_Bullet(); });
    m_pParentModel->Callback(0, 5, [this]() { Make_Bullet(); });
    m_pParentModel->Callback(1, 5, [this]() { Make_Bullet(); });
}

void CJetFly_Attack::Make_Bullet()
{
          m_pGameInstance->Play_Sound(L"ST_FlashFly_Shoot_A.ogg", CSound::SOUND_EFFECT, 0.3f);
    _vector Hend_Local_Pos = {m_pPerantPartBonMatrix->_41, m_pPerantPartBonMatrix->_42, m_pPerantPartBonMatrix->_43,
                              m_pPerantPartBonMatrix->_44};

    _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(m_pPerantWorldMat));

    _vector Dir = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION) -
        dynamic_cast<CBody_JetFly*>(m_pParentPartObject)->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

    _float m_pDamage = 3.f;
    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = Dir;
    Desc.vPos = vHPos;
    Desc.fDamage = &m_pDamage;
    Desc.iActorType = CSkill::MONSTER::TYPE_JETFLY;
    CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(L"Prototype GameObject_Bullet", &Desc);
    m_pGameInstance->Add_Clon_to_Layers(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"), pGameObject);
}

CJetFly_Attack* CJetFly_Attack::Create(void* pArg)
{
    CJetFly_Attack* pInstance = new CJetFly_Attack();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMecanoBot_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetFly_Attack::Free()
{
	__super::Free();
}