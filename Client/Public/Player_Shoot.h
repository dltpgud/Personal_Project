#pragma once

#include "Player_StateNode.h"

BEGIN(Client)

class CPlayer_Shoot : public CPlayer_StateNode
{
private:
    CPlayer_Shoot();
    virtual ~CPlayer_Shoot() = default;

public:
    virtual HRESULT Initialize(void* pDesc) override;
    virtual void State_Enter(_uint* pState, _uint* pPreState) override;
    virtual _bool State_Processing(_float fTimeDelta, _uint* pState, _uint* pPreState) override;
    virtual _bool State_Exit(_uint* pState) override;
    virtual void Init_CallBack_Func() override;
    virtual _bool IsActive(_uint stateFlags) const override;
    virtual void SetActive(_bool active, _uint* pState) override;
    virtual _bool CanEnter(_uint* pState) override;
    virtual _bool CheckInputCondition(_uint stateFlags) override;

private:
    HRESULT UI_CallBack();

private:
    _bool m_bAutoFire{};
    _float m_fLastFireTime{ 0.f }; 

private:
	class CPlayer_ShootingStateUI* m_pShootingUI{};
    class CPlayer_ShootingStateUI* m_pAutoShootingUI{};

public:
    static CPlayer_Shoot* Create(void* pArg);
	virtual void Free();
};

END