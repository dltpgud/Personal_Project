#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Engine)
class CActor;
END

BEGIN(Client)

class CJetFly_Dead : public CStateMachine
{
public:
    struct DEAD_DESC : STATEMACHINE_DESC
    {
    };

private:
    CJetFly_Dead();
    virtual ~CJetFly_Dead() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

public:
    static CJetFly_Dead* Create(void* pArg);
	virtual void Free();
};

END