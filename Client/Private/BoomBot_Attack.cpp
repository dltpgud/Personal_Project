#include"stdafx.h"
#include "BoomBot_Attack.h"
#include "BoomBot.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Body_BoomBot.h"

CBoomBot_Attack::CBoomBot_Attack()
{
}

HRESULT CBoomBot_Attack::Initialize(void* pArg)
{
    ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);
    m_fLength = pDesc->fLength;
    m_pParentBoneMat = pDesc->pParentBoneMat;
    m_pParentWorldMat = pDesc->pParentWorldMat;
	__super::Initialize(pDesc);

	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 8;
    pNodeDesc.bIsLoop = true;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CBoomBot_Attack::StateMachine_Playing(_float fTimeDelta)
{
    if (*m_fLength > 30.f || *m_fLength < 15.f)
    {
        Reset_StateMachine();
        return Result::Finished;
    }
    
    m_pParentObject->Get_Transform()->Rotation_to_Player(fTimeDelta);


  return  __super::StateMachine_Playing(fTimeDelta);
}

void CBoomBot_Attack::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

void CBoomBot_Attack::Init_CallBack_Func()
{
   m_pParentModel->Callback(8, 0,[this](){Make_Bullet();});
}

void CBoomBot_Attack::Make_Bullet()
{
    m_pGameInstance->Play_Sound(L"ST_MortarPod_Shoot.ogg", CSound::SOUND_EFFECT, 0.3f);
    _vector Hend_Local_Pos = {m_pParentBoneMat->_41, m_pParentBoneMat->_42, m_pParentBoneMat->_43,
                              m_pParentBoneMat->_44};

    _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(m_pParentWorldMat));

    _vector Dir = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION) -
                  m_pParentPartObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
    _float m_pDamage = 2.f;
    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = Dir;
    Desc.vPos = vHPos;
    Desc.fDamage = &m_pDamage;
    Desc.iActorType = CSkill::MONSTER::TYPE_BOOMBOT;
    Desc.iSkillType = CSkill::STYPE_STURN;


    CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(L"Prototype GameObject_Bullet", &Desc);
    m_pGameInstance->Add_Clon_to_Layers(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"), pGameObject);
}

CBoomBot_Attack* CBoomBot_Attack::Create(void* pArg)
{
    CBoomBot_Attack* pInstance = new CBoomBot_Attack();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBoomBot_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoomBot_Attack::Free()
{
	__super::Free();
}