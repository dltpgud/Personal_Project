#include "stdafx.h"
#include "Player_StateMachine.h"
#include "Player_Fall.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "ShootingUI.h"
CPlayer_Fall::CPlayer_Fall()
{
}

HRESULT CPlayer_Fall::Initialize(void* pDesc)
{
	if(FAILED (__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Fall::State_Enter(_uint* pState)
{
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_SPTINT_FALL;
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;

    static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingReload"))->Set_Open(false);
    static_cast<CShootingUI*>(m_pGameInstance->Find_Clone_UIObj(L"ShootingShoot"))->Set_Open(false);

    m_pParentObject->Set_onCell(false);
    m_pParentObject->Get_Transform()->Set_MoveSpeed(8.f);

    m_FallTimedelta = 0.f;
    m_FallTime = 2.f;

    m_bSetBody = false;

    __super::State_Enter(pState);
}

_bool CPlayer_Fall::State_Processing(_float fTimedelta, _uint* pState)
{
    __super::State_Processing(fTimedelta, pState);

    _vector vPos = m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

      m_FallTimedelta += fTimedelta;

    if (XMVectorGetY(vPos) > -7)
    {
        m_pParentObject->Get_Transform()->Go_Move(CTransform::DOWN, fTimedelta);
        
    }

    if (m_FallTimedelta > m_FallTime / 3.5f)
    {
        if (m_bSetBody == false)
            m_pParentObject->Set_PartObj_Set_Anim(CPlayer::PART_BODY, CBody_Player::BODY_DEATH, false);
        m_bSetBody = true;

        __super::State_Processing(fTimedelta, pState);
    }

    if (m_FallTimedelta > m_FallTime)
    {
        return true;
    }

    return false;
}

_bool CPlayer_Fall::State_Exit(_uint* pState)
{
    m_pParentObject->Get_Transform()->Set_MoveSpeed(4.f);
    m_pParentObject->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, m_pParentObject->Get_Navi()->Get_SafePos());
    m_pParentObject->Set_onCell(true);
    m_pParentObject->Get_Navi()->Find_CurrentCell(m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
    return true;
}

void CPlayer_Fall::Init_CallBack_Func()
{

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
    if ((*pState & MOV_JUMP) != 0)
        return false;

    _bool bFall = m_pParentObject->Get_Navi()->ISFall();

    if (true == bFall)
    {
        return true;
    }
    else
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
