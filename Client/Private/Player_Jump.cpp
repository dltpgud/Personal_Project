 #include "stdafx.h"
#include "Player_Jump.h"
#include "Weapon.h"
#include "Body_Player.h"

CPlayer_Jump::CPlayer_Jump()
    : m_fJumpTime(0.f)
    , m_bJumpSound(false)
    , m_bJump(false)
    , m_fJumpSpeedMultiplier(4.5f) // 점프 속도 배율 설정 (1.0f = 기본 속도, 2.5f = 2.5배 빠름)
    , m_bDoubleJump(false)
    , m_bDoubleJumpSound(false)
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
    // hit 플래그가 켜져 있으면 끄기
    *pState &= ~MOV_HIT;
    
    m_StateDesc.iCurMotion[CPlayer::PART_BODY] = CBody_Player::BODY_JUMP_RUN_LOOP; // 일반 점프 애니메이션
    m_StateDesc.iCurMotion[CPlayer::PART_WEAPON] = CWeapon::WS_IDLE;
    m_StateDesc.bLoop = true;
    m_StateDesc.fPlayTime = 1.f;

    // 점프 시작
    m_fJumpTime = 0.f;
    m_bJumpSound = false;
    m_bJump = true;

    // 플레이어 점프 상태 활성화
    m_pParentObject->Set_bJump(true);
    m_pParentObject->Set_onCell(false);

    // 점프 사운드 재생
    m_pGameInstance->Play_Sound(L"ST_Player_Jump.ogg", CSound::SOUND_BGM, 1.f);
        
    __super::State_Enter(pState);
}

_bool CPlayer_Jump::State_Processing(_float fTimeDelta, _uint* pState)
{
    m_fJumpTime += fTimeDelta;
    
    Move_KeyFlage(pState);

    // 더블 점프 입력 처리 (점프 중에 스페이스바를 누르면 더블 점프)
    if (!m_bDoubleJump  && m_pGameInstance->Get_DIKeyDown(DIK_SPACE))
    {
        // 더블 점프 시작
        m_bDoubleJump = true;
       
        // 더블 점프 사운드 재생
        if (!m_bDoubleJumpSound)
        {
            m_pParentObject->Get_Transform()->Reset_DoubleJumpTime();
            m_pGameInstance->Play_Sound(L"ST_Player_DoubleJump.ogg", CSound::SOUND_BGM, 0.5f);
            m_bDoubleJumpSound = true;
        }
    }
    
    // 점프 중 무기 애니메이션을 IDLE로 강제 설정
    m_pParentObject->Set_PartObj_Set_Anim(CPlayer::PART_WEAPON, CWeapon::WS_IDLE, true);
    
    _int isFall{10};
    m_pParentObject->Get_Transform()->Go_jump(fTimeDelta * m_fJumpSpeedMultiplier, m_pParentObject->Get_fY(), &m_bJump,&isFall, m_pParentObject->Get_Navi());
    cout << isFall << endl;
    if (isFall < 0) {
        if (false == m_JumpFall) 
        {
            m_pParentObject->Set_PartObj_Set_Anim(CPlayer::PART_BODY, CBody_Player::BODY_JUMP_FALL, true);
            m_JumpFall = true;
        }
    }

    // 점프가 끝났는지 확인
    if (!m_bJump)
    {
        m_bDoubleJump = false;
        m_bDoubleJumpSound = false;
        m_JumpFall = false;
       
        return true;
    }
    
    // 점프 애니메이션 재생
    if (__super::State_Processing(fTimeDelta, pState))
    {
        // 애니메이션이 끝나면 점프 종료
        return true;
    }
    
    // 점프 시간이 일정 시간을 넘으면 점프 종료
    if (m_fJumpTime > 1.5f) // 점프 시간을 1.5초로 제한
    {
        return true;
    }
    
    return false;
}

_bool CPlayer_Jump::State_Exit(_uint* pState)
{
    SetActive(false, pState);
    
    // 점프 상태 종료 시 플레이어 상태 초기화
    m_bJump = false;
    m_pParentObject->Set_bJump(false);
    m_pParentObject->Set_onCell(true);
    
    // 더블 점프 관련 변수 초기화
    m_bDoubleJump = false;
    m_bDoubleJumpSound = false;
    m_JumpFall = false;
    
    return true;
}

void CPlayer_Jump::Init_CallBack_Func()
{
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
    // 스페이스바를 누르면 점프 상태로 진입
    if (m_pGameInstance->Get_DIKeyDown(DIK_SPACE))
    {
        m_pParentObject->Get_Transform()->StartJump();
        // 이미 점프 중이면 더블 점프
        if ((*pState & MOV_JUMP) != 0)
        {
       
            return true; // 더블 점프 허용
        }
        else
        {
            // 첫 번째 점프 - 모든 상태 플래그를 끄고 점프 상태만 키기
            *pState = MOV_JUMP; // 모든 플래그를 초기화하고 점프만 설정
            return true;
        }
    }
    
    return false;
}

_bool CPlayer_Jump::CheckInputCondition(_uint stateFlags)
{
    // 점프 중에는 다른 입력 조건 체크하지 않음 (hit 상태가 활성화되어도 점프 상태 유지)
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
