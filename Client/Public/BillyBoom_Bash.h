#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)

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
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual HRESULT Initialize(void* pArg) override;

private:
    _float m_fRimTimeSum{};

private:
    
   public:
    static CBillyBoom_Bash* Create(void* pArg);
	virtual void Free();
};

END