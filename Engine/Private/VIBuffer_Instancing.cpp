#include "VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing & Prototype)
	: CVIBuffer{ Prototype }
	, m_pVBInstance { Prototype.m_pVBInstance }
	, m_iNumInstance { Prototype.m_iNumInstance }
	, m_iInstanceVertexStride { Prototype.m_iInstanceVertexStride }
	, m_InstanceBufferDesc { Prototype.m_InstanceBufferDesc }
	, m_InstanceInitialDesc { Prototype.m_InstanceInitialDesc }
	, m_iNumIndexPerInstance { Prototype.m_iNumIndexPerInstance }
	, m_pInstanceVertices{ Prototype.m_pInstanceVertices }
	, m_pSpeed { Prototype.m_pSpeed }
	, m_vPivot{ Prototype.m_vPivot }
	, m_isLoop { Prototype.m_isLoop }
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void * pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance, 
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride
	};

	_uint				iOffsets[] = {
		0,
		0, 
	};


	/* �������۵��� ��ġ�� ���ε��Ѵ�. */
	/* ������ �������۸� ���ÿ� ��ġ�� ���ε��ϴ� ���� �����ϴ� .*/
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);

	/* �ε��� ���۸� ��ġ�� ���ε��Ѵ�. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

void CVIBuffer_Instancing::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX*			pVertices = static_cast<VTXMATRIX*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector				vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation, 
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop &&
			pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vLifeTime.y = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instancing::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMATRIX*			pVertices = static_cast<VTXMATRIX*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vTranslation.y -= m_pSpeed[i] * fTimeDelta;
		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && 
			pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vLifeTime.y = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;									
		}		
	}	

	m_pContext->Unmap(m_pVBInstance, 0);	
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	if (false == m_bClone)
	{
		Safe_Delete_Array(m_pSpeed);
		Safe_Delete_Array(m_pInstanceVertices);
	}

	Safe_Release(m_pVBInstance);

}
