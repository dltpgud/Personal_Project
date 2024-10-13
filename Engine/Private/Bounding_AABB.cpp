#include "Bounding_AABB.h"



CBounding_AABB::CBounding_AABB()
{
}

HRESULT CBounding_AABB::Initialize(void* pArg)
{
	BOUND_AABB_DESC*	pDesc = static_cast<BOUND_AABB_DESC*>(pArg);

	/* 사본콜라이더로서 실 사용하고자하는 컴포넌트로 생성이 될때 호출 된다. */
	m_pBoundDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);

	return S_OK;
}

void CBounding_AABB::Free()
{
	__super::Free();
}
