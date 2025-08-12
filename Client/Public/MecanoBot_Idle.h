#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Client)

class CMecanoBot_Idle : public CStateMachine
{
private:
    CMecanoBot_Idle();
    virtual ~CMecanoBot_Idle() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

public:
    static CMecanoBot_Idle* Create(void* pArg);
	virtual void Free();
};

END