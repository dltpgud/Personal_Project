
#include "VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CVIBuffer{pDevice, pContext}
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& Prototype)
    : CVIBuffer{Prototype}
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_iNumVertices = 2; // 라인 2정점
    m_iNumVertexBuffers = 1;
    m_iIndexStride = 0;
    m_iNumIndexices = 0;
    m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    VTXPOSTEX initVerts[2] ;

    initVerts[0].vPosition = _float3(0.f, 0.f, 0.f);
    initVerts[0].vTexcoord = _float2(0.0f, 0.f);

    initVerts[1].vPosition = _float3(0.f, 0.f, 0.f);
    initVerts[1].vTexcoord = _float2(0.0f, 0.f);
    
    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    m_InitialDesc.pSysMem = initVerts;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

  #pragma region INDEX_BUFFER

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    m_iNumIndexices = 2;
    m_iIndexStride = sizeof(_ushort);
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;

    _ushort indices[2] = {0, 1};
    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndexices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;
    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    m_InitialDesc.pSysMem = indices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;
   
#pragma endregion


    return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
    return S_OK;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Trail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
    CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Trail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Trail::Free()
{
    __super::Free();
}
