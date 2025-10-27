#include "VIBuffer_DecalCube.h"

CVIBuffer_DecalCube::CVIBuffer_DecalCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instancing{pDevice, pContext}
{
}

CVIBuffer_DecalCube::CVIBuffer_DecalCube(const CVIBuffer_DecalCube& Prototype) : CVIBuffer_Instancing{Prototype}
{
}

HRESULT CVIBuffer_DecalCube::Initialize_Prototype()
{
    m_iVertexStride = sizeof(VTXDECAL);
    m_iNumVertices = 8;
    m_iIndexStride = sizeof(_uint);
    m_iNumIndexPerInstance = 36;
    m_iNumIndexices = m_iNumIndexPerInstance;
    m_iNumVertexBuffers = 2;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // 초기 최대 인스턴스 수 (필요시 자동 확장)
    m_iMaxInstances = 10000;

#pragma region Vertex Buffer
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    VTXDECAL verts[8] = {
        {{-0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}}, {{0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}},
        {{0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}}, {{-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}},
        {{-0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}},   {{0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}},
        {{0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}},   {{-0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}},
    };

    m_InitialDesc.pSysMem = verts;
    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;
#pragma endregion

#pragma region Index Buffer
    _uint idx[36] = {1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7, 4, 5, 1, 4, 1, 0,
                     3, 2, 6, 3, 6, 7, 5, 4, 7, 5, 7, 6, 0, 1, 2, 0, 2, 3};

    m_BufferDesc.ByteWidth = sizeof(idx);
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    m_InitialDesc.pSysMem = idx;
    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;
#pragma endregion

#pragma region Instance Buffer (Dynamic)
    Create_InstanceBuffer(m_iMaxInstances);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_DecalCube::Initialize(void* pArg)
{
    return S_OK;
}

void CVIBuffer_DecalCube::Create_InstanceBuffer(UINT maxCount)
{
    Safe_Release(m_pVBInstance);

    ZeroMemory(&m_InstanceBufferDesc, sizeof(m_InstanceBufferDesc));
    m_iInstanceVertexStride = sizeof(DecalInstanceData);
    m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_InstanceBufferDesc.ByteWidth = m_iInstanceVertexStride * maxCount;

    if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance)))
        MSG_BOX("InstanceBuffer 생성 실패");

    m_iMaxInstances = maxCount;
}

void CVIBuffer_DecalCube::Update(const vector<DecalInstanceData>& instanceData)
{
    m_iNumInstance = (_uint)instanceData.size();
    if (m_iNumInstance == 0)
        return;

    // capacity 초과 시 확장 (2배씩 증가)
    if (m_iNumInstance > m_iMaxInstances)
    {
        Create_InstanceBuffer(m_iMaxInstances * 2);
    }

    // Map / Unmap (WRITE_DISCARD)
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (SUCCEEDED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped)))
    {
        memcpy(mapped.pData, instanceData.data(), sizeof(DecalInstanceData) * m_iNumInstance);
        m_pContext->Unmap(m_pVBInstance, 0);
    }
}

CVIBuffer_DecalCube* CVIBuffer_DecalCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_DecalCube* pInstance = new CVIBuffer_DecalCube(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create: CVIBuffer_DecalCube");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_DecalCube::Clone(void* pArg)
{
    CVIBuffer_DecalCube* pInstance = new CVIBuffer_DecalCube(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone: CVIBuffer_DecalCube");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_DecalCube::Free()
{
    __super::Free();
    Safe_Release(m_pVBInstance);
}
