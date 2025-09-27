#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
    : CComponent{ Prototype}, m_pVB{Prototype.m_pVB}, m_pIB{Prototype.m_pIB},
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

    m_pContext->DrawIndexed(m_iNumIndexices, 0, 0);
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

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);

    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppOut)
{
    //return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialDesc, ppOut);
    const D3D11_SUBRESOURCE_DATA* pInitData = nullptr;

    // 초기 데이터가 있으면 전달
    if (m_InitialDesc.pSysMem)
        pInitData = &m_InitialDesc;

    return m_pDevice->CreateBuffer(&m_BufferDesc, pInitData, ppOut);
}

void CVIBuffer::Free()
{
    __super::Free();

        Safe_Delete_Array(m_pVertexPositions);


    Safe_Release(m_pVB); 
    Safe_Release(m_pIB);
}
