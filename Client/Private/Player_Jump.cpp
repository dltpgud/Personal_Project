#include "stdafx.h"
#include "Player_Jump.h"
#include "Weapon.h"
#include "Body_Player.h"
CPlayer_Jump::CPlayer_Jump()
{
}

HRESULT CPlayer_Jump::Initialize(void* pDesc)
{
	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Jump::State_Enter(_uint* pState, _uint* pPreState)
{
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_JUMP_RUN_LOOP; 
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;

    m_bJump = true;

    m_pParentObject->Set_onCell(false);  

    __super::State_Enter(pState, pPreState);
}

_bool CPlayer_Jump::State_Processing(_float fTimeDelta, _uint* pState, _uint* pPreState)
{
    if ((*pState & MOV_SPRINT)!=0)
        m_pGameInstance->Set_OpenUI(true, TEXT("PlayerState"));

    Move_KeyFlage(pState);

    if (!m_bDoubleJump && m_pGameInstance->Get_DIKeyDown(DIK_SPACE) &&
        false == m_pParentObject->GetFlag(CPlayer::FLAG_KEYLOCK))
    {
        m_bDoubleJump = true;
       
        if (!m_bDoubleJumpSound)
        {
            m_pParentObject->Get_Transform()->Reset_DoubleJumpTime();
            m_pGameInstance->Play_Sound(L"ST_Player_DoubleJump.ogg", &m_pChannel, 0.5f);
            m_bDoubleJumpSound = true;
        }
    }

    _float isFall{};
    m_pParentObject->Get_Transform()->Go_jump(fTimeDelta * m_fJumpSpeedMultiplier, m_pParentObject->Get_fY(), &m_bJump,&isFall, m_pParentObject->Get_Navigation());

    if (isFall < 0.f) {
        if (false == m_JumpFall) 
        {
            m_pParentObject->Set_PartObj_Set_Anim(CPlayer::PART_BODY, CBody_Player::BODY_JUMP_FALL, true);
            m_JumpFall = true;
        }
    }

    if (!m_bJump)
    {
        return true;
    }
    

    if (__super::State_Processing(fTimeDelta, pState, pPreState))
    {
        return true;
    }
   
    return false;
}

_bool CPlayer_Jump::State_Exit(_uint* pState)
{
    m_bJump = false;
    m_bDoubleJump = false;
    m_bDoubleJumpSound = false;
    m_JumpFall = false;

    // 점프가 완전히 끝났을 때만 셀을 설정
    // 총쏘기 상태일 때는 점프 상태를 유지하고, 점프가 끝났을 때만 셀을 찾음
    m_pParentObject->Set_onCell(true);
    m_pParentObject->Get_Navigation()->Find_CurrentCell(
        m_pParentObject->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));

    // 점프가 끝났으므로 false를 반환하여 IDLE 상태로 전환
    return false;
}

void CPlayer_Jump::Init_CallBack_Func()
{
    for (_int i = 0; i < BODY_TYPE::T_END; i++)
    {
        m_pParentObject->BodyCallBack(i, CBody_Player::BODY_JUMP_RUN_LOOP, 1, [this]()
                                      { m_pGameInstance->Play_Sound(L"ST_Player_Jump.ogg", &m_pChannel, 1.f); });
    }
}

_bool CPlayer_Jump::IsActive(_uint stateFlags) const
{
    return (stateFlags & MOV_JUMP) != 0;
}

void CPlayer_Jump::SetActive(_bool active, _uint* pState)
{
    if (active)
        *pState |= MOV_JUMP;
    else
        *pState &= ~MOV_JUMP;
}

_bool CPlayer_Jump::CanEnter(_uint* pState)
{
    if (*pState & MOV_STURN)
        return false;

    if (m_pGameInstance->Get_DIKeyDown(DIK_SPACE) && false ==  m_pParentObject->GetFlag(CPlayer::FLAG_KEYLOCK))
    {
        m_pParentObject->Get_Transform()->StartJump();
        return true;
    }
    
    return false;
}

_bool CPlayer_Jump::CheckInputCondition(_uint stateFlags)
{
    // 점프 상태에서는 총쏘기와 리로드가 가능하도록 수정
    // if (stateFlags & (BEH_SHOOT | BEH_RELOAD))
    // {
    //     return false;
    // }
    return true;
}

CPlayer_Jump* CPlayer_Jump::Create(void* pArg)
{
	CPlayer_Jump* pInstance = new CPlayer_Jump();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer_Jump");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Jump::Free()
{
	__super::Free();
}
