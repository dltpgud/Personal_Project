#include "Decal.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : m_pDevice{pDevice}, m_pContext{pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

CDecal::CDecal(const CDecal& Prototype)
    : m_pDevice{Prototype.m_pDevice}, m_pContext{Prototype.m_pContext}, m_pVIBufferCom{Prototype.m_pVIBufferCom},
      m_pTextureCom{Prototype.m_pTextureCom}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pVIBufferCom);
    Safe_AddRef(m_pTextureCom);
}

HRESULT CDecal::Initialize_Proto(const _tchar* FilePath, const _uint& TexNum)
{
    m_pVIBufferCom = CVIBuffer_Cube::Create(m_pDevice, m_pContext);
    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, FilePath, TexNum);
    return S_OK;
}

HRESULT CDecal::Initialize_PoolObj(const DECAL_DESC& DecalDesc)
{
    m_fDecalTime = 0.f;
    m_bDead = false;
    _vector n = XMVector3Normalize(DecalDesc.vHitNormal);
    _vector t = XMVector3Normalize(XMVector3Orthogonal(n));
    _vector b = XMVector3Normalize(XMVector3Cross(n, t));
    _matrix TBN = _matrix(t, b, n, XMVectorSet(0, 0, 0, 1));
    _matrix S = XMMatrixScaling(DecalDesc.fSize, DecalDesc.fSize, DecalDesc.fDepth);
    _matrix T = XMMatrixTranslationFromVector(DecalDesc.vHitPoint + n * 0.001f);
    _matrix world = S * TBN * T;
    _matrix invWorld = XMMatrixInverse(nullptr, world);
    XMStoreFloat4x4(&m_WorldMatInv, invWorld);

    m_fLifeTime = DecalDesc.fLifeTime;

    // 셰이더로 넘겨줄 값
    XMStoreFloat3(&m_Tangent, t);
    XMStoreFloat3(&m_Binormal, b);
    XMStoreFloat3(&m_Normal, n);
    return S_OK;
   // return Initialize(DecalDesc);
}

HRESULT CDecal::Initialize(const DECAL_DESC& DecalDesc)
{
    _vector n = XMVector3Normalize(DecalDesc.vHitNormal); 
    _vector t = XMVector3Normalize(XMVector3Orthogonal(n));
    _vector b = XMVector3Normalize(XMVector3Cross(n, t));
    _matrix TBN = _matrix(t, b, n, XMVectorSet(0, 0, 0, 1));
    _matrix S = XMMatrixScaling(DecalDesc.fSize, DecalDesc.fSize, DecalDesc.fDepth);
    _matrix T = XMMatrixTranslationFromVector(DecalDesc.vHitPoint + n * 0.001f);
    _matrix world = S * TBN * T;
    _matrix invWorld = XMMatrixInverse(nullptr, world);
    XMStoreFloat4x4(&m_WorldMatInv, invWorld);

    m_fLifeTime = DecalDesc.fLifeTime;
 
    // 셰이더로 넘겨줄 값
    XMStoreFloat3(&m_Tangent, t);
    XMStoreFloat3(&m_Binormal, b);
    XMStoreFloat3(&m_Normal, n);
    return S_OK;
}

void CDecal::Update(_float fTimeDelta)
{
   m_fDecalTime += fTimeDelta;
   m_fLifeTime -= fTimeDelta;
   if (m_fLifeTime <= 0.f)
        m_bDead = true;
}

HRESULT CDecal::Render(CShader* pShader)
{
    if (FAILED(pShader->Bind_Matrix("g_WorldInvMatrix", &m_WorldMatInv)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(pShader, "g_DecalNormalAtlas", 0)))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(pShader, "g_DecalAtlas", 1)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_DecalFade", &m_fLifeTime, sizeof(_float))))
        return E_FAIL;
    if (FAILED(pShader->Bind_RawValue("g_fDecalTime", &m_fDecalTime, sizeof(_float))))
        return E_FAIL;
    if (FAILED(pShader->Bind_RawValue("g_fDecalTangent", &m_Tangent, sizeof(_float3))))
        return E_FAIL;
    if (FAILED(pShader->Bind_RawValue("g_fDecalBinormal", &m_Binormal, sizeof(_float3))))
        return E_FAIL;
    if (FAILED(pShader->Bind_RawValue("g_fDecalNormal", &m_Normal, sizeof(_float3))))
        return E_FAIL;
    
    pShader->Begin(7);

    m_pVIBufferCom->Bind_Buffers();
    
    m_pVIBufferCom->Render();
    
    return S_OK;
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* FilePath,const _uint& TexNum)
{
    CDecal* pInstance = new CDecal(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Proto(FilePath, TexNum)))
    {
        MSG_BOX("Failed to Created : CDecal");
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CDecal::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
