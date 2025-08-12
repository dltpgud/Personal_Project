#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Engine)
class CActor;
END

BEGIN(Client)

class CBillyBoom_Dead : public CStateMachine
{
public:
    struct DEAD_DESC : STATEMACHINE_DESC
    {
        _int*    iEmissiveMashNum{};
        _float*  fEmissivePower{};
        _float3* fEmissiveColor{};
    };

private:
    CBillyBoom_Dead();
    virtual ~CBillyBoom_Dead() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func() {};
    virtual HRESULT Initialize(void* pArg);

private:
    _float   m_DeadTimeSum{};
    _bool    m_bEffect{};
    _int*    m_iEmissiveMashNum{};
    _float*  m_fEmissivePower{};
    _float3* m_fEmissiveColor{};

public:
    static CBillyBoom_Dead* Create(void* pArg);
	virtual void Free();
};

END