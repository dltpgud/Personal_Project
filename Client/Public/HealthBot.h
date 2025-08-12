#pragma once
#include "Client_Defines.h"
#include "Actor.h"
#include "InteractiveUI.h"
BEGIN(Engine)

END

BEGIN(Client)
class CInteractiveUI;
class CPlayerUI;
class CHealthBot final : public CActor
{
public:
    enum PARTOBJID
    {
        PART_BODY,
        PART_WEAPON,
        PART_EFFECT,
        PART_END
    };
    enum STATE
    {
      ST_Idle,
      ST_Interactive,
      ST_Dead,
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
    void    intersect(_float fTimedelta);

private:
    _bool m_bInteract = false;
    CInteractiveUI* m_pInteractiveUI = { nullptr };
    _bool m_bOpenUI = false;
    _bool m_bHealth = false;
    _float m_fTimeSum{ 0.f };
    _bool m_bSound{ true };
public:
    static CHealthBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
