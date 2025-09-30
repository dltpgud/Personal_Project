#include "CS_RayCastingDecal.h"

CCS_RayCastingDecal::CCS_RayCastingDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComputeShader(pDevice, pContext)
{
}

CCS_RayCastingDecal::CCS_RayCastingDecal(const CCS_RayCastingDecal& Proto) : CComputeShader(Proto)
{
}

HRESULT CCS_RayCastingDecal::Initialize_Proto()
{
    if (FAILED(Create_CS(L"../Bin/ShaderFiles/RaycastDecal_CS.hlsl", "CS_Raycast", &m_pComputeShader)))
        return E_FAIL;

    // CB
    D3D11_BUFFER_DESC cbd{};
    cbd.ByteWidth = sizeof(RayCB);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(m_pDevice->CreateBuffer(&cbd, nullptr, &m_pCB)))
        return E_FAIL;

    if (FAILED(CreateOutputs()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCS_RayCastingDecal::Initialize(void* pArg)
{
    return S_OK;
}

void CCS_RayCastingDecal::UpdateRayCB(const _float3& O, const _float3& D_norm, _uint numTris, const _float3& decalSize)
{
    D3D11_MAPPED_SUBRESOURCE m{};
    if (SUCCEEDED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &m)))
    {
        auto* cb = reinterpret_cast<RayCB*>(m.pData);
        cb->RayOrigin = O;
        cb->RayDir = D_norm; // ★ 정규화해서 넣기
        cb->NumTris = numTris;
        cb->DecalSize = decalSize;
        m_pContext->Unmap(m_pCB, 0);
    }
}

HRESULT CCS_RayCastingDecal::Compute(UINT X, UINT Y, UINT Z )
{
    const UINT clearMax[4] = {0x7f7fffff, 0, 0, 0};
    m_pContext->ClearUnorderedAccessViewUint(m_pUAV_MinT, clearMax);

    m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);
    m_pContext->CSSetConstantBuffers(0, 1, &m_pCB);

    ID3D11ShaderResourceView* srvs[2] = {m_pSRV_Pos, m_pSRV_Idx};
    m_pContext->CSSetShaderResources(0, 2, srvs);

    ID3D11UnorderedAccessView* uavs[3] = {m_pUAV_DecalWorld, m_pUAV_DecalWorldInv, m_pUAV_MinT};
    UINT initCounts[3] = {0, 0, 0};
    m_pContext->CSSetUnorderedAccessViews(0, 3, uavs, initCounts);

    m_pContext->Dispatch(X, 1, 1);

    //언바인드
    ID3D11ShaderResourceView* nullSRV[2] = {nullptr, nullptr};
    m_pContext->CSSetShaderResources(0, 2, nullSRV);
    ID3D11UnorderedAccessView* nullUAV[3] = {nullptr, nullptr, nullptr};
    m_pContext->CSSetUnorderedAccessViews(0, 3, nullUAV, initCounts);
    return E_NOTIMPL;
}

HRESULT CCS_RayCastingDecal::CreateOutputs()
{
    if (FAILED(CreateStructuredUAVSRV(m_pDevice, sizeof(_float4x4), 1, &m_pBuf_DecalWorld, &m_pUAV_DecalWorld,
                                      &m_pSRV_DecalWorld)))
        return E_FAIL;

    if (FAILED(CreateStructuredUAVSRV(m_pDevice, sizeof(_float4x4), 1, &m_pBuf_DecalWorldInv, &m_pUAV_DecalWorldInv,
                                      &m_pSRV_DecalWorldInv)))
        return E_FAIL;

    if (FAILED(CreateStructuredUAVSRV(m_pDevice, sizeof(_uint), 1, &m_pBuf_MinT, &m_pUAV_MinT, nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCS_RayCastingDecal::CreateStructuredUAVSRV(ID3D11Device* dev, UINT elemSize, UINT elemCount,
                                                    ID3D11Buffer** ppBuf, ID3D11UnorderedAccessView** ppUAV,
                                                    ID3D11ShaderResourceView** ppSRV)
{
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = elemSize * elemCount;
    bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | (ppSRV ? D3D11_BIND_SHADER_RESOURCE : 0);
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = elemSize;
    HRESULT hr = dev->CreateBuffer(&bd, nullptr, ppBuf);
    if (FAILED(hr))
        return hr;

    D3D11_UNORDERED_ACCESS_VIEW_DESC ud{};
    ud.Format = DXGI_FORMAT_UNKNOWN;
    ud.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    ud.Buffer.FirstElement = 0;
    ud.Buffer.NumElements = elemCount;
    hr = dev->CreateUnorderedAccessView(*ppBuf, &ud, ppUAV);
    if (FAILED(hr))
        return hr;

    if (ppSRV)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC sd{};
        sd.Format = DXGI_FORMAT_UNKNOWN;
        sd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        sd.Buffer.FirstElement = 0;
        sd.Buffer.NumElements = elemCount;
        hr = dev->CreateShaderResourceView(*ppBuf, &sd, ppSRV);
    }
    return hr;
}


CCS_RayCastingDecal* CCS_RayCastingDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCS_RayCastingDecal* pInstance = new CCS_RayCastingDecal(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Proto()))
    {
        MSG_BOX("Failed To Created : CCS_RayCastingDecal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCS_RayCastingDecal::Clone(void* pArg)
{
    CCS_RayCastingDecal* pInstance = new CCS_RayCastingDecal(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Created : CCS_RayCastingDecal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCS_RayCastingDecal::Free()
{
    __super::Free();


}
