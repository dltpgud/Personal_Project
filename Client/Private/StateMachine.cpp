#include "stdafx.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "GameObject.h"
CStateMachine::CStateMachine() : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CStateMachine::Initialize(void* pArg)
{
    auto pDesc = static_cast<STATEMACHINE_DESC*>(pArg);
    m_pParentModel      = pDesc->pParentModel;
    m_iBaseIndex        = pDesc->iBaseIndex;
    m_iNextMachineIdx   = pDesc->iNextMachineIdx;
    m_pParentPartObject = pDesc->pParentPartObject;
    m_pParentObject     = pDesc->pParentObject;

    m_iNextIndex = m_iBaseIndex;
    SetFlag(ACTIVE);
    Init_CallBack_Func();
    return S_OK;
}

CStateMachine::Result CStateMachine::StateMachine_Playing(_float fTimeDelta)
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

    // 상태가 변경되면 Enter 실행
    if (m_iCurIndex != m_iNextIndex)
    {
        m_iCurIndex = m_iNextIndex;
        m_StateNodes[m_iCurIndex]->State_Enter();
    }

    // 상태 진행
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

void CStateMachine::Reset_StateMachine()
{
    m_iCurIndex = -1;
    m_iNextIndex = m_iBaseIndex;

    ClearFlag(PAUSED);
    ClearFlag(FINISHED);
    SetFlag(ACTIVE);
}

CStateMachine* CStateMachine::Create(void* pArg)
{
    CStateMachine* pInstance = new CStateMachine();

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CStateMachine");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStateMachine::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

    for (auto& Node : m_StateNodes) Safe_Release(Node);

    m_StateNodes.clear();
    m_StateNodes.shrink_to_fit();
}
