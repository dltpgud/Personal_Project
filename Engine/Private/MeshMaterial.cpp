#include "MeshMaterial.h"
#include "Shader.h"
CMeshMaterial::CMeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{pDevice}, m_pContext{pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}
/*
HRESULT CMeshMaterial::Initialize(_uint* iNumTexture, const _tchar*** pTextureFilePath)
{
    for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        for (_uint j = 0; j < iNumTexture[i]; j++)
        {
            ID3D11ShaderResourceView* pSRV = {nullptr};

            HRESULT hr{};
            _tchar szExt[MAX_PATH] = {};
            _wsplitpath_s(pTextureFilePath[i][j], nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

            if (!lstrcmpW(szExt, TEXT(".dds")))
                hr = CreateDDSTextureFromFile(m_pDevice, pTextureFilePath[i][j], nullptr, &pSRV);
            else if (!lstrcmpW(szExt, TEXT(".tga")))
                hr = E_FAIL;
            else
                hr = CreateWICTextureFromFile(m_pDevice, pTextureFilePath[i][j], nullptr, &pSRV);

            if (FAILED(hr))
            {
                MSG_BOX("Failed : LoadTexture");
                return E_FAIL;
            }

            m_Materials[i].push_back(pSRV);
        }
    }

    return S_OK;
}*/

HRESULT CMeshMaterial::Bind_ShaderResource(CShader* pShader, aiTextureType eTextureType, _uint iIndex,
                                           const _char* pConstantName)
{
    if (iIndex >= m_Materials[eTextureType].size())
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_Materials[eTextureType][iIndex]);
}

HRESULT CMeshMaterial::Initialize(HANDLE& hFile)
{
    DWORD dwByte{};
    _bool bReadFile{};

    _uint iNumTexture[AI_TEXTURE_TYPE_MAX]{};
    bReadFile = ReadFile(hFile, iNumTexture, sizeof(_uint) * AI_TEXTURE_TYPE_MAX, &dwByte, nullptr);

    for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
    {
        for (_uint k = 0; k < iNumTexture[j]; k++)
        {
            _tchar MaterialPath[MAX_PATH]{};

            bReadFile = ReadFile(hFile, MaterialPath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

            ID3D11ShaderResourceView* pSRV = { nullptr };

            HRESULT hr{};
            _tchar			szExt[MAX_PATH] = {};
            _wsplitpath_s(MaterialPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

            if (!lstrcmpW(szExt, TEXT(".dds")))
                hr = CreateDDSTextureFromFile(m_pDevice, MaterialPath, nullptr, &pSRV);
            else if (!lstrcmpW(szExt, TEXT(".tga")))
                hr = E_FAIL;
            else
                hr = CreateWICTextureFromFile(m_pDevice, MaterialPath, nullptr, &pSRV);

            if (FAILED(hr))
            {
                MSG_BOX("Failed : LoadTexture");
                return E_FAIL;
            }

            m_Materials[j].push_back(pSRV);
        }
    }

    return S_OK;
}

/*
CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint* iNumTexture,
                                     const _tchar*** pTextureFilePath)
{
    CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iNumTexture, pTextureFilePath)))
    {
        MSG_BOX("Failed To Created : CMeshMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}*/

CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE& hFile)
{
    CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hFile)))
    {
        MSG_BOX("Failed To Created : CMeshMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMeshMaterial::Free()
{
    __super::Free();

    for (auto& Textures : m_Materials)
    {
        for (auto& pSRV : Textures) Safe_Release(pSRV);
        Textures.clear();
    }

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
