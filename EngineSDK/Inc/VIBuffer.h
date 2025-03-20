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
 
    /*D3D11_BUFFER_DESC : 생성할 버퍼를 서술하는 구조체
      D3D11_SUBRESOURCE_DATA : 버퍼 초가화에 사용할 자료를 서술하는 구조체
      이 두 구조체를 채운 뒤, 다바이스에서 버퍼생성함 수를 호출하면 정점 버퍼가 생성됨

      D3D_PRIMITIVE_TOPOLOGY : 파이프라인이 입력 어셈블러 단계에 바인딩된 정점 데이터를 해석하는 방식입니다.
                                                       이러한 기본 토폴로지 값은 정점 데이터가 화면에 렌더링되는 방식을
      결정합니다.
    */
protected:
    HRESULT Create_Buffer(ID3D11Buffer** ppOut);

public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;
};
END
