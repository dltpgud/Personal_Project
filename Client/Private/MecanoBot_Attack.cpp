#include"stdafx.h"
#include "MecanoBot_Attack.h"
#include "MecanoBot.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Body_MecanoBot.h"
CMecanoBot_Attack::CMecanoBot_Attack()
{
}

HRESULT CMecanoBot_Attack::Initialize(void* pArg)
{
    ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);
    m_fLength = pDesc->fLength;
    m_pParentBoneMat = pDesc->pParentBoneMat;
    m_pParentWorldMat = pDesc->pParentWorldMat;
	__super::Initialize(pDesc);

	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 9;
    pNodeDesc.bIsLoop = true;
    m_StateNodes.push_back(CStateNode::Create(&pNodeDesc));
  
	return S_OK;
}

CStateMachine::Result CMecanoBot_Attack::StateMachine_Playing(_float fTimeDelta)
{
    if (*m_fLength > 30.f || *m_fLength < 15.f)
    {
        Reset_StateMachine();
        return Result::Finished;
    }
    
    m_pParentObject->Get_Transform()->Rotation_to_Player(fTimeDelta);


  return  __super::StateMachine_Playing(fTimeDelta);
}

void CMecanoBot_Attack::Reset_StateMachine()
{
   __super::Reset_StateMachine();
}

void CMecanoBot_Attack::Init_CallBack_Func()
{
    m_pParentModel->Callback(9, 3,[this](){Make_Bullet();});
}

void CMecanoBot_Attack::Make_Bullet()
{
    m_pGameInstance->Play_Sound(L"ST_MortarPod_Shoot.ogg", CSound::SOUND_EFFECT, 0.3f);
    _vector Hend_Local_Pos = {m_pParentBoneMat->_41, m_pParentBoneMat->_42, m_pParentBoneMat->_43,
                              m_pParentBoneMat->_44};

    _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(m_pParentWorldMat));

    _vector Dir = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION) -
                  m_pParentPartObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
    _float m_pDamage = 10;
    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = Dir;
    Desc.vPos = vHPos;
    Desc.fDamage = &m_pDamage;
    Desc.iActorType = CSkill::MONSTER::TYPE_MECANOBOT;
    CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(L"Prototype GameObject_Bullet", &Desc);
    m_pGameInstance->Add_Clon_to_Layers(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"), pGameObject);

}

CMecanoBot_Attack* CMecanoBot_Attack::Create(void* pArg)
{
    CMecanoBot_Attack* pInstance = new CMecanoBot_Attack();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMecanoBot_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMecanoBot_Attack::Free()
{
	__super::Free();
}