#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)
class CActor;
END

BEGIN(Client)

class CBillyBoom_Barre : public CStateMachine
{
public:
	struct ATTACK_DESC : STATEMACHINE_DESC
	{
       const _float4x4* pPerantPartBonMatrix{};
       const _float4x4* pPerantWorldMat{};
	};

    enum ANIM_INDEX
    {
       BARRE_IN,
       BARRE_PRESHOOT,
       BARRE_SHOOT,
       ANIM_END
    };

    private:
    CBillyBoom_Barre();
    virtual ~CBillyBoom_Barre() = default;
    HRESULT Make_Barre();

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func();
    virtual HRESULT Initialize(void* pArg);


private: 
    const _float4x4* m_pPerantPartBonMatrix{};
    const _float4x4* m_pPerantWorldMat{};
    _float m_pDamage{20.f};
public:
    static CBillyBoom_Barre* Create(void* pArg);
	virtual void Free();
};

END