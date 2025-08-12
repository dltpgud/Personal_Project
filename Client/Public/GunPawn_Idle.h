#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Client)

class CGunPawn_Idle : public CStateMachine
{
private:
    CGunPawn_Idle();
    virtual ~CGunPawn_Idle() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

public:
    static CGunPawn_Idle* Create(void* pArg);
	virtual void Free();
};

END