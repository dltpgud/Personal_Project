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

_bool CBounding_OBB::RayIntersect(_vector RayPos, _vector RayDir, _float& fDis, _vector* hitPos)
{
	_bool		isColl = { false };

    isColl = m_pBoundDesc->Intersects(RayPos, RayDir, fDis);
	
	if (true == isColl && nullptr != hitPos)
	{
       *hitPos = RayPos + RayDir * fDis;
	}

	return isColl;
}

_float CBounding_OBB::Get_iCurRadius()
{
    const _float ex = m_pBoundDesc->Extents.x;
    const _float ey = m_pBoundDesc->Extents.y;
    const _float ez = m_pBoundDesc->Extents.z;
    return sqrtf(ex * ex + ey * ey + ez * ez);
}

_float3 CBounding_OBB::Get_iCurCenter()
{
    return m_pBoundDesc->Center;
}
_bool CBounding_OBB::IsInside(const _float3& pos)
{
    return m_pBoundDesc->Contains(XMLoadFloat3(&pos)) == DirectX::ContainmentType::CONTAINS;
}

AABB CBounding_OBB::Get_WorldAABB() const
{
    AABB worldAABB;
    _float3 corners[8];
    m_pBoundDesc->GetCorners(corners); //  회전 포함된 OBB의 월드 코너 8개

    _float3 minP = corners[0];
    _float3 maxP = corners[0];

    for (int i = 1; i < 8; ++i)
    {
        minP.x = min(minP.x, corners[i].x);
        minP.y = min(minP.y, corners[i].y);
        minP.z = min(minP.z, corners[i].z);

        maxP.x = max(maxP.x, corners[i].x);
        maxP.y = max(maxP.y, corners[i].y);
        maxP.z = max(maxP.z, corners[i].z);
    }

    worldAABB.min = {minP.x, minP.y, minP.z};
    worldAABB.max = {maxP.x, maxP.y, maxP.z};
    return worldAABB;
}

void CBounding_OBB::Set_Info(BOUND_DESC* pBoundDesc)
{
  BOUND_OBB_DESC* pDesc = static_cast<BOUND_OBB_DESC*>(pBoundDesc);
    m_pBoundDesc_Original->Center = pDesc->vCenter;
    m_pBoundDesc_Original->Extents = pDesc->vExtents;

  	_float4 vRotation = {};

  XMStoreFloat4(&vRotation,XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));
    m_pBoundDesc_Original->Orientation = vRotation;
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
