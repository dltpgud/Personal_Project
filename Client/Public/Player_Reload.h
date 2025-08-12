#pragma once

#include "Player_StateNode.h"

BEGIN(Client)

class CPlayer_Reload : public CPlayer_StateNode
{
private:
    CPlayer_Reload();
    virtual ~CPlayer_Reload() = default;

public:
    virtual HRESULT Initialize(void* pDesc) override;
    virtual void State_Enter(_uint* pState) override;
    virtual _bool State_Processing(_float fTimeDelta, _uint* pState) override;
    virtual _bool State_Exit(_uint* pState) override;
    virtual void Init_CallBack_Func() override;
    virtual _bool IsActive(_uint stateFlags) const override;
    virtual void SetActive(_bool active, _uint* pState) override;
    virtual _bool CanEnter(_uint* pState) override;
    virtual _bool CheckInputCondition(_uint stateFlags) override;

private:
    HRESULT UI_CallBack();

private:
    class CShootingUI* m_pShootingUI{};

public:
    static CPlayer_Reload* Create(void* pArg);
	virtual void Free();
};

END