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
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual HRESULT Initialize(void* pArg) override;

public:
    static CJetFly_Idle* Create(void* pArg);
	virtual void Free();
};

END