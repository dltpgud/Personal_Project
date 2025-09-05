#include"stdafx.h"
#include "GunPawn_Move.h"
#include "GameInstance.h"
#include "Bullet.h"

CGunPawn_Move::CGunPawn_Move()
{
}

HRESULT CGunPawn_Move::Initialize(void* pArg)
{
    MOVE_DESC* pDesc = static_cast<MOVE_DESC*>(pArg);
    m_fLength = pDesc->fLength;
	__super::Initialize(pDesc);

	m_StateNodes.resize(ANIM_END);

	CStateNode::STATENODE_DESC pNodeDesc{};
	pNodeDesc.pParentModel = m_pParentModel;
	pNodeDesc.iCurrentState = 11;
    pNodeDesc.iNextStateIdx = BACK;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[GO] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 8;
    pNodeDesc.iNextStateIdx = BACK;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[BACK] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 12;
    pNodeDesc.iNextStateIdx = BACK;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[LEFT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 13;
    pNodeDesc.iNextStateIdx = BACK;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[RIGHT] = CStateNode::Create(&pNodeDesc);

    pNodeDesc.iCurrentState = 2;
    pNodeDesc.iNextStateIdx = BACK;
    pNodeDesc.bIsLoop = true;
    m_StateNodes[JETPACK] = CStateNode::Create(&pNodeDesc);
	return S_OK;
}

CStateMachine::Result CGunPawn_Move::StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim)
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

    if (m_iCurIndex != m_iNextIndex)
    {
        if (m_iNextIndex == GO)
            m_pGameInstance->Play_Sound(L"ST_Enemy_Step_Medium.ogg", &m_pChannel, 1.f, true);
        if (m_iNextIndex == BACK)
            m_pGameInstance->Play_Sound(L"ST_Enemy_Step_Medium.ogg", &m_pChannel, 1.f, true);

        m_iCurIndex = m_iNextIndex;
        m_StateNodes[m_iCurIndex]->State_Enter();
    }
    
    m_pGameInstance->SetChannelVolume( &m_pChannel, 60.f,
    m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION) - m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
 
    _bool isFall = m_pParentObject->Get_Navigation()->Get_bFalling();

    if (true == isFall)
    {
        m_iNextIndex = JETPACK;
    }
    else if (*m_fLength < 10.f)
    {
        m_iNextIndex = BACK;
    }
    else
        m_iNextIndex = GO;

    if (*m_fLength > 13.f || isFall)
    {
        if (m_pGameInstance->Get_Player()->Get_onCell())
        {
            m_pGameInstance->Add_Job(
                [&]()
                {
                    m_pParentObject->Get_Navigation()->Set_Taget(
                                m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
                });
            
            if (m_pParentObject->Get_Transform()->FollowPath(m_pParentObject->Get_Navigation(), fTimeDelta))
            {
                if (false == isFall)
                {
                    Reset_StateMachine(pRim);
                    return Result::Finished;
                }
            }
        }
        else
        {
            m_pParentObject->Get_Transform()->Go_Move(CTransform::GO, fTimeDelta, m_pParentObject->Get_Navigation());
        }
         
    }
    else if (*m_fLength <= 13.f && *m_fLength > 10.f && false == isFall)
    {
        Reset_StateMachine(pRim);
        return Result::Finished;
    }

    if (m_iCurIndex == BACK )
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

void CGunPawn_Move::Reset_StateMachine(RIM_LIGHT_DESC* pRim)
{
    m_pGameInstance->StopSound(&m_pChannel);
    __super::Reset_StateMachine(pRim);
}

CGunPawn_Move* CGunPawn_Move::Create(void* pArg)
{
    CGunPawn_Move* pInstance = new CGunPawn_Move();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CGunPawn_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunPawn_Move::Free()
{
	__super::Free();
}