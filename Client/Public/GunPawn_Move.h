#pragma once

#include "Client_Defines.h"
#include "StateMachine.h"

BEGIN(Engine)

END 
BEGIN(Client)

class CGunPawn_Move : public CStateMachine
{
public:
	struct MOVE_DESC : STATEMACHINE_DESC
	{
	   const _float* fLength{};
	};

	enum ANIM_INDEX {
        GO,BACK,LEFT,RIGHT,JETPACK,
		ANIM_END
	};

private:
    CGunPawn_Move();
    virtual ~CGunPawn_Move() = default;

public:
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim) override;
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim) override;

private:
    virtual HRESULT Initialize(void* pArg) override;

private:
    const _float* m_fLength{};
public:
        static CGunPawn_Move* Create(void* pArg);
	virtual void Free();
};

END