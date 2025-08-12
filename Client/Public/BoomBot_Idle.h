#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Client)

class CBoomBot_Idle : public CStateMachine
{
private:
        CBoomBot_Idle();
    virtual ~CBoomBot_Idle() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

public:
    static CBoomBot_Idle* Create(void* pArg);
	virtual void Free();
};

END