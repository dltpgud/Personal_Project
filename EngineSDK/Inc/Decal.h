#pragma once
#include "Base.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Cube;
class CDecal : public CBase
{
public:
    CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDecal(const CDecal& Prototype);
    virtual ~CDecal() = default;

public:
    HRESULT Initialize_Proto(const _tchar* FilePath, const _uint& TexNum);
    HRESULT Initialize(void* pArg, CTexture* pTextureFromProto = nullptr);
    void Update(_float fTimeDelta);
    HRESULT Render(class CShader* pShader);
    _uint Get_LifeState()
    {
        return m_LifeState;
    }
    class CTexture* Get_Texture()
    {
        return m_pTextureCom;
    }

    DECAL_DESC::TYPE Get_Type() const
    {
        return (DECAL_DESC::TYPE)m_iDecalType;
    }
    const XMFLOAT4X4& Get_WorldInv() const
    {
        return m_WorldMatInv;
    }
    const XMFLOAT3& Get_Tangent() const
    {
        return m_Tangent;
    }
    const XMFLOAT3& Get_Binormal() const
    {
        return m_Binormal;
    }
    const XMFLOAT3& Get_Normal() const
    {
        return m_Normal;
    }

    float Get_LifeTime() const
    {
        return m_fLifeTime;
    }
    float Get_DecalTime() const
    {
        return m_fDecalTime;
    }
    int Get_TexIndex() const
    {
        return m_iTexIndex;
    }
    bool Is_Normal() const
    {
        return m_bNormal;
    }

    XMFLOAT3 Get_Pos() const
    {
        return m_fDecalPos;
    }
    XMFLOAT3 Get_Dir() const
    {
        return m_fDecalDir;
    }
    XMFLOAT3 Get_Size() const
    {
        return m_fDecalSize;
    }
    _uint Get_iDecalType() const
    {
        return m_iDecalType;
   }

private:
    class CVIBuffer_Cube* m_pVIBufferCom = nullptr;
    class CTexture* m_pTextureCom{};
    ID3D11Device* m_pDevice{};
    ID3D11DeviceContext* m_pContext{};

private:
    _float m_fLifeTime{}, m_fDecalTime{};
    _float3 m_fDecalPos{}, m_fDecalDir{}, m_fDecalSize{};
    _float4x4 m_WorldMatInv;
    _float3 m_Tangent{}, m_Binormal{}, m_Normal{};
    _uint m_LifeState = {OBJ_NOEVENT};
    _uint m_iDecalType= {};
    _bool m_bNormal{};
    _int m_iTexIndex;

public:
    static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* FilePath,
                          const _uint& TexNum);
    virtual CDecal* Clone(void* pArg, CTexture* pTextureFromProto = nullptr);
    virtual void Free() override;
};
END
