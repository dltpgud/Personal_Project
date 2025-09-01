#pragma once
#include "Client_Defines.h"
#include "Actor.h"
#include "InteractiveUI.h"
BEGIN(Engine)

END

BEGIN(Client)
class CInteractiveUI;
class CPlayer_HpUI;
class CHealthBot final : public CActor
{
public:
    enum PARTOBJID
    {
        PART_BODY,
        PART_END
    };
    enum STATE
    {
      ST_IDLE     = 0 ,
      ST_INTERACT = 1 << 0, 
      ST_DEAD     = 1 << 1
    };                   

    enum FLAG : _uint
    {
        FLAG_NONE = 1 << 2,
        FLAG_OPENUI = 1 << 3,    
        FLAG_INTERACTUI = 1 << 4   
    };

private:
    CHealthBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CHealthBot(const CHealthBot& Prototype);
    virtual ~CHealthBot() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    HRESULT Add_Components();
    HRESULT Add_PartObjects();
    HRESULT Init_CallBack();
    void SetState(_uint flag, bool value)
    {
        if (value)
            m_iState |= flag;
        else
            m_iState &= ~flag;
    }

    bool HasState(_uint flag) const
    {
        return (m_iState & flag) != 0;
    }

private:
   
    CInteractiveUI* m_pInteractiveUI = { nullptr };
   
public:
    static CHealthBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
