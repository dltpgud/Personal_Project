#include "stdafx.h"
#include "Player_StateMachine.h"
#include "Player_Health.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "Player_HpUI.h"
CPlayer_Health::CPlayer_Health()
{
}

HRESULT CPlayer_Health::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Health::State_Enter(_uint* pState, _uint* pPreState)
{
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_GENERIC_STAP;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = false;

    m_pGameInstance->Set_OpenUI(true, TEXT("PlayerState"));
    __super::State_Enter(pState, pPreState);
}

_bool CPlayer_Health::State_Processing(_float fTimedelta, _uint* pState, _uint* pPreState)
{
    _bool bBodyAnimFinished = m_pParentObject->Set_PartObj_Play_Anim(CPlayer::PART_BODY, fTimedelta * m_StateDesc.fPlayTime);
    _bool bWeaponAnimFinished = m_pParentObject->Set_PartObj_Play_Anim(CPlayer::PART_WEAPON, fTimedelta * m_StateDesc.fPlayTime);

    return bBodyAnimFinished && bWeaponAnimFinished;
}

_bool CPlayer_Health::State_Exit(_uint* pState)
{
    return true;
}

void CPlayer_Health::Init_CallBack_Func()
{
   for(_int i = 0; i <BODY_TYPE::T_END; i++){ m_pParentObject->BodyCallBack(i, CBody_Player::BODY_GENERIC_STAP,
        0, [this]() { m_pGameInstance->Play_Sound(L"ST_Player_Flash_Stop.ogg",  &m_pChannel, 1.f); });
   }

   for(_int i = 0; i <BODY_TYPE::T_END; i++){ m_pParentObject->BodyCallBack(i, CBody_Player::BODY_GENERIC_STAP, 20,
         [this]() { static_cast<CPlayer_HpUI*>(m_pGameInstance->Find_Clone_UIObj(L"PlayerHP"))->Set_HPGage(100); });
   }     
}

_bool CPlayer_Health::IsActive(_uint stateFlags) const
{
    return (stateFlags & CPlayer::MOV_HEALTH) != 0;
}

void CPlayer_Health::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= CPlayer::MOV_HEALTH; 
    else
        *pState &= ~CPlayer::MOV_HEALTH;
}

_bool CPlayer_Health::CanEnter(_uint* pState)
{
    return (*pState & CPlayer::MOV_HEALTH) && !(*pState & CPlayer::MOV_STURN);
}

_bool CPlayer_Health::CheckInputCondition(_uint stateFlags)
{
    return true;
}

CPlayer_Health* CPlayer_Health::Create(void* pArg)
{
    CPlayer_Health* pInstance = new CPlayer_Health();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Health");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Health::Free()
{
	__super::Free();
}
