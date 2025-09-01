#include "stdafx.h"
#include "GameInstance.h"
#include "Player_StateUI.h"
#include "Player_StateMachine.h"
#include "Player_Idle.h"
#include "Player_Swich.h"
#include "Player_Reload.h"
#include "Player_Shoot.h"
#include "Player_Run.h"
#include "Player_Sprint.h"
#include "Player_Jump.h"
#include "Player_Hit.h"
#include "Player_Fall.h"
#include "Player_Sturn.h"
#include "Player_Health.h"
#include "Player_ShootingStateUI.h"
CPlayer_StateMachine::CPlayer_StateMachine() : m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_StateMachine::Initialize(void* pArg)
{
    PLAYER_STATEMACHINE_DESC* pDesc = static_cast<PLAYER_STATEMACHINE_DESC*>(pArg);
    m_pParentObject = pDesc->pParentObject;
    

    if (FAILED(Initialize_StateUI(pDesc->pCurWeapon)))
        return E_FAIL;

	m_StateNodes.resize(NODE_END);

    m_StateNodes[NODE_HIT]    = CPlayer_Hit::Create(pDesc);
    m_StateNodes[NODE_STUN]   = CPlayer_Sturn::Create(pDesc);
    m_StateNodes[NODE_FALL]   = CPlayer_Fall::Create(pDesc);
    m_StateNodes[NODE_SHOOT]  = CPlayer_Shoot::Create(pDesc);      
    m_StateNodes[NODE_RELOAD] = CPlayer_Reload::Create(pDesc);
    m_StateNodes[NODE_SWICH]  = CPlayer_Swich::Create(pDesc);
    m_StateNodes[NODE_JUMP]   = CPlayer_Jump::Create(pDesc);
    m_StateNodes[NODE_SPRINT] = CPlayer_Sprint::Create(pDesc);
    m_StateNodes[NODE_RUN]    = CPlayer_Run::Create(pDesc);
    m_StateNodes[NODE_IDLE]   = CPlayer_Idle::Create(pDesc);
    m_StateNodes[NODE_HEALTH] = CPlayer_Health::Create(pDesc);

    m_iState = CPlayer_StateNode::MOV_IDLE;

	return S_OK;
}

void CPlayer_StateMachine::ResetMachine()
{
    m_iPreviousState = 0;
    m_iState = CPlayer_StateNode::MOV_IDLE;
    m_iLastActiveState = 0;
    m_pGameInstance->Set_OpenUI(false, TEXT("Player_ShootingUI"));
    m_pGameInstance->Set_OpenUI(false, TEXT("Player_AutoShootingUI"));
    m_pGameInstance->Set_OpenUI(false, TEXT("PlayerState"));
}

void CPlayer_StateMachine::StateMachine_Playing(_float fTimeDelta)
{
    if (m_StateNodes.empty())
        return;

    for (auto& node : m_StateNodes)
    {
        if (nullptr == node)
            continue;
       
        _bool bactive = node->IsActive(m_iState); // 활성여부

        progress_Move(fTimeDelta, &m_iState); // DIR 플래그로 이동

        if (!bactive) // 활성 안되어 있으니 검사
        {
            if (node->CanEnter(&m_iState)) //상태를 활성화할 수 있는가?
            {
                Check_UIState(&m_iPreviousState);  // 이전 상태에 따른 UI 끄기 
                node->State_Enter(&m_iState, &m_iPreviousState);  // 상태 진입
                m_iPreviousState = m_iState;  
                node->SetActive(true, &m_iState); //플래그 켜기
                m_iLastActiveState = m_iState; // 현재 활성화된 상태 저장
            }
        }
        else
        {
            if (node->State_Processing(fTimeDelta, &m_iState, &m_iPreviousState)) // 상태 진행
            {
                _bool bReturnPrev = node->State_Exit(&m_iState); // 상태 탈출
                m_iPreviousState = m_iState;  
                node->SetActive(false, &m_iState); // 상태 크기

                if (!bReturnPrev)
                {
                    Check_UIState(&m_iPreviousState);  
                    m_iState = CPlayer_StateNode::MOV_IDLE;
                    m_StateNodes[NODE_IDLE]->State_Enter(&m_iState, &m_iPreviousState); 
                }
            }
            else
            {
                // 상태가 활성화되어 있지만 이전 상태와 다른 경우 State_Enter 재호출
                if (m_iLastActiveState != m_iState && node->CanEnter(&m_iState))
                {
                    Check_UIState(&m_iPreviousState); 
                    node->State_Enter(&m_iState, &m_iPreviousState);
                    m_iPreviousState = m_iState;  
                    node->SetActive(true, &m_iState); 
                    m_iLastActiveState = m_iState;
                }
            }
        }
    }

    return;
}

void CPlayer_StateMachine::progress_Move(_float fTimeDelta, _uint* pState) 
{   
    if (m_pParentObject->GetFlag(CPlayer::FLAG_KEYLOCK) == true)
    {
        *pState &= ~(CPlayer_StateNode::DIR_FORWARD | CPlayer_StateNode::DIR_BACK | CPlayer_StateNode::DIR_LEFT |
                     CPlayer_StateNode::DIR_RIGHT);
        return ;
    }

    if (*pState & CPlayer_StateNode::DIR_LEFT)
        m_pParentObject->Get_Transform()->Go_Move(CTransform::LEFT, fTimeDelta, m_pParentObject->Get_Navigation());
    
    if (*pState & CPlayer_StateNode::DIR_RIGHT)
        m_pParentObject->Get_Transform()->Go_Move(CTransform::RIGHT, fTimeDelta, m_pParentObject->Get_Navigation());
    
    if (*pState & CPlayer_StateNode::DIR_FORWARD)
        m_pParentObject->Get_Transform()->Go_Move(CTransform::GO, fTimeDelta, m_pParentObject->Get_Navigation());
    
    if (*pState & CPlayer_StateNode::DIR_BACK)
        m_pParentObject->Get_Transform()->Go_Move(CTransform::BACK, fTimeDelta, m_pParentObject->Get_Navigation());


}

void CPlayer_StateMachine::Check_UIState(_uint* pPreState) const
{
    if (((*pPreState & CPlayer_StateNode::MOV_HIT) == 0) || ((*pPreState & CPlayer_StateNode::MOV_SPRINT) == 0) ||
        ((*pPreState & CPlayer_StateNode::MOV_HEALTH) == 0) || ((*pPreState & CPlayer_StateNode::MOV_FALL) == 0)||
        (*pPreState & CPlayer_StateNode::MOV_STURN) == 0 || (*pPreState & CPlayer_StateNode::MOV_JUMP) == 0)
    {
        m_pGameInstance->Set_OpenUI(false, TEXT("PlayerState"));
    }

    if (((*pPreState & CPlayer_StateNode::BEH_RELOAD) == 0) || ((*pPreState & CPlayer_StateNode::BEH_SHOOT) == 0))
    {
        m_pGameInstance->Set_OpenUI(false, TEXT("Player_ShootingUI"));
        m_pGameInstance->Set_OpenUI(false, TEXT("Player_AutoShootingUI"));
    }
}

HRESULT CPlayer_StateMachine::Initialize_StateUI(_uint* CurWeapon)
{
    CPlayer_StateUI::CPlayerEffectUI_DESC Desc{};
    Desc.State = &m_iState;
    Desc.iDeleteLevel = LEVEL_STATIC;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"PlayerState", L"Prototype GameObject_PlayerEffectUI", &Desc)))
        return E_FAIL;

    CPlayer_ShootingStateUI::CShootingUI_DESC SDesc{};
    SDesc.iWeaPonTYPE = CurWeapon;
    SDesc.iWeaPonState = &m_iState;
    SDesc.fX = g_iWinSizeX * 0.7f;
    SDesc.fY = g_iWinSizeY * 0.57f;
    SDesc.fZ = 0.001f;
    SDesc.fSizeX = 100.f;
    SDesc.fSizeY = 100.f;
    SDesc.UID = UIID_PlayerShooting;
    SDesc.Update = false;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"Player_ShootingUI", L"Prototype GameObject_ShootingUI", &SDesc)))
         return E_FAIL;

    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"Player_AutoShootingUI", L"Prototype GameObject_ShootingUI", &SDesc)))
        return E_FAIL;
    return S_OK;
}

CPlayer_StateMachine* CPlayer_StateMachine::Create(void* pArg)
{
	CPlayer_StateMachine* pInstance = new CPlayer_StateMachine();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_StateMachine::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

    for (auto& Node : m_StateNodes)
            Safe_Release(Node);
       
    m_StateNodes.clear();
    m_StateNodes.shrink_to_fit();
}
