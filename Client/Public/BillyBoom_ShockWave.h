#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)

END

BEGIN(Client)

class CBillyBoom_ShockWave : public CStateMachine
{
public:
	struct ATTACK_DESC : STATEMACHINE_DESC
	{
	  const _float4x4* pPerantPartBonMatrix{};
      const _float4x4* pPerantWorldMat{};
	};

    enum ANIM_INDEX
    {
       SHOCKWAVE_IN,
       SHOCKWAVE_PRESHOOT,
       SHOCKWAVE_SHOOT,
       ANIM_END
    };

private:
    CBillyBoom_ShockWave();
    virtual ~CBillyBoom_ShockWave() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual void Init_CallBack_Func() override;
    virtual HRESULT Initialize(void* pArg) override;

private:
    HRESULT Make_ShockWave();

private:
    const _float4x4* m_pPerantPartBonMatrix{};
    const _float4x4* m_pPerantWorldMat{};
    _float m_fRimTimeSum{};

public:
    static CBillyBoom_ShockWave* Create(void* pArg);
	virtual void Free();
};

END