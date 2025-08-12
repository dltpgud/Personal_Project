#pragma once

#include "Player_StateNode.h"

BEGIN(Client)

class CPlayer_Jump : public CPlayer_StateNode
{
private:
    CPlayer_Jump();
    virtual ~CPlayer_Jump() = default;

public:
    virtual HRESULT Initialize(void* pDesc) override;
    virtual void State_Enter(_uint* pState) override;
    virtual _bool State_Processing(_float fTimeDelta, _uint* pState) override;
    virtual _bool State_Exit(_uint* pState) override;
    virtual void Init_CallBack_Func() override;
    virtual _bool IsActive(_uint stateFlags) const override;
    virtual void SetActive(_bool active, _uint* pState) override;
    virtual _bool CanEnter(_uint* pState) override;
    virtual _bool CheckInputCondition(_uint stateFlags) override;

private:
    _float m_fJumpTime{};
    _bool m_bJumpSound{};
    _bool m_bJump{};
    _float m_fJumpSpeedMultiplier{}; // 점프 속도 배율
    _bool m_bDoubleJump{};         // 더블 점프 여부
    _bool m_bDoubleJumpSound{};    // 더블 점프 사운드 재생 여부
    _bool m_JumpFall{};
    
public:
    static CPlayer_Jump* Create(void* pArg);
	virtual void Free();
    
    // 점프 속도 조절 함수들
    void Set_JumpSpeedMultiplier(_float multiplier) { m_fJumpSpeedMultiplier = multiplier; }
    _float Get_JumpSpeedMultiplier() const { return m_fJumpSpeedMultiplier; }
};

END