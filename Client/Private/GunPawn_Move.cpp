#include"stdafx.h"
#include "GunPawn_Move.h"
#include "GunPawn.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Body_GunPawn.h"
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

CStateMachine::Result CGunPawn_Move::StateMachine_Playing(_float fTimeDelta)
{
    if (HasFlag(PAUSED))
        return Result::None;

    if (m_StateNodes.empty())
        return Result::None;

    // nullptr 체크 추가
    if (m_pParentObject == nullptr || m_fLength == nullptr)
    {
        Reset_StateMachine();
        return Result::Finished;
    }

    if (m_iNextIndex < 0 || m_iNextIndex >= static_cast<_int>(m_StateNodes.size()))
    {
        Reset_StateMachine();
        return Result::Finished;
    }

    if (*m_fLength < 15.f) {
    
    m_iNextIndex = BACK;
    }

    if (m_iCurIndex != m_iNextIndex)
    {
        if (m_iNextIndex == GO)
            m_pGameInstance->Play_Sound(L"ST_Enemy_Move_Roll2.ogg", CSound::SOUND_MONSTER_ROLL2, 0.2f);
        if (m_iNextIndex == BACK)
            m_pGameInstance->Play_Sound(L"ST_Enemy_Move_Roll.ogg", CSound::SOUND_MONSTER_ROLL2, 0.2f);

        m_iCurIndex = m_iNextIndex;
        m_StateNodes[m_iCurIndex]->State_Enter();
    }
    
    // 추가 nullptr 체크
    CGunPawn* pGunPawn = dynamic_cast<CGunPawn*>(m_pParentObject);
    if (pGunPawn == nullptr || pGunPawn->Get_Navi() == nullptr)
    {
        Reset_StateMachine();
        return Result::Finished;
    }
    
    _bool isFall = pGunPawn->Get_Navi()->ISFall();

    if (true == isFall)
    {
        Reset_StateMachine();
        return Result::Finished;
    }
    if (*m_fLength > 20.f && false == isFall)
    {
        m_iCurIndex = GO;
        m_pGameInstance->Add_Job(
            [this]()
            {
                // Job 내부에서도 nullptr 체크
                if (m_pParentObject != nullptr)
                {
                    CGunPawn* pGunPawn = dynamic_cast<CGunPawn*>(m_pParentObject);
                    if (pGunPawn != nullptr && m_pGameInstance->Get_Player() != nullptr)
                    {
                        pGunPawn->Set_Taget(
                            m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
                    }
                }
            });
        if (m_pParentObject->Get_Transform()->FollowPath(pGunPawn->Get_Navi(), fTimeDelta))
        {
             Reset_StateMachine();
            return Result::Finished;
        }
    }
    else if (*m_fLength <= 20.f && *m_fLength >= 15.f)
    {
        Reset_StateMachine();
        return Result::Finished;
    }

    if (m_iCurIndex == BACK && false == isFall)
    {

       m_pParentObject->Get_Transform()->Go_Move(CTransform::BACK, fTimeDelta, pGunPawn->Get_Navi());
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
            Reset_StateMachine();
            SetFlag(FINISHED);
            return Result::Finished;
        }
  }
    return Result::Running;
}

void CGunPawn_Move::Reset_StateMachine()
{
    m_pGameInstance->StopSound(CSound::SOUND_MONSTER_ROLL2);
   __super::Reset_StateMachine();
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