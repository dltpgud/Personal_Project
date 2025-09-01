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
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual HRESULT Initialize(void* pArg) override;

public:
    static CGunPawn_Idle* Create(void* pArg);
	virtual void Free();
};

END