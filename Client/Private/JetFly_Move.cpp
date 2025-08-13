#include"stdafx.h"
#include "JetFly_Move.h"
#include "JetFly.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Body_JetFly.h"

CJetFly_Move::CJetFly_Move()
{
}

HRESULT CJetFly_Move::Initialize(void* pArg)
{
    MOVE_DESC* pDesc = static_cast<MOVE_DESC*>(pArg);
    m_fLength = pDesc->fLength;
	__super::Initialize(pDesc);

	m_StateNodes.resize(ANIM_END);

	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 6;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[GO] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 5;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[BACK] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 7;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[LEFT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 8;
    pNodeDesc.iNextStateIdx = -1;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[RIGHT] = CStateNode::Create(&pNodeDesc);

   
	return S_OK;
}

CStateMachine::Result CJetFly_Move::StateMachine_Playing(_float fTimeDelta)
{
    if (HasFlag(PAUSED))
        return Result::None;

    if (m_StateNodes.empty())
        return Result::None;

    if (m_iNextIndex < 0 || m_iNextIndex >= static_cast<_int>(m_StateNodes.size()))
    {
        Reset_StateMachine();
        return Result::Finished;
    }
    if (*m_fLength < 15.f)
    {

        m_iNextIndex = BACK;
        m_pParentObject->Get_Transform()->Go_Move(CTransform::BACK, fTimeDelta,
                                                  dynamic_cast<CJetFly*>(m_pParentObject)->Get_Navi());
    }

    if (m_iCurIndex != m_iNextIndex)
    {
        m_pGameInstance->Play_Sound(L"ST_Enemy_Move_Fly_Loop.ogg", CSound::SOUND_MONSTER_FLY, 0.2f);
       m_iCurIndex = m_iNextIndex;
       m_StateNodes[m_iCurIndex]->State_Enter();
    }
    if (*m_fLength > 20.f)
    {
        if (m_pGameInstance->Get_Player()->Get_onCell())
        {
            m_pGameInstance->Add_Job(
                [this]()
                {
                    dynamic_cast<CJetFly*>(m_pParentObject)
                        ->Set_Taget(
                            m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
                });
            if (m_pParentObject->Get_Transform()->FollowPath(dynamic_cast<CJetFly*>(m_pParentObject)->Get_Navi(),
                                                             fTimeDelta))
            {

                Reset_StateMachine();
                return Result::Finished;
            }
        }
        else
            m_pParentObject->Get_Transform()->Go_Move(CTransform::GO, fTimeDelta,
                                                      dynamic_cast<CJetFly*>(m_pParentObject)->Get_Navi());
    }
    else if (*m_fLength <= 20.f && *m_fLength >= 15.f)
    {
       Reset_StateMachine();
       return Result::Finished;
    }
  
    if( m_StateNodes[m_iCurIndex]->State_Processing(fTimeDelta))
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

void CJetFly_Move::Reset_StateMachine()
{
    m_pGameInstance->StopSound(CSound::SOUND_MONSTER_ROLL2);
   __super::Reset_StateMachine();
}

CJetFly_Move* CJetFly_Move::Create(void* pArg)
{
    CJetFly_Move* pInstance = new CJetFly_Move();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CJetFly_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetFly_Move::Free()
{
	__super::Free();
}