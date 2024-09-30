#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CVIBuffer{pDevice, pContext}
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& Prototype) : CVIBuffer{Prototype}
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_iNumVertices = 4;
    m_iIndexStride = sizeof(_ushort); // 4바이트로 할꺼면 _uint
    m_iNumIndexices = 6;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT; // 4바이트로 할꺼면 DXGI_FORMAT_R32_UINT
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    // D3D11_BUFFER_DESC : 생성할 버퍼를 서술하는 구조체 - m_BufferDesc
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    /* 할당하고자하는 메모리공간의 크기(Byte)*/
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;

    /* 버퍼의 속성 (정적, 동적) */

    /*1.버퍼가 쓰이는 방식*/
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; // 정적으로 만들면 락언락이 불가..
                                              // GPU가 버퍼의 자원을 읽고 써야 한다면 이 용도를 지정한다.
    /*2. 바인딩 플래그*/
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 정점 버퍼의 경우 이 플래그를 이용

    /*3.CPU가 버퍼에 접근히는 방식을 결정하는 플래그들을 지정한다*/
    m_BufferDesc.CPUAccessFlags = 0; // 0으로 설정하면 Cpu가 버퍼를 읽거나 쓰지 않는다.

    /*4. 기타 플래그 사용 여부*/
    m_BufferDesc.MiscFlags = 0; // 정점 버퍼에 대해서는 이 기타 플래그들을 시용할 필요가 없다.

    /*5. 구조적 버퍼(structured buffer)에 저장된 원소 하나의 크기(바이트 단위). */
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    // D3D11_SUBRESOURCE_DATA: 버퍼 초가화에 사용할 자료를 서술하는 구조체 - m_InitialDesc
    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];

    pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexcoord = _float2(0.0f, 0.f);

    pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexcoord = _float2(1.0f, 0.f);

    pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexcoord = _float2(1.0f, 1.f);

    pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexcoord = _float2(0.0f, 1.f);

    // pSysMem : 정점 버퍼를 초기화할 자료를 담은 시스템 메모리 배열을 가리키는 포인터.
    m_InitialDesc.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndexices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    _ushort* pIndices = new _ushort[m_iNumIndexices];

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    pIndices[3] = 0;
    pIndices[4] = 2;
    pIndices[5] = 3;

    m_InitialDesc.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;
#pragma endregion

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void* pArg)
{
    return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* pArg)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Rect::Free()
{
    __super::Free();
}
