#include"stdafx.h"
#include "StateNode.h"
#include "GameInstance.h"
#include "Model.h"

CStateNode::CStateNode() : m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CStateNode::Initialize(void* pArg)
{
    STATENODE_DESC* pDesc = static_cast<STATENODE_DESC*>(pArg);

    m_StateDesc.pParentModel  = pDesc -> pParentModel;
    m_StateDesc.iCurrentState = pDesc ->iCurrentState;
    m_StateDesc.iNextStateIdx = pDesc ->iNextStateIdx;
    m_StateDesc.bCondition    = pDesc -> bCondition;
    m_StateDesc.bIsLoop       = pDesc -> bIsLoop;
    m_StateDesc.fPlayAniTime  = pDesc -> fPlayAniTime;
    return S_OK;
}

void CStateNode::State_Enter()
{
    m_StateDesc.pParentModel->Set_Animation(m_StateDesc.iCurrentState, m_StateDesc.bIsLoop);
}

_bool CStateNode::State_Processing(_float fTimeDelta)
{
    m_bFinished = m_StateDesc.pParentModel->Play_Animation(fTimeDelta * m_StateDesc.fPlayAniTime);
    return m_bFinished;
}

_bool CStateNode::State_Exit(_int* NextIndex)
{
    *NextIndex = m_StateDesc.iNextStateIdx;

    if (*NextIndex == -1)
        return false;

    if (!m_StateDesc.bCondition)
        return true;
   
    return *m_StateDesc.bCondition;
}

CStateNode* CStateNode::Create(void* pDesc)
{
    CStateNode* pInstance = new CStateNode();

    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX("Failed to Created : CStateNode");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStateNode::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
