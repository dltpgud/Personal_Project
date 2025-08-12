#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)
class CActor;
END

BEGIN(Client)

class CBoomBot_Hit : public CStateMachine
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
    CBoomBot_Hit();
    virtual ~CBoomBot_Hit() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

private:
    const _int* m_HitType{};

public:
    static CBoomBot_Hit* Create(void* pArg);
	virtual void Free();
};

END