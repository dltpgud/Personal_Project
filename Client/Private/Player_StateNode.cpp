#include "stdafx.h"
#include "Player_StateNode.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "Body_Player.h"

CPlayer_StateNode::CPlayer_StateNode()
	:	m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayer_StateNode::Initialize(void* pArg)
{
    PLAYER_INFO_DESC* pDesc = static_cast<PLAYER_INFO_DESC*>(pArg);
    m_pCurWeapon    = pDesc->pCurWeapon;
    m_pParentObject = pDesc->pParentObject;
     Init_CallBack_Func();
	return S_OK;
}

void CPlayer_StateNode::State_Enter(_uint* pState)
{
    for (_int i = 0; i < CPlayer::PART_END; i++)
        m_pParentObject->Set_PartObj_Set_Anim(i, m_StateDesc.iCurMotion[i], m_StateDesc.bLoop);
    
}

_bool CPlayer_StateNode::State_Processing(_float fTimedelta, _uint* pState)
{
	if (false == CheckInputCondition(*pState))
    return true; 

    m_pParentObject->Set_PartObj_Play_Anim(CPlayer::PART_WEAPON, fTimedelta * m_StateDesc.fPlayTime);
    
return m_pParentObject->Set_PartObj_Play_Anim(CPlayer::PART_BODY, fTimedelta * m_StateDesc.fPlayTime);
}

_bool CPlayer_StateNode::State_Exit(_uint* pState)
{

	 return true;
}

void CPlayer_StateNode::Init_CallBack_Func()
{
}

_bool CPlayer_StateNode::Move_KeyFlage(_uint* pState)
{
    _bool bMove{};

    if (m_pGameInstance->Get_DIKeyDown(DIK_A) || m_pGameInstance->Get_DIKeyState(DIK_A)) 
	{
	  bMove = true;
      *pState |= DIR_LEFT;
	}
    if (m_pGameInstance->Get_DIKeyDown(DIK_D) || m_pGameInstance->Get_DIKeyState(DIK_D))
	{
      bMove = true;
      *pState |= DIR_RIGHT;
	}
	if (m_pGameInstance->Get_DIKeyDown(DIK_W) || m_pGameInstance->Get_DIKeyState(DIK_W))
	{
      bMove = true;
      *pState |= DIR_FORWARD;
	}
	if (m_pGameInstance->Get_DIKeyDown(DIK_S) || m_pGameInstance->Get_DIKeyState(DIK_S))
	{
      bMove = true;
      *pState |= DIR_BACK;
	}

	if (false == bMove)
	*pState &= ~(DIR_FORWARD | DIR_BACK | DIR_LEFT | DIR_RIGHT);

    return bMove;
}

CPlayer_StateNode* CPlayer_StateNode::Create(void* pArg)
{
	CPlayer_StateNode* pInstance = new CPlayer_StateNode();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_StateNode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_StateNode::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
