#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Engine)

END

BEGIN(Client)

class CJetFly_Attack : public CStateMachine
{
public:
	struct ATTACK_DESC : STATEMACHINE_DESC
    {
       const _float4x4* pPerantPartBonMatrix{};
       const _float4x4* pPerantWorldMat{};
	   const _float* fLength{};
	};

    enum ANIM_INDEX
    {
        BARRELROLL_LEFT,
        BARRELROLL_RIGHT,
        SHOOT,
        ANIM_END
    };

    private:
        CJetFly_Attack();
    virtual ~CJetFly_Attack() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual void Init_CallBack_Func() override;
    virtual HRESULT Initialize(void* pArg) override;

private:
    void Make_Bullet();

private:
    const _float*m_fLength{};
    const _float4x4* m_pPerantPartBonMatrix{};
    const _float4x4* m_pPerantWorldMat{};
 
public:
    static CJetFly_Attack* Create(void* pArg);
	virtual void Free();
};

END