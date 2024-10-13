#include "VIBuffer_Cell.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell & Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_Cell::Initialize_Prototype(const _float3* pPoints)
{
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumVertices = 3;
	m_iIndexStride = sizeof(_ushort);
	m_iNumIndexices = 4;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;   //D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
#pragma region VERTEX_BUFFER
	/* dx9 : 정점버퍼를 할당하고 -> 락언락해서 정점버퍼에 초기값을 채운다. */
	/* dx9 : 정점버퍼에 초기값을 채우면서 정점버퍼를 할당한다*/
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 할당하고자하는 메모리공간의 크기(Byte)*/
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;

	/* 버퍼의 속성 (정적, 동적) */
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
	VTXPOS*		pVertices = new VTXPOS[m_iNumVertices];
	memcpy(pVertices, pPoints, sizeof(VTXPOS) * 3);

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
	_ushort*			pIndices = new _ushort[m_iNumIndexices];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;	

	m_InitialDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(void * pArg)
{
	return S_OK;
}

CVIBuffer_Cell * CVIBuffer_Cell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3* pPoints)
{
	CVIBuffer_Cell*		pInstance = new CVIBuffer_Cell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pPoints)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Cell::Clone(void * pArg)
{
	CVIBuffer_Cell*		pInstance = new CVIBuffer_Cell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_Cell::Free()
{
	__super::Free();


}
