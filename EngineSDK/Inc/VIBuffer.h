#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer : public CComponent
{
protected:
    CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer(const CVIBuffer& Prototype);
    virtual ~CVIBuffer() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual HRESULT Render();

public:
    /* 그리기 위해 필요한 값들을 장치에 올린다. */
    virtual  HRESULT Bind_Buffers();

protected:
    ID3D11Buffer* m_pVB = {nullptr}; // 버텍스 버퍼 변수
    ID3D11Buffer* m_pIB = {nullptr}; // 인덱스 버퍼 변수

    /* 정점 or 인덱스버퍼를 할당하기위한 기초 데이터들. */
    D3D11_BUFFER_DESC m_BufferDesc = {};
    D3D11_SUBRESOURCE_DATA m_InitialDesc = {};
    _uint m_iNumVertexBuffers = {};
    _uint m_iVertexStride = {}; /* 정점 하나의 크기. */
    _uint m_iNumVertices = {};
    _uint m_iIndexStride = {};
    _uint m_iNumIndexices = {};
    _float3* m_pVertexPositions = {nullptr};
    DXGI_FORMAT m_eIndexFormat = {};
    D3D_PRIMITIVE_TOPOLOGY m_ePrimitiveTopology = {};
 
protected:
    HRESULT Create_Buffer(ID3D11Buffer** ppOut);

public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;
};
END
