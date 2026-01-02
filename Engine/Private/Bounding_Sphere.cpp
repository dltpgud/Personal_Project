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

	m_iCurRadius = m_pBoundDesc->Radius;
	m_iCurCenter = m_pBoundDesc->Center;
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

_bool CBounding_Sphere::IsInside(const _float3& pos)
{
    return m_pBoundDesc->Contains(XMLoadFloat3(&pos)) == DirectX::ContainmentType::CONTAINS;
}

AABB CBounding_Sphere::Get_WorldAABB() const
{
    AABB worldAABB;

    // 구의 중심에서 반지름만큼 떨어진 최소값과 최대값 계산
    worldAABB.min = {m_pBoundDesc->Center.x - m_pBoundDesc->Radius, m_pBoundDesc->Center.y - m_pBoundDesc->Radius,
                     m_pBoundDesc->Center.z - m_pBoundDesc->Radius};

    worldAABB.max = {m_pBoundDesc->Center.x + m_pBoundDesc->Radius, m_pBoundDesc->Center.y + m_pBoundDesc->Radius,
                     m_pBoundDesc->Center.z + m_pBoundDesc->Radius};
    return worldAABB;
}

_bool CBounding_Sphere::SweepTOI(_vector startPosWS, _vector endPosWS, CCollider* Taget, OUT TOIResult& out)
{

	if (!Taget)
        return false;

    // 내가 스피어일 때만 우선 구현 (총알/스킬 CCD 1단계)

        float r = Get_iCurRadius();

        // 타겟이 AABB/OBB면 월드AABB로 근사(1단계)
        AABB taabb = Taget->Get_WorldAABB();

        return SweepTOI_SphereVsAABB(startPosWS, endPosWS, taabb, r, out);
    

    return false;
}

AABB CBounding_Sphere::InflateAABB(const AABB& b, float r)
{
    AABB o = b;
    o.min.x -= r;
    o.min.y -= r;
    o.min.z -= r;
    o.max.x += r;
    o.max.y += r;
    o.max.z += r;
    return o;
}

_bool CBounding_Sphere::RayAABB_EnterTime(_vector o, _vector dN, const AABB& b, _float tMax, _float& outEnter,
                                          _vector* outNormal)
{
    _float3 ro, rd;
    XMStoreFloat3(&ro, o);
    XMStoreFloat3(&rd, dN);

    float tmin = 0.0f;
    float tmax = tMax;

    _vector nrm = XMVectorZero();

    auto slab = [&](float pos, float dir, float minv, float maxv, _vector axisNeg, _vector axisPos) -> bool
    {
        const float eps = 1e-8f;
        if (fabsf(dir) < eps)
        {
            return (pos >= minv && pos <= maxv);
        }

        float invD = 1.0f / dir;
        float t0 = (minv - pos) * invD;
        float t1 = (maxv - pos) * invD;

        _vector candN = (t0 > t1) ? axisPos : axisNeg; // 들어오는 면의 법선
        if (t0 > t1)
            std::swap(t0, t1);

        if (t0 > tmin)
        {
            tmin = t0;
            nrm = candN;
        }
        tmax = min(tmax, t1);
        return tmin <= tmax;
    };

    if (!slab(ro.x, rd.x, b.min.x, b.max.x, XMVectorSet(-1, 0, 0, 0), XMVectorSet(1, 0, 0, 0)))
        return false;
    if (!slab(ro.y, rd.y, b.min.y, b.max.y, XMVectorSet(0, -1, 0, 0), XMVectorSet(0, 1, 0, 0)))
        return false;
    if (!slab(ro.z, rd.z, b.min.z, b.max.z, XMVectorSet(0, 0, -1, 0), XMVectorSet(0, 0, 1, 0)))
        return false;

    outEnter = tmin;
    if (outNormal)
        *outNormal = nrm;
    return true;
}

_bool CBounding_Sphere::SweepTOI_SphereVsAABB(_vector start, _vector end, const AABB& targetAABB, _float radius,
                                              OUT TOIResult& out)
{
    out = TOIResult{};

    _vector delta = end - start;
    float lenSq = XMVectorGetX(XMVector3LengthSq(delta));
    if (!(lenSq > 1e-12f) || !std::isfinite(lenSq))
        return false;

    float len = sqrtf(lenSq);
    _vector dir = delta / len; // 정규화
    float tEnter = 0.f;
    _vector hitN = XMVectorZero();

    AABB inflated = InflateAABB(targetAABB, radius);

    // len이 곧 tMax(거리 단위)
    if (!RayAABB_EnterTime(start, dir, inflated, len, tEnter, &hitN))
        return false;

    if (tEnter < 0.f || tEnter > len)
        return false;

    out.hit = true;
    out.distance = tEnter;
    out.toi = tEnter / len; // 0~1
    out.normal = hitN;

    // 구 중심 위치(충돌 시점)
    _vector centerAtHit = start + dir * tEnter;

    // 표면 접점은 중심에서 법선방향으로 radius만큼 이동
    out.position = centerAtHit - hitN * radius;
    return true;
}

_bool CBounding_Sphere::RayIntersect(_vector RayPos, _vector RayDir, _float& fDis, _vector* hitPos)
{
	_bool		isColl = { false };
	

	isColl = m_pBoundDesc->Intersects(RayPos, RayDir,fDis);

    if (true == isColl && nullptr != hitPos)
	{
       *hitPos = RayPos + RayDir * fDis;
	}

	return isColl;
}

_float CBounding_Sphere::Get_iCurRadius()
{
	return m_iCurRadius;
}

_float3 CBounding_Sphere::Get_iCurCenter()
{
	return m_iCurCenter;
}

 void CBounding_Sphere::Set_Info(BOUND_DESC* pBoundDesc)
{
     BOUND_SPHERE_DESC* pDesc = static_cast<BOUND_SPHERE_DESC*>(pBoundDesc);
     m_pBoundDesc_Original->Center = pDesc->vCenter;
     m_pBoundDesc_Original->Radius = pDesc->fRadius;
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
