#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

END

BEGIN(Client)
class CTrigger final : public CGameObject
{
    enum Trigger_Type{ TYPE_ASTAR,TYPE_LAVA   };

public:
    struct CTrigger_DESC : CGameObject::GAMEOBJ_DESC
    {
        _int iColType{};
        _int iTriggerType{};
    };

private:
    CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTrigger(const CTrigger& Prototype);
    virtual ~CTrigger() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    HRESULT Set_TriggerZone(_int Type);

private:
    HRESULT Add_Components();
    HRESULT Set_Coll_Info();

private:
    _bool m_bTriggered = false;
    _int m_iLEVEL{};
    _int m_iCoType{};
    _float m_fRadius{1.f};
    _float3 m_fRot{0.f, 0.f, 0.f};
    _float3 m_fExtents{1.f, 1.f, 1.f};
    _float3 m_fCenter{0.f, 0.f, 0.f};

public:
    static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END