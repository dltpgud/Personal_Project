#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Client)

class CBillyBoom_Idle : public CStateMachine
{
private:
    CBillyBoom_Idle();
    virtual ~CBillyBoom_Idle() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

public:
        static CBillyBoom_Idle* Create(void* pArg);
	virtual void Free();
};

END