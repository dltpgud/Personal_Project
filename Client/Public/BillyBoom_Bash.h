#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)
class CActor;
END

BEGIN(Client)

class CBillyBoom_Bash : public CStateMachine
{
public:
	struct ATTACK_DESC : STATEMACHINE_DESC
	{
	};

    enum ANIM_INDEX
    {
         BASH_PRESHOOT,
         BASH_SHOOT,
       ANIM_END
    };

private:
    CBillyBoom_Bash();
    virtual ~CBillyBoom_Bash() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

private:
    
   public:
    static CBillyBoom_Bash* Create(void* pArg);
	virtual void Free();
};

END