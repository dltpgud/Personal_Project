#include "Texture.h"
#include "Shader.h"
CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

CTexture::CTexture(const CTexture& Prototype)
    : CComponent{Prototype}, m_SRVs{Prototype.m_SRVs}, m_iNumTextures{Prototype.m_iNumTextures}
{
    for (auto& pSRV : m_SRVs) Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
    m_iNumTextures = iNumTextures;

    m_SRVs.reserve(iNumTextures);

    for (size_t i = 0; i < iNumTextures; i++)
    {
        _tchar szTextureFilePath[MAX_PATH] = TEXT("");

        wsprintf(szTextureFilePath, pTextureFilePath, i);

        _tchar szEXT[MAX_PATH] = TEXT("");

        _wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

        HRESULT hr = {};

        ID3D11ShaderResourceView* pSRV = {nullptr};

        if (false == lstrcmpW(szEXT, TEXT(".dds")))
            hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

        else if (false == lstrcmpW(szEXT, TEXT(".tga")))
            hr = E_FAIL;

        else
            hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

        if (FAILED(hr))
            return E_FAIL;

        m_SRVs.push_back(pSRV);
    }
    return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
    if (iTextureIndex >= m_iNumTextures)
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath,
                           _uint iNumTextures)
{
    CTexture* pInstance = new CTexture(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
    {
        MSG_BOX("Failed to Created : CTexture");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
    CTexture* pInstance = new CTexture(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CTexture");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTexture::Free()
{
    __super::Free();

    for (auto& pSRV : m_SRVs) Safe_Release(pSRV);
    m_SRVs.clear();
}
