#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_AABB::CBounding_AABB()
{
}

HRESULT CBounding_AABB::Initialize(const BOUND_DESC* pBoundDesc)
{
	const BOUND_AABB_DESC* pDesc = static_cast<const BOUND_AABB_DESC*>(pBoundDesc);

	/* 사본콜라이더로서 실 사용하고자하는 컴포넌트로 생성이 될때 호출 된다. */
	m_pBoundDesc_Original = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pBoundDesc = new BoundingBox(*m_pBoundDesc_Original);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix = WorldMatrix;  // 바운딩박스가 객체가 돌면 같이 회전하는데.. 강제로 잡아줌

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]);

	m_pBoundDesc_Original->Transform(*m_pBoundDesc, TransformMatrix);
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eType, CBounding* pTargetBounding)
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

_bool CBounding_AABB::RayIntersect(_vector RayPos, _vector RayDir, _float& fDis)
{
	_bool		isColl = { false };

	
	isColl = m_pBoundDesc->Intersects(RayPos, RayDir, fDis);
 

	return isColl;
}


#ifdef _DEBUG

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{

	DX::Draw(pBatch, *m_pBoundDesc, vColor);

	return S_OK;
}

#endif

CBounding_AABB* CBounding_AABB::Create(const BOUND_DESC* pBoundDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB();

	if (FAILED(pInstance->Initialize(pBoundDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundDesc_Original);
	Safe_Delete(m_pBoundDesc);


}
