#pragma once
#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Engine)

END 
BEGIN(Client)

class CJetFly_Move : public CStateMachine
{
public:
	struct MOVE_DESC : STATEMACHINE_DESC
	{
	   const _float* fLength{};
	};

	enum ANIM_INDEX {
        GO,BACK,LEFT,RIGHT,
		ANIM_END
	};

private:
    CJetFly_Move();
    virtual ~CJetFly_Move() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual HRESULT Initialize(void* pArg) override;

private:
    const _float* m_fLength{};
    _bool m_bRot{};

public:
        static CJetFly_Move* Create(void* pArg);
	virtual void Free();
};

END