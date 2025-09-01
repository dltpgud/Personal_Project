#pragma once

#include "Player_StateNode.h"

BEGIN(Client)

class CPlayer_Idle : public CPlayer_StateNode
{
private:
    CPlayer_Idle();
    virtual ~CPlayer_Idle() = default;

public:
    virtual HRESULT Initialize(void* pDesc) override;
    virtual void State_Enter(_uint* pState, _uint* pPreState) override;
    virtual _bool State_Processing(_float fTimeDelta, _uint* pState, _uint* pPreState) override;
    virtual _bool State_Exit(_uint* pState) override;
    virtual void Init_CallBack_Func() override;
    virtual _bool IsActive(_uint stateFlags) const override;
    virtual void SetActive(_bool active, _uint* pState) override;
    virtual _bool CanEnter(_uint* pState)  override;
    virtual _bool CheckInputCondition(_uint stateFlags) override;

private:
    

public:
    static CPlayer_Idle* Create(void* pArg);
	virtual void Free();
};

END