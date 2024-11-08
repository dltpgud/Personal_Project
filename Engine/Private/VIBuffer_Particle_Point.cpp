#include "..\Public\VIBuffer_Particle_Point.h"
#include "GameInstance.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point & Prototype)
	: CVIBuffer_Instancing{ Prototype }	
{

}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCING_DESC* pDesc)
{
	m_vPivot = pDesc->vPivot;
	m_isLoop = pDesc->isLoop;
	m_iNumInstance = pDesc->iNumInstance;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_iIndexStride = sizeof(_ushort);
	m_iNumIndexPerInstance = 1;
	m_iNumIndexices = m_iNumIndexPerInstance * m_iNumInstance;
	m_iNumVertexBuffers = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;

	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
	VTXPOINT*		pVertices = new VTXPOINT[m_iNumVertices];

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices[0].vPSize = _float2(1.0f, 1.f);

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
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndexices);

	m_InitialDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion
	
#pragma region INSTANCE_BUFFER
	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);

	m_iInstanceVertexStride = sizeof(VTXMATRIX);

	/* 할당하고자하는 메모리공간의 크기(Byte)*/
	m_InstanceBufferDesc.ByteWidth = m_iInstanceVertexStride * m_iNumInstance;

	/* 버퍼의 속성 (정적, 동적) */
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	m_pSpeed = new _float[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float		fScale = m_pGameInstance->Compute_Random(pDesc->vSize.x, pDesc->vSize.y);
		m_pSpeed[i] = m_pGameInstance->Compute_Random(pDesc->vSpeed.x, pDesc->vSpeed.y);

		m_pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);

		m_pInstanceVertices[i].vTranslation = _float4(m_pGameInstance->Compute_Random(pDesc->vCenter.x - pDesc->vRange.x * 0.5f, pDesc->vCenter.x + pDesc->vRange.x * 0.5f), 
			m_pGameInstance->Compute_Random(pDesc->vCenter.y - pDesc->vRange.y * 0.5f, pDesc->vCenter.y + pDesc->vRange.y * 0.5f), 
			m_pGameInstance->Compute_Random(pDesc->vCenter.z - pDesc->vRange.z * 0.5f, pDesc->vCenter.z + pDesc->vRange.z * 0.5f), 
			1.f);
		m_pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Compute_Random(pDesc->vLifeTime.x, pDesc->vLifeTime.y), 0.f);		
	}

	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion


	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Particle_Point * CVIBuffer_Particle_Point::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CVIBuffer_Instancing::INSTANCING_DESC* pDesc)
{
	CVIBuffer_Particle_Point*		pInstance = new CVIBuffer_Particle_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Particle_Point::Clone(void * pArg)
{
	CVIBuffer_Particle_Point*		pInstance = new CVIBuffer_Particle_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_Particle_Point::Free()
{
	__super::Free();

	
}
