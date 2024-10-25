#include "Cell.h"

#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _uint iIndex, const _uint& Type)
{
	m_iIndex = iIndex;
	m_Type = static_cast<TYPE> (Type);
	for (size_t i = 0; i < POINT_END; i++)	
		m_vPoints[i] = pPoints[i];


#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::isIn(_fvector vAfterLocalPos, _fvector vBeforeLocalPos, _int* pNeighborIndex ,_vector* Slide )
{

	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % POINT_END]) - XMLoadFloat3(&m_vPoints[i]);  // 점과 점의 방향벡터(라인)을 그린다. 앞에 나누기 연산을 한건 없는 4번째 점을 찾을까봐..
		_vector		vNormal = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);// 선들의 법선을 구한다.
		_vector		vDir = vAfterLocalPos - XMLoadFloat3(&m_vPoints[i]);

		if ( 0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vNormal), XMVector3Normalize(vDir))))
		{
			*pNeighborIndex = m_iNeighbors[i];

			_vector MoveDir = vAfterLocalPos - vBeforeLocalPos;

			_vector vReflectDir = XMVector3Dot(XMVector3Normalize(vNormal) * -1, MoveDir);

			_float vReflectLength = XMVectorGetX(vReflectDir);
		 	if (vReflectLength < 0.f)
		 		vReflectLength *= -1.f;

		     _vector Slid{};
		 	 Slid = MoveDir - vReflectLength * XMVector3Normalize(vNormal);

		 	 *Slide = Slid;

			return false;
		}
	}
	
	return true;
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
	 // XMVector3Equal : bool값 반환
	// XMVectorEqual : 벡터의 각 요소들을 비교해서 0,1로 표현해 반환
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSour)) {

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDest))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDest))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDest))
			return true;
	}

	return false;
}
#ifdef _DEBUG
HRESULT CCell::Render()
{

#ifdef _DEBUG
	m_pVIBuffer->Bind_Buffers();

	return m_pVIBuffer->Render();
#endif
}
#endif
CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _uint iIndex, const _uint& Type )
{
	CCell*		pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex, Type)))
	{
		MSG_BOX("Failed To Created : CCell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
