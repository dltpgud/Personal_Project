#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"

CBounding_Sphere::CBounding_Sphere()
{
}

HRESULT CBounding_Sphere::Initialize(const BOUND_DESC* pBoundDesc)
{
	const BOUND_SPHERE_DESC*	pDesc = static_cast<const BOUND_SPHERE_DESC*>(pBoundDesc);

	/* 사본콜라이더로서 실 사용하고자하는 컴포넌트로 생성이 될때 호출 된다. */
	m_pBoundDesc_Original = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pBoundDesc = new BoundingSphere(*m_pBoundDesc_Original);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{

	m_pBoundDesc_Original->Transform(*m_pBoundDesc, WorldMatrix);
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE eType, CBounding* pTargetBounding)
{
	_bool		isColl = { false };

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pBoundDesc->Intersects(*static_cast<CBounding_AABB*>(pTargetBounding)->Get_Desc());
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pBoundDesc->Intersects(*static_cast<CBounding_OBB*>(pTargetBounding)->Get_Desc());
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pBoundDesc->Intersects(*static_cast<CBounding_Sphere*>(pTargetBounding)->Get_Desc());
		break;
	}
	return isColl;
}

_bool CBounding_Sphere::RayIntersect(_vector RayPos, _vector RayDir, _float& fDis)
{
	_bool		isColl = { false };

	_float Dist{};
	isColl = m_pBoundDesc->Intersects(RayPos, RayDir, Dist);
	fDis = Dist;
	return isColl;
}

#ifdef _DEBUG

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{

	DX::Draw(pBatch, *m_pBoundDesc, vColor);

	return S_OK;
}

#endif

CBounding_Sphere * CBounding_Sphere::Create(const BOUND_DESC * pBoundDesc)
{
	CBounding_Sphere*		pInstance = new CBounding_Sphere();

	if (FAILED(pInstance->Initialize(pBoundDesc)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundDesc_Original);
	Safe_Delete(m_pBoundDesc);


}
