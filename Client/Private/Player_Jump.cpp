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

void CPlayer_Jump::State_Enter(_uint* pState)
{
   // *pState &= ~MOV_HIT;
    
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_JUMP_RUN_LOOP; 
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;

    m_bJump = true;

    m_pParentObject->Set_onCell(false);  

    __super::State_Enter(pState);
}

_bool CPlayer_Jump::State_Processing(_float fTimeDelta, _uint* pState)
{
    Move_KeyFlage(pState);

    // 더블 점프 입력 처리 
    if (!m_bDoubleJump  && m_pGameInstance->Get_DIKeyDown(DIK_SPACE))
    {
        m_bDoubleJump = true;
       
        if (!m_bDoubleJumpSound)
        {
            m_pParentObject->Get_Transform()->Reset_DoubleJumpTime();
            m_pGameInstance->Play_Sound(L"ST_Player_DoubleJump.ogg", CSound::SOUND_BGM, 0.5f);
            m_bDoubleJumpSound = true;
        }
    }

    _int isFall{10};
    m_pParentObject->Get_Transform()->Go_jump(fTimeDelta * m_fJumpSpeedMultiplier, m_pParentObject->Get_fY(), &m_bJump,&isFall, m_pParentObject->Get_Navi());

    if (isFall < 0) {
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
    
    if (__super::State_Processing(fTimeDelta, pState))
    {
        return true;
    }
   
    return false;
}

_bool CPlayer_Jump::State_Exit(_uint* pState)
{
    m_bJump = false;
    m_pParentObject->Set_onCell(true);

  //  *pState &= ~BEH_SHOOT;

    m_bDoubleJump = false;
    m_bDoubleJumpSound = false;
    m_JumpFall = false;
    
    return true;
}

void CPlayer_Jump::Init_CallBack_Func()
{
    for (_int i = 0; i < BODY_TYPE::T_END; i++)
    {
        m_pParentObject->BodyCallBack(i, CBody_Player::BODY_JUMP_RUN_LOOP, 1, [this]()
                                      { m_pGameInstance->Play_Sound(L"ST_Player_Jump.ogg", CSound::SOUND_BGM, 1.f); });
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

    if (m_pGameInstance->Get_DIKeyDown(DIK_SPACE))
    {
        m_pParentObject->Get_Transform()->StartJump();
     
        *pState = MOV_JUMP; 
        return true;
    }
    
    return false;
}

_bool CPlayer_Jump::CheckInputCondition(_uint stateFlags)
{
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
