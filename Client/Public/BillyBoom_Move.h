#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Engine)
class CActor;
END 
BEGIN(Client)

class CBillyBoom_Move : public CStateMachine
{
public:
	struct MOVE_DESC : STATEMACHINE_DESC
	{ 
	   const _float* fLength{};
	};

private:
    CBillyBoom_Move();
    virtual ~CBillyBoom_Move() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

private:
    const _float* m_fLength{};

public:
    static CBillyBoom_Move* Create(void* pArg);
	virtual void Free();
};

END