#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)

END

BEGIN(Client)

class CGunPawn_Attack : public CStateMachine
{
public:
	struct ATTACK_DESC : STATEMACHINE_DESC
    {
       const _float4x4* pParentBoneMat{};
       const _float4x4* pParentWorldMat{};
	   const _float* fLength{};
	};

    enum ANIM_INDEX
    {
            GRENADE_PRESHOOT, GRENADE_SHOOT,
            ANIM_END
    };

    private:
        CGunPawn_Attack();
    virtual ~CGunPawn_Attack() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual void Init_CallBack_Func() override;
    virtual HRESULT Initialize(void* pArg) override;

private:
    HRESULT Make_Bullet();

private:
    const _float*m_fLength{};
    const _float4x4* m_pParentBoneMat{};
    const _float4x4* m_pParentWorldMat{};
   
 public:
    static CGunPawn_Attack* Create(void* pArg);
	virtual void Free();
};

END