#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)

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
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual HRESULT Initialize(void* pArg) override;

private:


public:
    static CJetFly_Hit* Create(void* pArg);
	virtual void Free();
};

END