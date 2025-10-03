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
    m_iMaxInstances = 10000;
    m_iVertexStride = sizeof(VTXDECAL);
	m_iNumVertices = 8;
    m_iIndexStride = sizeof(_uint);
    m_iNumIndexPerInstance = 36;
    m_iNumIndexices = m_iNumIndexPerInstance * m_iMaxInstances;
	m_iNumVertexBuffers = 2;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
        VTXDECAL* pVertices = new VTXDECAL[m_iNumVertices];

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = pVertices[0].vPosition;

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = pVertices[1].vPosition;

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = pVertices[2].vPosition;

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = pVertices[3].vPosition;

	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = pVertices[4].vPosition;

	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = pVertices[5].vPosition;

	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = pVertices[6].vPosition;

	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = pVertices[7].vPosition;

	m_InitialDesc.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	
   ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

        m_iNumIndexPerInstance = 36;
        m_iNumIndexices = m_iNumIndexPerInstance; // 인덱스는 큐브 1개 기준 36개만 필요

        // 큐브 인덱스
        _uint cubeIndices[36] = {
            1, 5, 6, 1, 6, 2, // +X
            4, 0, 3, 4, 3, 7, // -X
            4, 5, 1, 4, 1, 0, // +Y
            3, 2, 6, 3, 6, 7, // -Y
            5, 4, 7, 5, 7, 6, // +Z
            0, 1, 2, 0, 2, 3  // -Z
        };

     m_BufferDesc.ByteWidth = sizeof(_uint) * m_iNumIndexices; // 36개짜리
        m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
        m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        m_BufferDesc.CPUAccessFlags = 0;
        m_BufferDesc.MiscFlags = 0;
        m_BufferDesc.StructureByteStride = 0;

        m_InitialDesc.pSysMem = cubeIndices;

        if (FAILED(__super::Create_Buffer(&m_pIB)))
            return E_FAIL;

#pragma endregion

#pragma region INSTANCE_BUFFER
        ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
        m_iInstanceVertexStride = sizeof(DecalInstanceData);
        m_InstanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        m_InstanceBufferDesc.CPUAccessFlags = 0;
        m_InstanceBufferDesc.MiscFlags = 0;
        m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
        m_InstanceBufferDesc.ByteWidth = m_iInstanceVertexStride * m_iMaxInstances;

        ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);

        if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance)))
            return E_FAIL;
#pragma endregion


        m_InstanceUpload.resize(m_iMaxInstances);

   
	Safe_Delete_Array(pVertices);
	//Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_DecalCube::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_DecalCube::Bind_Buffers()
{
    UINT strides[2] = {m_iVertexStride, m_iInstanceVertexStride};
    UINT offsets[2] = {0, 0};
    ID3D11Buffer* bufs[2] = {m_pVB, m_pVBInstance};
    m_pContext->IASetVertexBuffers(0, 2, bufs, strides, offsets);

    // IB 바인딩 (포맷을 위에서 고친 값으로!)
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

    // 드로우
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
    return S_OK;
}

HRESULT CVIBuffer_DecalCube::Render()
{

    m_pContext->DrawIndexedInstanced(36, m_iNumInstance, 0, 0, 0);

    return S_OK;
}

void CVIBuffer_DecalCube::Update(const vector <DecalInstanceData>& m_InstanceData)
{
 
    m_iNumInstance = (_uint)m_InstanceData.size();
    if (m_iNumInstance > m_iMaxInstances)
        m_iNumInstance = m_iMaxInstances;

    if (m_iNumInstance == 0)
    {
        // 필요 없으면 업로드/드로우 생략
        return;
    }

    // 업로드용 CPU 버퍼의 앞부분에 실제 개수만 복사
    memcpy(m_InstanceUpload.data(), m_InstanceData.data(), size_t(m_iNumInstance) * sizeof(DecalInstanceData));
    m_pContext->UpdateSubresource(m_pVBInstance, 0, nullptr, m_InstanceUpload.data(), 0, 0);
   // D3D11_MAPPED_SUBRESOURCE mapped{};
   // if (SUCCEEDED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
   // {
   //     memcpy(mapped.pData, m_InstanceData.data(), sizeof(DecalInstanceData) * m_iNumInstance);
   //     m_pContext->Unmap(m_pVBInstance, 0);
   // }
}

CVIBuffer_DecalCube* CVIBuffer_DecalCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_DecalCube* pInstance = new CVIBuffer_DecalCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_DecalCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_DecalCube::Clone(void* pArg)
{
    CVIBuffer_DecalCube* pInstance = new CVIBuffer_DecalCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_DecalCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_DecalCube::Free()
{
	__super::Free();


}
