#include "VIBuffer_Point.h"

CVIBuffer_Point::CVIBuffer_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CVIBuffer{pDevice, pContext}
{
}

CVIBuffer_Point::CVIBuffer_Point(const CVIBuffer_Point& Prototype) : CVIBuffer{Prototype}
{
}

HRESULT CVIBuffer_Point::Initialize_Prototype()
{
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_iNumVertices = 1;
    m_iIndexStride = sizeof(_ushort); // 4바이트로 할꺼면 _uint
    m_iNumIndexices = 1;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT; // 4바이트로 할꺼면 DXGI_FORMAT_R32_UINT
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

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

    pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);
    pVertices[0].vTexcoord = _float2(0.0f, 0.f);
    // pSysMem : 정점 버퍼를 초기화할 자료를 담은 시스템 메모리 배열을 가리키는 포인터.
    m_InitialDesc.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

#pragma endregion
    Safe_Delete_Array(pVertices);
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

    m_InitialDesc.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;
#pragma endregion


    Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT CVIBuffer_Point::Initialize(void* pArg)
{
    return S_OK;
}

CVIBuffer_Point* CVIBuffer_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_Point* pInstance = new CVIBuffer_Point(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Point::Clone(void* pArg)
{
    CVIBuffer_Point* pInstance = new CVIBuffer_Point(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Point::Free()
{
    __super::Free();
}
