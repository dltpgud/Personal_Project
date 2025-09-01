#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Engine)

END

BEGIN(Client)

class CBillyBoom_Intro : public CStateMachine
{
public:
	struct ATTACK_DESC : STATEMACHINE_DESC
	{
      _int*    iEmissiveMashNum{};
      _float*  fEmissivePower{};
      _float3* fEmissiveColor{};
	};

    private:
    CBillyBoom_Intro();
        virtual ~CBillyBoom_Intro() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual void Init_CallBack_Func() override;
    virtual HRESULT Initialize(void* pArg) override;
    void Set_Setting();
private:
    _int*    m_iEmissiveMashNum{};
    _float*  m_fEmissivePower{};
    _float3* m_fEmissiveColor{};
   public:
    static CBillyBoom_Intro* Create(void* pArg);
	virtual void Free();
};

END