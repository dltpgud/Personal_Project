#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Engine)

END

BEGIN(Client)

class CBoomBot_Dead : public CStateMachine
{
public:
    struct DEAD_DESC : STATEMACHINE_DESC
    {
    };

private:
    CBoomBot_Dead();
    virtual ~CBoomBot_Dead() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual HRESULT Initialize(void* pArg) override;

public:
    static CBoomBot_Dead* Create(void* pArg);
	virtual void Free();
};

END