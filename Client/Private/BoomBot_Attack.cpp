#include"stdafx.h"
#include "BoomBot_Attack.h"
#include "GameInstance.h"
#include "Bullet.h"

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

CStateMachine::Result CBoomBot_Attack::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    if (*m_fLength > 30.f || *m_fLength < 15.f)
    {
        Reset_StateMachine(pRim);
        return Result::Finished;
    }

    m_pParentObject->Get_Transform()->Rotation_to_Player(fTimeDelta);

    return __super::StateMachine_Playing(fTimeDelta, pRim);
}

void CBoomBot_Attack::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    __super::Reset_StateMachine(pRim);
}

void CBoomBot_Attack::Init_CallBack_Func()
{
    m_pParentModel->Callback(8, 0, [&]() { Make_Bullet(); });
}

void CBoomBot_Attack::Make_Bullet()
{
    m_pGameInstance->Play_Sound(L"ST_MortarPod_Shoot.ogg", &m_pChannel, 1.f);
    m_pGameInstance->SetChannelVolume( &m_pChannel, 60.f, m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION) -
            m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));

    _vector Hend_Local_Pos = {m_pParentBoneMat->_41, m_pParentBoneMat->_42, m_pParentBoneMat->_43,
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
    Desc.iDamage = 2;
    Desc.iActorType = CSkill::NOMAL_MONSTER;
    Desc.iSkillType = CSkill::STYPE_STURN;
    Desc.fScale     = _float2{0.2f, 0.2f};
    Desc.fClolor[CSkill::COLOR::CSTART] = _float4(0.f, 0.f, 0.f, 0.f);
    Desc.fClolor[CSkill::COLOR::CEND]   = _float4(1.f, 0.1f, 1.f, 1.f); 

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