#include"stdafx.h"
#include "GunPawn_Attack.h"
#include "GunPawn.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Body_GunPawn.h"
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
    m_pGameInstance->Play_Sound(L"ST_Enemy_Rocket_Shoot.ogg", CSound::SOUND_EFFECT, 0.3f);
    _vector Hend_Local_Pos = {m_pParentBoneMat->_41-0.5f, m_pParentBoneMat->_42+0.1f, m_pParentBoneMat->_43,
                              m_pParentBoneMat->_44};

    _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(m_pParentWorldMat));

    _vector Dir = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION) -
                      m_pParentPartObject->Get_Transform()-> Get_TRANSFORM(CTransform::T_POSITION);

    _float m_pDamage = 10;
    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = Dir;
    Desc.vPos = vHPos;
    Desc.fDamage = &m_pDamage;
    Desc.iActorType = CSkill::MONSTER::TYPE_GUNPAWN;
    CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(L"Prototype GameObject_Bullet", &Desc);
    m_pGameInstance->Add_Clon_to_Layers(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"), pGameObject);

    return S_OK;
}

CStateMachine::Result CGunPawn_Attack::StateMachine_Playing(_float fTimeDelta)
{ 
  m_pParentObject->Get_Transform()->Rotation_to_Player(fTimeDelta);

  return  __super::StateMachine_Playing(fTimeDelta);
}

void CGunPawn_Attack::Reset_StateMachine()
{
    
   __super::Reset_StateMachine();
}

void CGunPawn_Attack::Init_CallBack_Func()
{
    m_pParentModel->Callback(1, 1,[this](){Make_Bullet();});
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