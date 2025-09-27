#include "ComputeShader.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent(pDevice, pContext)
{
}

CComputeShader::CComputeShader(const CComputeShader& Proto)
    : CComponent(Proto), m_pComputeShader{Proto.m_pComputeShader}, m_pSRV{Proto.m_pSRV}, m_pUAV{Proto.m_pUAV}
{
}

HRESULT CComputeShader::Initialize_Proto()
{

    return S_OK;
}

HRESULT CComputeShader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CComputeShader::Bind_SRV(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_SRV(pConstantName, m_pSRV);
}

HRESULT CComputeShader::Compute(UINT X, UINT Y, UINT Z)
{
    m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);

    const _uint iInt{};
    m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pUAV, &iInt);
    m_pContext->CSSetShaderResources(0, 1, &m_pSRV);

    m_pContext->Dispatch(X, Y, Z);

    return S_OK;
}

HRESULT CComputeShader::Create_CS(_wstring strFilePath, _string strEntryPoint, ID3D11ComputeShader** ppComputeShader)
{
    _uint iHlslFlag = {0};

#ifdef _DEBUG
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                    strEntryPoint.c_str(), "cs_5_0", iHlslFlag, 0, &shaderBlob, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }

        if (shaderBlob)
            shaderBlob->Release();

        return hr;
    }

    hr = m_pDevice->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr,
                                        ppComputeShader);

    return hr;
}

HRESULT CComputeShader::Create_SRV(ID3D11Resource* pData, D3D11_SHADER_RESOURCE_VIEW_DESC* pSRVDesc)
{
    return m_pDevice->CreateShaderResourceView(pData, pSRVDesc, &m_pSRV);
}

HRESULT CComputeShader::Create_UAV(ID3D11Resource* pData, D3D11_UNORDERED_ACCESS_VIEW_DESC* pUAVDesc)
{
    return m_pDevice->CreateUnorderedAccessView(pData, pUAVDesc, &m_pUAV);
}

void CComputeShader::Free()
{
    __super::Free();
    Safe_Release(m_pComputeShader);
    Safe_Release(m_pComputeShader);
    Safe_Release(m_pSRV);
    Safe_Release(m_pUAV);
}
