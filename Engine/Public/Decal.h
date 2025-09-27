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
    HRESULT Initialize_PoolObj(const DECAL_DESC& DecalDesc);
    HRESULT Initialize(const DECAL_DESC& DecalDesc);
    void Update(_float fTimeDelta) ;
    HRESULT Render(class CShader* pShader) ;
    _bool Get_Dead() { return m_bDead ;}

private:
    class CVIBuffer_Cube* m_pVIBufferCom = nullptr; 
    class CTexture* m_pTextureCom{};
    ID3D11Device* m_pDevice{};
    ID3D11DeviceContext* m_pContext{};

private:
    _float m_fLifeTime{}, m_fDecalTime{};
    _float4x4 m_WorldMatInv;
    _bool m_bDead{};
    _float3 m_Tangent{}, m_Binormal{}, m_Normal{};

public:
    static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* FilePath, const _uint& TexNum);
    virtual void Free() override;
};
END
