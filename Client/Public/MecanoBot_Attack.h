#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"
BEGIN(Engine)
class CActor;
END

BEGIN(Client)

class CMecanoBot_Attack : public CStateMachine
{
public:
	struct ATTACK_DESC : STATEMACHINE_DESC
    {
       const _float4x4* pParentBoneMat{};
        const _float4x4* pParentWorldMat{};
	   const _float* fLength{};
	};

private:
    CMecanoBot_Attack();
    virtual ~CMecanoBot_Attack() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta);
    virtual void Reset_StateMachine();

private:
    virtual void Init_CallBack_Func();
    virtual HRESULT Initialize(void* pArg);

private:
    void Make_Bullet();

private:
    const _float*m_fLength{};
    const _float4x4* m_pParentBoneMat{};
    const _float4x4* m_pParentWorldMat{};


public:
    static CMecanoBot_Attack* Create(void* pArg);
	virtual void Free();
};

END