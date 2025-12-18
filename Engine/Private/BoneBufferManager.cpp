#include "BoneBufferManager.h"
#include "Bone.h"


CBoneBufferManager::CBoneBufferManager()
{
}

HRESULT CBoneBufferManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, UINT maxInstances,
                                       UINT maxBones)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    m_maxInstances = maxInstances;
    m_maxBones = maxBones;

    UINT totalCount = maxInstances * maxBones;

    D3D11_BUFFER_DESC desc{};
    desc.ByteWidth = sizeof(_float4x4) * totalCount;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = sizeof(_float4x4);

    HRESULT hr = m_pDevice->CreateBuffer(&desc, nullptr, &m_pBuffer);
    if (FAILED(hr))
        return E_FAIL;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.ElementWidth = totalCount;

    return m_pDevice->CreateShaderResourceView(m_pBuffer, &srvDesc, &m_pSRV);
}

void CBoneBufferManager::UploadBones(UINT instanceID, const vector<CBone*>& Bones,
                                     const vector<_float4x4>& OffsetMatrices)
{
    D3D11_MAPPED_SUBRESOURCE mapped{};
    m_pContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    _float4x4* pData = reinterpret_cast<_float4x4*>(mapped.pData);
    UINT baseIndex = instanceID * m_maxBones;

    for (UINT i = 0; i < Bones.size(); ++i)
    {
        for (UINT i = 0; i < Bones.size(); ++i)
        {
            _matrix Final = Bones[i]->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&OffsetMatrices[i]);

            XMStoreFloat4x4(&pData[baseIndex + i], Final);
        }
    }

    m_pContext->Unmap(m_pBuffer, 0);
}

CBoneBufferManager* CBoneBufferManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, UINT maxInstances,
                                               UINT maxBones)
{
    CBoneBufferManager* pInstance = new CBoneBufferManager();

    if (FAILED(pInstance->Initialize( pDevice,  pContext, maxInstances, maxBones)))
    {
        MSG_BOX("Failed to Created : CBoneBufferManager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoneBufferManager::Free()
{
    __super::Free();
    Safe_Release( m_pDevice );
    Safe_Release (m_pContext );

    Safe_Release( m_pBuffer );
    Safe_Release( m_pSRV );
}
