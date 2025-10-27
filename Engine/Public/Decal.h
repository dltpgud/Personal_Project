#pragma once
#include "Base.h"

BEGIN(Engine)

class CDecal : public CBase
{
public:
    CDecal();
    CDecal(const CDecal& Prototype);
    virtual ~CDecal() = default;

public:
    HRESULT Initialize_Proto();
    HRESULT Initialize(void* pArg);
    void Update(_float fTimeDelta);
    HRESULT Render(class CShader* pShader);

    _uint Get_LifeState() const
    {
        return m_LifeState;
    }
    DECAL_DESC::TYPE Get_Type() const
    {
        return (DECAL_DESC::TYPE)m_iDecalType;
    }
    const _float4x4& Get_WorldInv() const
    {
        return m_WorldMatInv;
    }
    _float Get_LifeTime() const
    {
        return m_fLifeTime;
    }
    _float Get_DecalTime() const
    {
        return m_fDecalTime;
    }
    _int Get_TexIndex() const
    {
        return m_iTexIndex;
    }
    _bool Is_Normal() const
    {
        return m_bNormal;
    }
    _float3 Get_Pos() const
    {
        return m_fDecalPos;
    }
    _float3 Get_Dir() const
    {
        return m_fDecalDir;
    }
    _float3 Get_Size() const
    {
        return m_fDecalSize;
    }
    _uint Get_iDecalType() const
    {
        return m_iDecalType;
    }
    _uint Get_iDecalID() const
    {
        return m_iDecalID;
    }
    _wstring Get_ProtoKey() const
    {
        return m_ProtoKey;
    }
 private:
    _float m_fLifeTime{}, m_fDecalTime{};
    _float3 m_fDecalPos{}, m_fDecalDir{}, m_fDecalSize{};
    _float4x4 m_WorldMatInv{};
    _uint m_LifeState = {OBJ_NOEVENT};
    _uint m_iDecalType= {};
    _bool m_bNormal{};
    _int m_iTexIndex{};
    _uint m_iDecalID{};
    _float m_fDeltaScaling{};
    _wstring m_ProtoKey;
    class CGameInstance* m_pGameInstance{}; 

public:
    static CDecal* Create();
    virtual CDecal* Clone(void* pArg);
    virtual void Free() override;
};
END
