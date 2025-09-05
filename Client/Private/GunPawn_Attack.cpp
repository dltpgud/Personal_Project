#include"stdafx.h"
#include "GunPawn_Attack.h"
#include "GameInstance.h"
#include "Bullet.h"

CGunPawn_Attack::CGunPawn_Attack()
{
}

HRESULT CGunPawn_Attack::Initialize(void* pArg)
{
    ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);
    m_fLength = pDesc->fLength;
    m_pParentBoneMat = pDesc->pParentBoneMat;
    m_pParentWorldMat = pDesc->pParentWorldMat;
	__super::Initialize(pDesc);

    m_StateNodes.resize(ANIM_END);

	CStateNode::STATENODE_DESC pNodeDesc{};
    pNodeDesc.pParentModel = m_pParentModel;
    pNodeDesc.iCurrentState = 0;
    pNodeDesc.iNextStateIdx = 1;
    pNodeDesc.fPlayAniTime = 1.f;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[GRENADE_PRESHOOT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 1;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.fPlayAniTime = 0.7f;
    pNodeDesc.bIsLoop = false;
    m_StateNodes[GRENADE_SHOOT] = CStateNode::Create(&pNodeDesc);

	return S_OK;
}

HRESULT CGunPawn_Attack::Make_Bullet()
{
    m_pGameInstance->Play_Sound(L"ST_Enemy_Rocket_Shoot.ogg", &m_pChannel, 1.f);
    m_pGameInstance->SetChannelVolume( &m_pChannel, 60.f,
    m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION) - m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
    
    _vector Hend_Local_Pos = {m_pParentBoneMat->_41-0.5f, m_pParentBoneMat->_42+0.1f, m_pParentBoneMat->_43,
                              m_pParentBoneMat->_44};

    _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(m_pParentWorldMat));

    _vector Dir = XMVectorSet(m_pParentWorldMat->_41, m_pParentWorldMat->_42, m_pParentWorldMat->_43, 1.f) +
                  XMVectorSet(m_pParentWorldMat->_31, m_pParentWorldMat->_32, m_pParentWorldMat->_33, 0.f) * 20.f; 
   Dir = XMVectorSetY(Dir, 2.f);
    if (m_pParentObject->IsLookAtPlayer(15.f))
        Dir = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = Dir;
    Desc.vPos = vHPos;
    Desc.iDamage = 4;
    Desc.iActorType = CSkill::NOMAL_MONSTER;
    Desc.iSkillType = CSkill::STYPE_NOMAL;
    Desc.fScale = _float2{0.2f, 0.2f};
    Desc.fClolor[CSkill::COLOR::CSTART] = _float4(0.f, 0.f, 0.f, 0.f);
    Desc.fClolor[CSkill::COLOR::CEND] = _float4(1.f, 0.f, 0.f, 1.f);
    CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(L"Prototype GameObject_Bullet", &Desc);
    m_pGameInstance->Add_Clon_to_Layers(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"), pGameObject);

    return S_OK;
}

CStateMachine::Result CGunPawn_Attack::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    m_pParentObject->Get_Transform()->Rotation_to_Player(fTimeDelta);

  return __super::StateMachine_Playing(fTimeDelta, pRim);
}

void CGunPawn_Attack::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
}

void CGunPawn_Attack::Init_CallBack_Func()
{
    m_pParentModel->Callback(1, 1, [&]() { Make_Bullet(); });
}

CGunPawn_Attack* CGunPawn_Attack::Create(void* pArg)
{
    CGunPawn_Attack* pInstance = new CGunPawn_Attack();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CGunPawn_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunPawn_Attack::Free()
{
	__super::Free();
}