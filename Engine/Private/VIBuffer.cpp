#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
    : CComponent{Prototype.m_pDevice, Prototype.m_pContext}, m_pVB{Prototype.m_pVB}, m_pIB{Prototype.m_pIB},
      m_iNumVertexBuffers{Prototype.m_iNumVertexBuffers}, m_iVertexStride{Prototype.m_iVertexStride},
      m_iNumVertices{Prototype.m_iNumVertices}, m_iIndexStride{Prototype.m_iIndexStride},
      m_iNumIndexices{Prototype.m_iNumIndexices}, m_eIndexFormat{Prototype.m_eIndexFormat},
      m_ePrimitiveTopology{Prototype.m_ePrimitiveTopology}
{
    Safe_AddRef(m_pIB);
    Safe_AddRef(m_pVB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer::Render()
{
    if (nullptr == m_pContext)
        return E_FAIL;
    /*인덱스 버퍼를이용해서 기본도형을그릴 때에는 쓰는 매서드*/
    /*인덱스 버퍼를 이용하지 않는 다면 Draw함수를 이용*/
    m_pContext->DrawIndexed(m_iNumIndexices, 0, 0);
    /*DrawIndexed (사용할 인덱스 개수, 사용할 인덱스 첫위치,정점들을 가져오기 전에，이 그리기 호출에서 사용할 인덱스들에
     * 더해지는 정수 값. )*/
    return S_OK;
}

HRESULT CVIBuffer::Bind_Buffers()
{
    if (nullptr == m_pContext)
        return E_FAIL;

    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB,
    };

    _uint iVertexStrides[] = {
        m_iVertexStride,
    };

    _uint iOffsets[] = {
        0,
    };

    /* 정점버퍼들을 장치에 바인딩한다. */
    /* 복수의 정점버퍼를 동시에 장치에 바인딩하는 것이 가능하다 .*/
    /*즉, 생성된 버퍼의 정점들을 실제로 파이프라인에 공급하려면 버퍼를 장치의 한 입력 슬롯에 묶어야 한다.
     * IASetVertexBuffers 함수는 이를 위한 메서드*/
    /*IASetVertexBuffers(정점 버퍼들을 붙이기 시작할 입력 슬로의 색인,
                                             입력 슬롯에 붙이고자하는 버퍼 개수,
                                             Strides(보폭)들의 배열의 첫원소를 가리키는 포인터,
                                             오프셋들의 배열의 첫 원소를 가리키는 포인터.)
                                             오프셋이란 정점 버퍼의 시작 위치에서 입력 조립기 단계가 정점 자료를 읽기
       시작할 정점 버퍼 안 위치까지의 거리 */
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);

    /* 인덱스 버퍼를 장치에 바인딩한다. */
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    /*IASetIndexBuffer(버퍼 변수, 인덱스의 형식, 바이트 단위 오프셋 - 입력 조립기가 인덱스를 읽기 시작할 위치 )*/

    /*기본 형식 및 입력 어셈블러 단계의 입력 데이터를 설명하는 데이터 순서에 대한 정보를 바인딩.
     정점 버퍼는 정점들읠 연속적인 메모리에 저장하는 자료 구조일뿐 어떤 식으로 조합해서 기본 도형을 형성할지는 정해주지
    않음. 따라서 기본도형을 형성하는 방식을 DireCt3D에게 알려 주는 데 쓰이는 수단이 바로 기본도형 위상구조(primitive
    topology)*/
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppOut)
{
    return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialDesc, ppOut);
    return S_OK;
}

void CVIBuffer::Free()
{
    __super::Free();

    Safe_Release(m_pIB);
    Safe_Release(m_pVB);
}
