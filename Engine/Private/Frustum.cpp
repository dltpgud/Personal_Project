#include "..\Public\Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	/* 투영스페이스 상의 절두체의 여덟개 모서리 점을 구했다. */
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	/* 월드 스페이스로 이전시키자. */
	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), m_pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ)));
		XMStoreFloat3(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPoints[i]), m_pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW)));
	}

	Make_Planes(m_vWorldPoints, m_WorldPlanes);
}


_bool CFrustum::isIn_WorldSpace(_fvector vTargetPos, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		/*ax + by + cz + d = ??
		a b c d
		x y z 1*/

		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlanes[i]), vTargetPos)))
			return false;	
	}

	return true;
}

_bool CFrustum::isIn_LocalSpace(_fvector vTargetPos, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		/*ax + by + cz + d = ??
		a b c d
		x y z 1*/

		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_LocalPlanes[i]), vTargetPos)))
			return false;
	}

	return true;
}

void CFrustum::Transform_To_LocalSpace(_fmatrix WorldMatrixInv)
{
	_float3			vLocalPoints[8] = {};
	
	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&vLocalPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPoints[i]), WorldMatrixInv));
	}

	// XMPlaneTransform();

	Make_Planes(vLocalPoints, m_LocalPlanes);
}

HRESULT CFrustum::Make_Planes(const _float3 * pPoints, _float4 * pPlanes)
{
	/* +x */
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	/* -x */
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));
	/* +y */
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[4])));
	/* -y */
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6])));
	/* +z */
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));
	/* -z */
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));

	return S_OK;
}

CFrustum * CFrustum::Create()
{
	CFrustum*		pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}

