#include "stdafx.h"
#include "Player_StateMachine.h"
#include "Player_Swich.h"
#include "Weapon.h"
#include "Body_Player.h"
CPlayer_Swich::CPlayer_Swich()
{
}

HRESULT CPlayer_Swich::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Swich::State_Enter(_uint* pState)
{
    // hit 플래그가 켜져 있으면 끄기
    *pState &= ~MOV_HIT;
    
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_SWICH;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;

    __super::State_Enter(pState);
}

_bool CPlayer_Swich::State_Processing(_float fTimedelta, _uint* pState)
{
    
    return __super::State_Processing(fTimedelta,pState);
}

_bool CPlayer_Swich::State_Exit(_uint* pState)
{
    SetActive(false, pState);
   // *Next = CPlayer_StateMachine::NODE_IDLE;
    m_pParentObject->Set_Change();
   return true;
}

void CPlayer_Swich::Init_CallBack_Func()
{
    for (_int i = 0; i < BODY_TYPE::T_END; i++)
    {
        m_pParentObject->BodyCallBack(i, CBody_Player::BODY_SWICH, 0,[this]() { m_pGameInstance->Play_Sound(L"ST_Player_Switch_T01.ogg", CSound::SOUND_EFFECT, 1.f); });
    }
}

_bool CPlayer_Swich::IsActive(_uint stateFlags) const
{
    return (stateFlags & BEH_SWICH) != 0;
}

void CPlayer_Swich::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= BEH_SWICH;
    else
        *pState &= ~BEH_SWICH;
}

_bool CPlayer_Swich::CanEnter(_uint* pState)
{
    // 점프 상태가 활성화되면 Switch 상태로 진입하지 않음
    if ((*pState & MOV_JUMP) != 0)
        return false;
        
    if (!m_pParentObject->Get_Change())
        return false;

    // 이미 무기 교체 중이면 중복 실행 방지
    if ((*pState & BEH_SWICH) != 0)
        return false;

    return true;
}

_bool CPlayer_Swich::CheckInputCondition(_uint stateFlags)
{
    return true;
}



CPlayer_Swich* CPlayer_Swich::Create(void* pArg)
{
    CPlayer_Swich* pInstance = new CPlayer_Swich();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Swich");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Swich::Free()
{
	__super::Free();
}
