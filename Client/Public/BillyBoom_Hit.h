#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Engine)

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
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual HRESULT Initialize(void* pArg) override;

private:
    const _int* m_HitType{nullptr};

public:
    static CBillyBoom_Hit* Create(void* pArg);
	virtual void Free();
};

END