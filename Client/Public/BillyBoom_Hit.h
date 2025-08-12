#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)
class CActor;
END

BEGIN(Client)

class CBillyBoom_Hit : public CStateMachine
{
public:

    struct HIT_DESC : STATEMACHINE_DESC
    {
        const _int* HitType{};
    };

	enum ANIM_INDEX
    {
        FrontHit,
        BackHit,
        LeftHit,
        RightHit,
        ANIM_END
    };

private:
        CBillyBoom_Hit();
    virtual ~CBillyBoom_Hit() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

private:
    const _int* m_HitType{nullptr};

public:
    static CBillyBoom_Hit* Create(void* pArg);
	virtual void Free();
};

END