#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

END

BEGIN(Tool)
class CTrigger final : public CGameObject
{
public:
    struct CTrigger_DESC : CGameObject::GAMEOBJ_DESC
    {
        _int Type{};
        _float fRadius{};
        _float3 fRot{};
        _float3 vExtents{};
        _float3 fCenter{};
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
    _tchar* Get_ProtoName();
    void SetDESC(_float fRadius, _float3 fRot, _float3 vExtents, _float3 fCenter);
    void Set_Type(_int Type);
    void Set_Trigger(_int Trigger)
    {
        m_iTrigger = Trigger;
    }

    _int Get_Type() {
    
    return m_Type;
    };
    _int Get_Trigger()
    {
      return  m_iTrigger ;
    }

private:
    _int m_Type{};
    _float m_fRadius{1.f};
    _float3 m_fRot{0.f,0.f,0.f};
    _float3 m_fExtents{1.f,1.f,1.f};
    _float3 m_fCenter{0.f,0.f,0.f};
    _int m_iTrigger{};

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();
    _tchar* m_Proto;
public:
    static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END