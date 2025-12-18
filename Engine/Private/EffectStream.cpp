#include "EffectStream.h"
#include "GameInstance.h"
CEffectStream ::CEffectStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}


HRESULT CEffectStream::CreateStructuredBuffer(_uint elementCount, _uint stride, ID3D11Buffer** outBuffer,
                                              ID3D11ShaderResourceView** outSRV, ID3D11UnorderedAccessView** outUAV,
                                              const void* initData)
{
    if (!m_pDevice || elementCount == 0 || stride == 0)
        return E_FAIL;

    D3D11_BUFFER_DESC bd{};
    bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bd.ByteWidth = elementCount * stride;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = stride;
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = initData;

    HRESULT hr = m_pDevice->CreateBuffer(&bd, initData ? &sd : nullptr, outBuffer);
    if (FAILED(hr))
        return hr;

    // SRV 생성
    if (outSRV)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
        srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvd.Format = DXGI_FORMAT_UNKNOWN;
        srvd.Buffer.FirstElement = 0;
        srvd.Buffer.NumElements = elementCount;
        hr = m_pDevice->CreateShaderResourceView(*outBuffer, &srvd, outSRV);
        if (FAILED(hr))
            return hr;
    }

    // UAV 생성
    if (outUAV)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
        uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavd.Format = DXGI_FORMAT_UNKNOWN;
        uavd.Buffer.FirstElement = 0;
        uavd.Buffer.NumElements = elementCount;
        hr = m_pDevice->CreateUnorderedAccessView(*outBuffer, &uavd, outUAV);
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}

HRESULT CEffectStream::CreateRawBuffer(_uint byteWidth, ID3D11Buffer** outBuffer, ID3D11UnorderedAccessView** outUAV,
                                       _bool allowDrawIndirect)
{
    if (!m_pDevice || byteWidth == 0)
        return E_FAIL;

    // 16바이트 정렬 패딩
    byteWidth = (byteWidth + 15u) & ~15u;

    D3D11_BUFFER_DESC bd{};
    bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    bd.ByteWidth = byteWidth;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    if (allowDrawIndirect)
        bd.MiscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

    HRESULT hr = m_pDevice->CreateBuffer(&bd, nullptr, outBuffer);
    if (FAILED(hr))
        return hr;

    // UAV 생성 (Raw view)
    if (outUAV)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
        uavd.Format = DXGI_FORMAT_R32_TYPELESS;
        uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavd.Buffer.FirstElement = 0;
        uavd.Buffer.NumElements = byteWidth / 4;
        uavd.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

        hr = m_pDevice->CreateUnorderedAccessView(*outBuffer, &uavd, outUAV);
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}

HRESULT CEffectStream::Create_CS(_wstring strFilePath, _string strEntryPoint, ID3D11ComputeShader** ppComputeShader)
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
            string err((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
            MessageBoxA(nullptr, err.c_str(), "Compute Shader Compile Error", MB_OK | MB_ICONERROR);
            OutputDebugStringA(err.c_str());
            errorBlob->Release();
        }

        Safe_Release(shaderBlob);
        return hr;
    }

    hr = m_pDevice->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr,
                                        ppComputeShader);
    
    Safe_Release(shaderBlob);
    return hr;
}

void CEffectStream::Free()
{
    __super::Free();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
