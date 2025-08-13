#pragma once

#include "Player_StateNode.h"

BEGIN(Client)

class CPlayer_Run : public CPlayer_StateNode
{
private:
    CPlayer_Run();
    virtual ~CPlayer_Run() = default;

public:
    virtual HRESULT Initialize(void* pDesc);
    virtual void State_Enter(_uint* pState);
    virtual _bool State_Processing(_float fTimeDelta, _uint* pState);
    virtual _bool State_Exit(_uint* pState);
    virtual void Init_CallBack_Func();
    virtual _bool IsActive(_uint stateFlags) const override;
    virtual void SetActive(_bool active, _uint* pState) override;
    virtual _bool CanEnter(_uint* pState) override;
    virtual _bool CheckInputCondition(_uint stateFlags) override;

private:
    

public:
    static CPlayer_Run* Create(void* pArg);
	virtual void Free();
};

END