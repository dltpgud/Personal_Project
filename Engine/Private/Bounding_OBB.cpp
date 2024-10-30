#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

CBounding_OBB::CBounding_OBB()
{
}

HRESULT CBounding_OBB::Initialize(const BOUND_DESC* pBoundDesc)
{
	const BOUND_OBB_DESC*	pDesc = static_cast<const BOUND_OBB_DESC*>(pBoundDesc);

	/* 사본콜라이더로서 실 사용하고자하는 컴포넌트로 생성이 될때 호출 된다. */
	_float4		vRotation = {};

	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));
	
	m_pBoundDesc_Original = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pBoundDesc = new BoundingOrientedBox(*m_pBoundDesc_Original);

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
	
	m_pBoundDesc_Original->Transform(*m_pBoundDesc, WorldMatrix);
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eType, CBounding* pTargetBounding)
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

_bool CBounding_OBB::RayIntersect(_vector RayPos, _vector RayDir, _float& fDis)
{
	_bool		isColl = { false };


	isColl = m_pBoundDesc->Intersects(RayPos, RayDir,fDis);

	return isColl;
}

#ifdef _DEBUG

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{

	DX::Draw(pBatch, *m_pBoundDesc, vColor);

	return S_OK;
}

#endif

CBounding_OBB * CBounding_OBB::Create(const BOUND_DESC * pBoundDesc)
{
	CBounding_OBB*		pInstance = new CBounding_OBB();

	if (FAILED(pInstance->Initialize(pBoundDesc)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundDesc_Original);
	Safe_Delete(m_pBoundDesc);


}
