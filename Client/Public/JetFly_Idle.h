#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Client)

class CJetFly_Idle : public CStateMachine
{
private:
    CJetFly_Idle();
    virtual ~CJetFly_Idle() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

public:
    static CJetFly_Idle* Create(void* pArg);
	virtual void Free();
};

END