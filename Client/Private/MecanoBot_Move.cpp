#include"stdafx.h"
#include "MecanoBot_Move.h"
#include "GameInstance.h"
#include "Bullet.h"

CMecanoBot_Move::CMecanoBot_Move()
{
}

HRESULT CMecanoBot_Move::Initialize(void* pArg)
{
    MOVE_DESC* pDesc = static_cast<MOVE_DESC*>(pArg);
    m_fLength = pDesc->fLength;
	__super::Initialize(pDesc);

	m_StateNodes.resize(ANIM_END);

	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 4;
    pNodeDesc.iNextStateIdx = BACK;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[GO] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 3;
    pNodeDesc.iNextStateIdx = BACK;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[BACK] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 5;
    pNodeDesc.iNextStateIdx = BACK;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[LEFT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 7;
    pNodeDesc.iNextStateIdx = BACK;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[RIGHT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 1;
    pNodeDesc.iNextStateIdx = BACK;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[JETPACK] = CStateNode::Create(&pNodeDesc);
	return S_OK;
}

CStateMachine::Result CMecanoBot_Move::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
{
    if (HasFlag(PAUSED))
        return Result::None;

    if (m_StateNodes.empty())
        return Result::None;

    if (m_iNextIndex < 0 || m_iNextIndex >= static_cast<_int>(m_StateNodes.size()))
    {
        Reset_StateMachine(pRim);
        return Result::Finished;
    }

    if (*m_fLength < 15.f) {
    
    m_iNextIndex = BACK;
    }

    if (m_iCurIndex != m_iNextIndex)
    {
        if (m_iNextIndex == GO)
            m_pGameInstance->Play_Sound(L"ST_Enemy_Move_Roll2.ogg", &m_pChannel, 1.f, true);
        if (m_iNextIndex == BACK)
            m_pGameInstance->Play_Sound(L"ST_Enemy_Move_Roll.ogg", &m_pChannel, 1.f, true);

        m_iCurIndex = m_iNextIndex;
        m_StateNodes[m_iCurIndex]->State_Enter();
    }
     m_pGameInstance->SetChannelVolume( &m_pChannel, 80.f,
     m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION) - m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));

      _bool isFall = m_pParentObject->Get_Navigation()->Get_bFalling();

      if (true == isFall)
      {
          Reset_StateMachine(pRim);
          return Result::Finished;
      }

    if (*m_fLength > 20.f && false == isFall)
    {
        m_iCurIndex = GO;
        if (m_pGameInstance->Get_Player()->Get_onCell())
        {
            m_pGameInstance->Add_Job(
                [&]()
                {
                   m_pParentObject->Get_Navigation()
                        ->Set_Taget(m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
                });
            if (m_pParentObject->Get_Transform()->FollowPath(m_pParentObject->Get_Navigation(), fTimeDelta))
            {
                Reset_StateMachine(pRim);
                return Result::Finished;
            }
        }
        else
            m_pParentObject->Get_Transform()->Go_Move(CTransform::GO, fTimeDelta, m_pParentObject->Get_Navigation());
    }
    else if (*m_fLength <= 20.f && *m_fLength >= 15.f)
    {
        Reset_StateMachine(pRim);
        return Result::Finished;
    }

    if (m_iCurIndex == BACK && false == isFall)
    {
       m_pParentObject->Get_Transform()->Go_Move(CTransform::BACK, fTimeDelta, m_pParentObject->Get_Navigation());
       m_pParentObject->Get_Transform()->Rotation_to_Player(fTimeDelta);
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
            Reset_StateMachine(pRim);
            SetFlag(FINISHED);
            return Result::Finished;
        }
  }
    return Result::Running;
}

void CMecanoBot_Move::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    m_pGameInstance->StopSound(&m_pChannel);
    __super::Reset_StateMachine(pRim);
}

CMecanoBot_Move* CMecanoBot_Move::Create(void* pArg)
{
    CMecanoBot_Move* pInstance = new CMecanoBot_Move();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMecanoBot_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMecanoBot_Move::Free()
{
	__super::Free();
}