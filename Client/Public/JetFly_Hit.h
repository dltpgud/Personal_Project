#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)
class CActor;
END

BEGIN(Client)

class CJetFly_Hit : public CStateMachine
{
public:
    struct HIT_DESC : STATEMACHINE_DESC
    {

    };

private:
        CJetFly_Hit();
    virtual ~CJetFly_Hit() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

private:


public:
    static CJetFly_Hit* Create(void* pArg);
	virtual void Free();
};

END