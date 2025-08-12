#include"stdafx.h"
#include "BillyBoom_Move.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Body_BillyBoom.h"
#include "BillyBoom.h"
CBillyBoom_Move::CBillyBoom_Move()
{
}

HRESULT CBillyBoom_Move::Initialize(void* pArg)
{
    MOVE_DESC* pDesc = static_cast<MOVE_DESC*>(pArg);
    m_fLength = pDesc->fLength;
	__super::Initialize(pDesc);

	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 21;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = true;
    m_StateNodes.push_back( CStateNode::Create(&pNodeDesc));

	return S_OK;
}

CStateMachine::Result CBillyBoom_Move::StateMachine_Playing(_float fTimeDelta)
{
    if (HasFlag(PAUSED))
        return Result::None;

    if (m_StateNodes.empty())
        return Result::None;

    if (m_iNextIndex < 0 || m_iNextIndex >= static_cast<_int>(m_StateNodes.size()))
    {
        Reset_StateMachine();
        SetFlag(FINISHED);
        return Result::Finished;
    }

    if (m_iCurIndex != m_iNextIndex)
    {
        m_pGameInstance->PlayBGM(CSound::SOUND_MONSTER_SETB, L"ST_BillyBoom_FootStep.ogg", 0.5f);
        static_cast<CBillyBoom*>(m_pParentObject)->Set_bSkill(false);
        m_iCurIndex = m_iNextIndex;
        m_StateNodes[m_iCurIndex]->State_Enter();
    }

    m_pParentObject->Get_Transform()->Rotation_to_Player(fTimeDelta);
    m_pParentObject->Get_Transform()->Go_Move(CTransform::GO, fTimeDelta,
                                              static_cast<CBillyBoom*>(m_pParentObject)->Get_Navi());
  
    if (*m_fLength < 25.f)
    {
        static_cast<CBody_BillyBoom*>(m_pParentPartObject)->ChangeState(static_cast<CBillyBoom*>(m_pParentObject)->Get_NextSkil());
        Reset_StateMachine();
        SetFlag(FINISHED);
        return Result::None;
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
            Reset_StateMachine();
            SetFlag(FINISHED);
            return Result::Finished;
        }
    }

    return Result::Running;
}

void CBillyBoom_Move::Reset_StateMachine()
{
    m_pGameInstance->StopSound(CSound::SOUND_MONSTER_SETB);
    static_cast<CBillyBoom*>(m_pParentObject)->Set_bSkill(true);
   __super::Reset_StateMachine();
}

CBillyBoom_Move* CBillyBoom_Move::Create(void* pArg)
{
    CBillyBoom_Move* pInstance = new CBillyBoom_Move();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBillyBoom_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBillyBoom_Move::Free()
{
	__super::Free();
}