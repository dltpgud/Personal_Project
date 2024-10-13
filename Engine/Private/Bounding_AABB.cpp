#include "Bounding_AABB.h"



CBounding_AABB::CBounding_AABB()
{
}

HRESULT CBounding_AABB::Initialize(void* pArg)
{
	BOUND_AABB_DESC*	pDesc = static_cast<BOUND_AABB_DESC*>(pArg);

	/* �纻�ݶ��̴��μ� �� ����ϰ����ϴ� ������Ʈ�� ������ �ɶ� ȣ�� �ȴ�. */
	m_pBoundDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);

	return S_OK;
}

void CBounding_AABB::Free()
{
	__super::Free();
}
