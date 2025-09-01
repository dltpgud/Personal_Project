#include "stdafx.h"
#include "Player_StateMachine.h"
#include "Player_Fall.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "Player_ShootingStateUI.h"
CPlayer_Fall::CPlayer_Fall()
{
}

HRESULT CPlayer_Fall::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Fall::State_Enter(_uint* pState, _uint* pPreState)
{
    if ((*pPreState & CPlayer_StateNode::MOV_HIT) != 0)
        return;

    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_SPTINT_FALL;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;
    m_pParentObject->Set_onCell(false);
    m_bSetBody = false;
    m_pParentObject->AddFlag(CPlayer::FLAG_KEYLOCK);
    __super::State_Enter(pState, pPreState);
}

_bool CPlayer_Fall::State_Processing(_float fTimedelta, _uint* pState, _uint* pPreState)
{
    _bool bDead = m_pParentObject->GetTriggerFlag(CActor::Trigger_Terrain);
    if (false == bDead)
    {
        __super::State_Processing(fTimedelta, pState, pPreState);
        m_pParentObject->Get_Transform()->Go_Move(CTransform::DOWN, fTimedelta, nullptr, m_bFromJump);
    }

    if (true == bDead )
    {
        if (m_bSetBody == false)
        {
            m_pParentObject->Set_PartObj_Set_Anim(CPlayer::PART_BODY, CBody_Player::BODY_DEATH, false);
            m_bSetBody = true;
        }
        if(__super::State_Processing(fTimedelta, pState, pPreState))
            return  true;
    }

    return false;
}

_bool CPlayer_Fall::State_Exit(_uint* pState)
{
    m_bSetBody = false;
    m_bFromJump = false;
    m_pParentObject->SetTriggerFlag(CActor::Trigger_Terrain, false);
    m_pParentObject->SetFlag(CPlayer::FLAG_KEYLOCK,false);
    m_pParentObject->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, m_pParentObject->Get_Navigation()->Get_SafePos());
    m_pParentObject->Set_onCell(true);
    m_pParentObject->Get_Navigation()->Find_CurrentCell(m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));

    return false;
}

void CPlayer_Fall::Init_CallBack_Func()
{
    for (_int i = 0; i < BODY_TYPE::T_END; i++)
    {
        m_pParentObject->BodyCallBack(i, CBody_Player::BODY_DEATH, 0,
            [this]()
            {
                m_pGameInstance->Play_Sound(L"ST_Player_Burn_Stop.ogg", &m_pChannel, 1.f);
                m_pGameInstance->Set_OpenUI(true, TEXT("PlayerState"));
            });
    }
}

_bool CPlayer_Fall::IsActive(_uint stateFlags) const
{
    return (stateFlags & MOV_FALL) != 0;
}

void CPlayer_Fall::SetActive(_bool active, _uint* pState)
{
    if (active)
    {
        *pState |= MOV_FALL;
    }
    else
    {
        *pState &= ~MOV_FALL;
    }
}

_bool CPlayer_Fall::CanEnter(_uint* pState)
{    
    _bool bFall = m_pParentObject->Get_Navigation()->Get_bFalling();

    if (true == bFall)
    {
        if ((*pState & MOV_JUMP) != 0)
        {
            m_bFromJump = true;
            return false;
        }
        return true;
    }

    m_bFromJump = false;
    return false;
}

_bool CPlayer_Fall::CheckInputCondition(_uint stateFlags)
{
    return true;
}

CPlayer_Fall* CPlayer_Fall::Create(void* pArg)
{
    CPlayer_Fall* pInstance = new CPlayer_Fall();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Fall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Fall::Free()
{
	__super::Free();
}
