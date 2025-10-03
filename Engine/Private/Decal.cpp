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

HRESULT CDecal::Initialize(void* pArg, CTexture* pTextureFromProto)
{
    if (m_LifeState == OBJ_POOL)
        m_LifeState = OBJ_NOEVENT;
    
   
    DECAL_DESC* pDecalDesc = static_cast<DECAL_DESC*>(pArg);
    m_fLifeTime = pDecalDesc->fLifeTime;
    m_iDecalType = pDecalDesc->iType;
    m_bNormal = pDecalDesc->bNormal;
    m_fDecalTime = 0.f;
    m_iTexIndex = pDecalDesc->iTexIndex;

    if (pTextureFromProto)
    {
        m_pTextureCom = pTextureFromProto;
    }

    if (m_iDecalType == DECAL_DESC::TYPE_SSD)
    {
        XMStoreFloat3(&m_fDecalPos, pDecalDesc->vPos);
        XMStoreFloat3(&m_fDecalDir, pDecalDesc->vDir);
        m_fDecalSize = {pDecalDesc->fSize, pDecalDesc->fSize, pDecalDesc->fDepth};
    }
    else
    {
        _vector n = XMVector3Normalize(pDecalDesc->vNormal);
        _vector t = XMVector3Normalize(XMVector3Orthogonal(n));
        _vector b = XMVector3Normalize(XMVector3Cross(n, t));
        _matrix TBN = _matrix(t, b, n, XMVectorSet(0, 0, 0, 1));
        _matrix S = XMMatrixScaling(pDecalDesc->fSize, pDecalDesc->fSize, pDecalDesc->fDepth);
        _matrix T = XMMatrixTranslationFromVector(pDecalDesc->vPos + n * 0.001f);
        _matrix world = S * TBN * T;
        _matrix invWorld = XMMatrixInverse(nullptr, world);
        XMStoreFloat4x4(&m_WorldMatInv, invWorld);
        XMStoreFloat3(&m_Tangent, t);
        XMStoreFloat3(&m_Binormal, b);
        XMStoreFloat3(&m_Normal, n);
    }
    return S_OK;
}

void CDecal::Update(_float fTimeDelta)
{
    m_fDecalTime += fTimeDelta;
    m_fLifeTime -= fTimeDelta;

    if (m_fLifeTime <= 0.f)
        m_LifeState = OBJ_POOL;
}

HRESULT CDecal::Render(CShader* pShader)
{
    if(true == m_bNormal)
    if (FAILED(m_pTextureCom->Bind_ShaderResource(pShader, "g_DecalNormalAtlas", 0)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(pShader, "g_DecalAtlas", 0)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_DecalFade", &m_fLifeTime, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fDecalTime", &m_fDecalTime, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_bDecalNormal", &m_bNormal, sizeof(_bool))))
        return E_FAIL;

    int iPass = 7;
    if (m_iDecalType == DECAL_DESC::TYPE_SSD)
    {
        if (FAILED(pShader->Bind_RawValue("g_fDecalPos", &m_fDecalPos, sizeof(_float3))))
            return E_FAIL;
        
        if (FAILED(pShader->Bind_RawValue("g_fDecalDir", &m_fDecalDir, sizeof(_float3))))
            return E_FAIL;

        if (FAILED(pShader->Bind_RawValue("g_DecalhalfSize", &m_fDecalSize, sizeof(_float3))))
            return E_FAIL;
    }
    else
    {
        iPass = 8;
        if (FAILED(pShader->Bind_RawValue("g_fDecalTangent", &m_Tangent, sizeof(_float3))))
            return E_FAIL;
        if (FAILED(pShader->Bind_RawValue("g_fDecalBinormal", &m_Binormal, sizeof(_float3))))
            return E_FAIL;
        if (FAILED(pShader->Bind_RawValue("g_fDecalNormal", &m_Normal, sizeof(_float3))))
            return E_FAIL;
        if (FAILED(pShader->Bind_Matrix("g_WorldMatrixInv", &m_WorldMatInv)))
            return E_FAIL;
    }

    pShader->Begin(iPass);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

    return S_OK;
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* FilePath,
                       const _uint& TexNum)
{
    CDecal* pInstance = new CDecal(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Proto(FilePath, TexNum)))
    {
        MSG_BOX("Failed to Created : CDecal");
        Safe_Release(pInstance);
    }

    return pInstance;
}
CDecal* CDecal::Clone(void* pArg, CTexture* pTextureFromProto )
{
    CDecal* pInstance = new CDecal(*this);

    if (FAILED(pInstance->Initialize(pArg, pTextureFromProto)))
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
