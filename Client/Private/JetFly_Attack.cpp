#include"stdafx.h"
#include "JetFly_Attack.h"
#include "GameInstance.h"
#include "Bullet.h"

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

CStateMachine::Result CJetFly_Attack::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    if (*m_fLength > 30.f || *m_fLength < 15.f)
    {
        Reset_StateMachine(pRim);
       return Result::Finished;
    }

    m_pParentObject->Get_Transform()->Rotation_to_Player(fTimeDelta);
   
    return __super::StateMachine_Playing(fTimeDelta,pRim);
}

void CJetFly_Attack::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
}

void CJetFly_Attack::Init_CallBack_Func()
{
    m_pParentModel->Callback(3, 5, [&]() { Make_Bullet(); });
    m_pParentModel->Callback(0, 5, [&]() { Make_Bullet(); });
    m_pParentModel->Callback(1, 5, [&]() { Make_Bullet(); });
}

void CJetFly_Attack::Make_Bullet()
{
    m_pGameInstance->Play_Sound(L"ST_FlashFly_Shoot_A.ogg", &m_pChannel, 1.f);
    m_pGameInstance->SetChannelVolume( &m_pChannel, 60.f, m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION) - m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
   
    _vector Hend_Local_Pos = {m_pPerantPartBonMatrix->_41, m_pPerantPartBonMatrix->_42, m_pPerantPartBonMatrix->_43,
                              m_pPerantPartBonMatrix->_44};

    _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(m_pPerantWorldMat));

    _vector vPos = XMVectorSet(m_pPerantWorldMat->_41, m_pPerantWorldMat->_42, m_pPerantWorldMat->_43, 1.f);
    _vector vLook = XMVectorSet(m_pPerantWorldMat->_31, m_pPerantWorldMat->_32, m_pPerantWorldMat->_33, 0.f);

    _vector Dir = vPos + vLook * 20.f; 

      Dir = XMVectorSetY(Dir, 0.f);

     if (m_pParentObject->IsLookAtPlayer(15.f))
          Dir = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = Dir;
    Desc.vPos = vHPos;
    Desc.iDamage = 2;
    Desc.iActorType = CSkill::NOMAL_MONSTER;
    Desc.iSkillType = CSkill::STYPE_NOMAL;
    Desc.fScale = _float2{0.2f, 0.2f};
    Desc.fClolor[CSkill::COLOR::CSTART] = _float4(0.f, 0.f, 0.f, 0.f);
    Desc.fClolor[CSkill::COLOR::CEND] = _float4(1.f, 0.f, 0.f, 1.f);

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