#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)
class CActor;
END

BEGIN(Client) 
 class CBody_BillyBoom;
class CBillyBoom_Laser : public CStateMachine
{
public:
    enum ANIM_INDEX
    {
        LASER_IN,
        LASER_PRESHOOT,
        LASER_SHOOTLOOP,
        ANIM_END
    };

    enum BONE_INDEX
    {
        LEFT_BONE,
        RIGHT_BONE,
        BONE_INDEX_END

    };


	struct ATTACK_DESC : STATEMACHINE_DESC
	{
            const _float4x4* pPerantPartBonMatrix[BONE_INDEX_END]{};
      const _float4x4* pPerantWorldMat{};
	};
private:
    CBillyBoom_Laser();
    virtual ~CBillyBoom_Laser() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func();
    virtual HRESULT Initialize(void* pArg);

private:
    HRESULT Make_Laser();

private:
    const _float4x4* m_pPerantPartBonMatrix[BONE_INDEX_END]{};
    const _float4x4* m_pPerantWorldMat{};
    _float m_pDamage{20.f};

    _float m_BeamY{-45.f};
    _float m_BeamZ{15.f};
    _uint m_iBoneIndex[BONE_INDEX_END]{};

public:
    static CBillyBoom_Laser* Create(void* pArg);
	virtual void Free();
};

END