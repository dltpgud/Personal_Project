#include "..\Public\QuadTree.h"
#include "Collider.h"
#include "GameInstance.h"
#include <algorithm>

template <typename T> inline T Clamp(const T& value, const T& minVal, const T& maxVal)
{
    return (value < minVal) ? minVal : (value > maxVal ? maxVal : value);
}
CQuadTree::CQuadTree() 
{
  
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
    m_iCorners[CORNER_LT] = iLT; // 왼쪽 위
    m_iCorners[CORNER_RT] = iRT; // 오른쪽 위
    m_iCorners[CORNER_RB] = iRB; // 오른쪽 아래
    m_iCorners[CORNER_LB] = iLB; // 왼쪽 아래

    if (1 == m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT]) // 칸의 개수가 1이면 더 이상 분할 안함
        return S_OK;

    m_iCenter = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RB]) >> 1; // 중앙 인덱스

    _uint iLC, iTC, iRC, iBC;
    // 사각형의 중점   
    iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
    iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
    iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
    iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

    // 자식 사각형들을 생성
    m_Children[CORNER_LT] = CQuadTree::Create(m_iCorners[CORNER_LT], iTC, m_iCenter, iLC);
    m_Children[CORNER_RT] = CQuadTree::Create(iTC, m_iCorners[CORNER_RT], iRC, m_iCenter);
    m_Children[CORNER_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[CORNER_RB], iBC);
    m_Children[CORNER_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[CORNER_LB]);

	return S_OK;
}

void CQuadTree::Culling(CGameInstance* pGameInstance, const _float3* pVerticesPos, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrixInv)
{
    //  isDraw() 함수로 카메라와의 거리를 계산해서 LOD를 결정
    if (nullptr == m_Children[CORNER_LT] || true == isDraw(pGameInstance, pVerticesPos, WorldMatrixInv))
    {
        _uint iIndices[4] = {
            m_iCorners[CORNER_LT],
            m_iCorners[CORNER_RT],
            m_iCorners[CORNER_RB],
            m_iCorners[CORNER_LB],
        };

        // 안전 마진을 위한 타일 크기 계산
        _float fTileSize = XMVector3Length(XMLoadFloat3(&pVerticesPos[iIndices[1]]) - XMLoadFloat3(&pVerticesPos[iIndices[0]])).m128_f32[0];
        _float fSafetyMargin = fTileSize * 0.2f; // 20% 안전 마진

        _bool isIn[4] = {
            // 각 코너들이 카메라 프러스텀 안에 있는지 확인 (안전 마진 적용)
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[0]]), 1.f), fSafetyMargin),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[1]]), 1.f), fSafetyMargin),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[2]]), 1.f), fSafetyMargin),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[3]]), 1.f), fSafetyMargin),
        };

        // 프러스텀 컬링 조건을 완화하여 구멍 방지
        // 4개 코너 중 하나라도 프러스텀 안에 있으면 두 삼각형 모두 렌더링
        if (true == isIn[0] || true == isIn[1] || true == isIn[2] || true == isIn[3])
        {
            pIndices[(*pNumIndices)++] = iIndices[0];
            pIndices[(*pNumIndices)++] = iIndices[1];
            pIndices[(*pNumIndices)++] = iIndices[2];
        }

        if (true == isIn[0] || true == isIn[1] || true == isIn[2] || true == isIn[3])
        {
            pIndices[(*pNumIndices)++] = iIndices[0];
            pIndices[(*pNumIndices)++] = iIndices[2];
            pIndices[(*pNumIndices)++] = iIndices[3];
        }

        return;  // 여기서 끝
    }

    // 중앙 점에서 사각형의 대각선 거리, 다시 자식 사각형들을 재귀 호출
    _float fRadius = 
        XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenter]))
            .m128_f32[0];

    // fRadius 반지름 안에 있는 Frustum 안에 있는 terrain 사각형이 있다면 자식들을 호출한다.
    if (true == pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
    {
        for (auto& pChild : m_Children)
        {
            if (nullptr != pChild)
                pChild->Culling(pGameInstance, pVerticesPos, pIndices, pNumIndices, WorldMatrixInv);
        }
    }
}

_bool CQuadTree::isDraw(CGameInstance * pGameInstance, const _float3 * pVerticesPos, _fmatrix WorldMatrixInv)
{
	_vector		vCamPosition = XMLoadFloat4(pGameInstance->Get_CamPosition());

	vCamPosition = XMVector3TransformCoord(vCamPosition, WorldMatrixInv);

	_float		fCamDistance = XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCenter]) - vCamPosition).m128_f32[0];

	_float fWidth = static_cast<_float>(m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT]);

	if (fCamDistance * 0.5f > fWidth) 
		return true;

	return false;
}

_bool CQuadTree::Picking_Ray(const _float3* pVerticesPos, _vector RayPos, _vector RayDir, _float& fBestDist,
                             _float3& vHitPos, _float3& vHitNormal)
{
    // 1️노드 AABB 계산
    XMFLOAT3 minPt = pVerticesPos[m_iCorners[CORNER_LT]];
    XMFLOAT3 maxPt = pVerticesPos[m_iCorners[CORNER_LT]];

    for (int i = 1; i < 4; ++i)
    {
        const auto& v = pVerticesPos[m_iCorners[i]];
        minPt.x = min(minPt.x, v.x);
        minPt.y = min(minPt.y, v.y);
        minPt.z = min(minPt.z, v.z);
        maxPt.x = max(maxPt.x, v.x);
        maxPt.y = max(maxPt.y, v.y);
        maxPt.z = max(maxPt.z, v.z);
    }

    BoundingBox bounds;
    BoundingBox::CreateFromPoints(bounds, XMLoadFloat3(&minPt), XMLoadFloat3(&maxPt));

    float boxDist;
    if (!bounds.Intersects(RayPos, RayDir, boxDist))
        return false;

    // 2️⃣ 리프라면 두 삼각형 검사
    if (m_Children[CORNER_LT] == nullptr)
    {
        _uint idx[4] = {m_iCorners[CORNER_LT], m_iCorners[CORNER_RT], m_iCorners[CORNER_RB], m_iCorners[CORNER_LB]};

        _float dist;
        // 첫 번째 삼각형
        if (TriangleTests::Intersects(RayPos, RayDir, XMLoadFloat3(&pVerticesPos[idx[0]]),
                                      XMLoadFloat3(&pVerticesPos[idx[1]]), XMLoadFloat3(&pVerticesPos[idx[2]]), dist))
        {
            if (dist < fBestDist)
            {
                fBestDist = dist;
                XMStoreFloat3(&vHitPos, RayPos + RayDir * dist);

                _vector e0 = XMLoadFloat3(&pVerticesPos[idx[1]]) - XMLoadFloat3(&pVerticesPos[idx[0]]);
                _vector e1 = XMLoadFloat3(&pVerticesPos[idx[2]]) - XMLoadFloat3(&pVerticesPos[idx[0]]);
                XMStoreFloat3(&vHitNormal, XMVector3Normalize(XMVector3Cross(e0, e1)));
            }
        }

        // 두 번째 삼각형
        if (TriangleTests::Intersects(RayPos, RayDir, XMLoadFloat3(&pVerticesPos[idx[0]]),
                                      XMLoadFloat3(&pVerticesPos[idx[2]]), XMLoadFloat3(&pVerticesPos[idx[3]]), dist))
        {
            if (dist < fBestDist)
            {
                fBestDist = dist;
                XMStoreFloat3(&vHitPos, RayPos + RayDir * dist);

                _vector e0 = XMLoadFloat3(&pVerticesPos[idx[2]]) - XMLoadFloat3(&pVerticesPos[idx[0]]);
                _vector e1 = XMLoadFloat3(&pVerticesPos[idx[3]]) - XMLoadFloat3(&pVerticesPos[idx[0]]);
                XMStoreFloat3(&vHitNormal, XMVector3Normalize(XMVector3Cross(e0, e1)));
            }
        }

        return true;
    }

    for (auto& child : m_Children)
    {
        if (child)
            child->Picking_Ray(pVerticesPos, RayPos, RayDir, fBestDist, vHitPos, vHitNormal);
    }

    return true;
}

_bool CQuadTree::Intersect_Node(const BoundingSphere& sphere, const _float3* pVerticesPos, OUT _float3* pHitPos,
                                OUT _float3* pHitNormal, OUT _float* pBestPenetration)
{
    // 1️⃣ 현재 노드의 AABB 계산
    _float3 minPt = pVerticesPos[m_iCorners[CORNER_LT]];
    _float3 maxPt = pVerticesPos[m_iCorners[CORNER_LT]];

    for (int i = 1; i < 4; ++i)
    {
        const auto& v = pVerticesPos[m_iCorners[i]];
        minPt.x = min(minPt.x, v.x);
        minPt.y = min(minPt.y, v.y);
        minPt.z = min(minPt.z, v.z);
        maxPt.x = max(maxPt.x, v.x);
        maxPt.y = max(maxPt.y, v.y);
        maxPt.z = max(maxPt.z, v.z);
    }

    BoundingBox nodeBox;
    BoundingBox::CreateFromPoints(nodeBox, XMLoadFloat3(&minPt), XMLoadFloat3(&maxPt));

    // 1차 교차 완화
    BoundingBox padded = nodeBox;
    padded.Extents.x += sphere.Radius;
    padded.Extents.y += sphere.Radius;
    padded.Extents.z += sphere.Radius;

    if (!padded.Intersects(sphere))
        return false;

    // 2️⃣ 리프면 삼각형 검사
    if (m_Children[CORNER_LT] == nullptr)
    {
        const _uint idx[4] = {m_iCorners[CORNER_LT], m_iCorners[CORNER_RT], m_iCorners[CORNER_RB],
                              m_iCorners[CORNER_LB]};

        const _float3& vLT = pVerticesPos[idx[0]];
        const _float3& vRT = pVerticesPos[idx[1]];
        const _float3& vRB = pVerticesPos[idx[2]];
        const _float3& vLB = pVerticesPos[idx[3]];

        _float3 hit{}, normal{};
        _float pen = -FLT_MAX;

        if (TestSphereTriangle(sphere, vLT, vRT, vRB, &hit, &normal, &pen))
        {
            if (pen > *pBestPenetration || *pBestPenetration == -FLT_MAX)
            {
                *pBestPenetration = max(pen, 0.0f);
                *pHitPos = hit;
                *pHitNormal = normal;
            }
        }
        if (TestSphereTriangle(sphere, vLT, vRB, vLB, &hit, &normal, &pen))
        {
            if (pen > *pBestPenetration || *pBestPenetration == -FLT_MAX)
            {
                *pBestPenetration = max(pen, 0.0f);
                *pHitPos = hit;
                *pHitNormal = normal;
            }
        }
        return true;
    }

    // 3️⃣ 자식 재귀 검사
    for (auto& child : m_Children)
    {
        if (child && child->Intersect_Node(sphere, pVerticesPos, pHitPos, pHitNormal, pBestPenetration))
            return true;
    }

    return false;
}

_bool CQuadTree::Intersect_Node(const BoundingBox& box, const _float3* pVerticesPos, OUT _float3* pHitPos, OUT _float3* pHitNormal, OUT _float* pBestPenetration)
{
    // 노드 AABB 계산
    _float3 minPt = pVerticesPos[m_iCorners[CORNER_LT]];
    _float3 maxPt = pVerticesPos[m_iCorners[CORNER_LT]];

    for (int i = 1; i < 4; ++i)
    {
        const auto& v = pVerticesPos[m_iCorners[i]];
        minPt.x = min(minPt.x, v.x);
        minPt.y = min(minPt.y, v.y);
        minPt.z = min(minPt.z, v.z);
        maxPt.x = max(maxPt.x, v.x);
        maxPt.y = max(maxPt.y, v.y);
        maxPt.z = max(maxPt.z, v.z);
    }

    BoundingBox nodeBox;
    BoundingBox::CreateFromPoints(nodeBox, XMLoadFloat3(&minPt), XMLoadFloat3(&maxPt));

    if (!box.Intersects(nodeBox))
        return false;

    if (m_Children[CORNER_LT] == nullptr)
    {
        const _uint idx[4] = {m_iCorners[CORNER_LT], m_iCorners[CORNER_RT], m_iCorners[CORNER_RB],
                              m_iCorners[CORNER_LB]};

        const _float3& vLT = pVerticesPos[idx[0]];
        const _float3& vRT = pVerticesPos[idx[1]];
        const _float3& vRB = pVerticesPos[idx[2]];
        const _float3& vLB = pVerticesPos[idx[3]];

        _float3 hit{}, normal{};
        _float pen = -FLT_MAX;

        if (TestAABBTriangle(box, vLT, vRT, vRB, &hit, &normal, &pen))
        {
            if (pen > *pBestPenetration)
            {
                *pBestPenetration = pen;
                *pHitPos = hit;
                *pHitNormal = normal;
            }
        }
        if (TestAABBTriangle(box, vLT, vRB, vLB, &hit, &normal, &pen))
        {
            if (pen > *pBestPenetration)
            {
                *pBestPenetration = pen;
                *pHitPos = hit;
                *pHitNormal = normal;
            }
        }
        return true;
    }

    _bool anyHit = false;
    for (auto& child : m_Children)
    {
        if (child && child->Intersect_Node(box, pVerticesPos, pHitPos, pHitNormal, pBestPenetration))
            anyHit = true;
    }
    return anyHit;
}

_bool CQuadTree::Intersect_Node(const BoundingOrientedBox& obb, const _float3* pVerticesPos, OUT _float3* pHitPos,
                                OUT _float3* pHitNormal, OUT _float* pBestPenetration)
{
    // 노드 AABB 계산
    _float3 minPt = pVerticesPos[m_iCorners[CORNER_LT]];
    _float3 maxPt = pVerticesPos[m_iCorners[CORNER_LT]];

    for (int i = 1; i < 4; ++i)
    {
        const auto& v = pVerticesPos[m_iCorners[i]];
        minPt.x = min(minPt.x, v.x);
        minPt.y = min(minPt.y, v.y);
        minPt.z = min(minPt.z, v.z);
        maxPt.x = max(maxPt.x, v.x);
        maxPt.y = max(maxPt.y, v.y);
        maxPt.z = max(maxPt.z, v.z);
    }

    BoundingBox nodeBox;
    BoundingBox::CreateFromPoints(nodeBox, XMLoadFloat3(&minPt), XMLoadFloat3(&maxPt));

    if (!obb.Intersects(nodeBox))
        return false;

    if (m_Children[CORNER_LT] == nullptr)
    {
        const _uint idx[4] = {m_iCorners[CORNER_LT], m_iCorners[CORNER_RT], m_iCorners[CORNER_RB],
                              m_iCorners[CORNER_LB]};

        const _float3& vLT = pVerticesPos[idx[0]];
        const _float3& vRT = pVerticesPos[idx[1]];
        const _float3& vRB = pVerticesPos[idx[2]];
        const _float3& vLB = pVerticesPos[idx[3]];

        _float3 hit{}, normal{};
        _float pen = -FLT_MAX;

        if (TestOBBTriangle(obb, vLT, vRT, vRB, &hit, &normal, &pen))
        {
            if (pen > *pBestPenetration)
            {
                *pBestPenetration = pen;
                *pHitPos = hit;
                *pHitNormal = normal;
            }
        }
        if (TestOBBTriangle(obb, vLT, vRB, vLB, &hit, &normal, &pen))
        {
            if (pen > *pBestPenetration)
            {
                *pBestPenetration = pen;
                *pHitPos = hit;
                *pHitNormal = normal;
            }
        }
        return true;
    }

    _bool anyHit = false;
    for (auto& child : m_Children)
    {
        if (child && child->Intersect_Node(obb, pVerticesPos, pHitPos, pHitNormal, pBestPenetration))
            anyHit = true;
    }
    return anyHit;
}



_bool CQuadTree::TestSphereTriangle(const BoundingSphere& sphere, const _float3& a, const _float3& b, const _float3& c,
                                    OUT _float3* oHit, OUT _float3* oNormal, OUT _float* oPen)
{
    _vector pa = XMLoadFloat3(&a);
    _vector pb = XMLoadFloat3(&b);
    _vector pc = XMLoadFloat3(&c);
    _vector center = XMLoadFloat3(&sphere.Center);

    // --- 1️⃣ 삼각형 평면 노멀 ---
    _vector e0 = pb - pa;
    _vector e1 = pc - pa;
    _vector n = XMVector3Normalize(XMVector3Cross(e0, e1));

    // --- 2️⃣ 평면까지 거리 ---
    _float dist = XMVectorGetX(XMVector3Dot(center - pa, n));  /// 노말 방향으로 얼마나떨어 져있는가?
    _vector proj = center - n * dist; // 구 중심을 평면 위로 내린 수직의 발

    // --- 3️⃣ 투영점이 삼각형 내부인지 체크 ---
    _vector c0 = XMVector3Cross(pb - pa, proj - pa);
    _vector c1 = XMVector3Cross(pc - pb, proj - pb);
    _vector c2 = XMVector3Cross(pa - pc, proj - pc);

    _bool inside = (XMVectorGetX(XMVector3Dot(c0, n)) >= 0.f && XMVectorGetX(XMVector3Dot(c1, n)) >= 0.f &&
                   XMVectorGetX(XMVector3Dot(c2, n)) >= 0.f);

    _vector closest;
    if (inside)
    {
        // 평면 안쪽이면 그 투영점을 사용
        closest = proj;
    }
    else
    {
        // --- 4️⃣ 내부가 아니라면 edge/vertex까지 검사 ---
        _vector v0 = pa;
        _vector v1 = pb;
        _vector v2 = pc;

        // 각 에지에 대해 최근접점 구하기
        auto ClosestPointOnSegment = [](_vector p, _vector a, _vector b)
        {
            _vector ab = b - a;
            float t = XMVectorGetX(XMVector3Dot(p - a, ab)) / XMVectorGetX(XMVector3Dot(ab, ab));
            t = Clamp(t, 0.0f, 1.0f);
            return a + ab * t;
        };

        _vector cp0 = ClosestPointOnSegment(center, v0, v1);
        _vector cp1 = ClosestPointOnSegment(center, v1, v2);
        _vector cp2 = ClosestPointOnSegment(center, v2, v0);

        _float d0 = XMVectorGetX(XMVector3LengthSq(center - cp0));
        _float d1 = XMVectorGetX(XMVector3LengthSq(center - cp1));
        _float d2 = XMVectorGetX(XMVector3LengthSq(center - cp2));

        if (d0 < d1 && d0 < d2)
            closest = cp0;
        else if (d1 < d2)
            closest = cp1;
        else
            closest = cp2;
    }

    // --- 5️⃣ 중심과 최근접점 사이 거리 계산 ---
    _vector diff = center - closest;
    _float distSq = XMVectorGetX(XMVector3LengthSq(diff));
    _float radius = sphere.Radius;

    if (distSq > radius * radius)
        return false;

    _float distActual = sqrtf(distSq);
    _float penetration = radius - distActual;
    if (penetration < 0.f)
        return false;

    _vector normal = (distActual > 0.0001f) ? XMVector3Normalize(diff) : n; // 중심이 겹쳤을 때 fallback

    _vector hitPos = closest;

    XMStoreFloat3(oHit, hitPos);
    XMStoreFloat3(oNormal, normal);
    *oPen = penetration;
    return true;
}

_bool CQuadTree::TestAABBTriangle(const BoundingBox& box, const _float3& a, const _float3& b, const _float3& c,
                                  OUT _float3* oHit, OUT _float3* oNormal, OUT _float* oPen)
{
    // 삼각형의 AABB 계산
    _vector v0 = XMLoadFloat3(&a);
    _vector v1 = XMLoadFloat3(&b);
    _vector v2 = XMLoadFloat3(&c);

    _vector vMin = XMVectorMin(XMVectorMin(v0, v1), v2);
    _vector vMax = XMVectorMax(XMVectorMax(v0, v1), v2);

    BoundingBox triBox;
    BoundingBox::CreateFromPoints(triBox, vMin, vMax);

    // 간단히 박스와 박스 교차로 근사
    if (!box.Intersects(triBox))
        return false;

    // 노멀 (삼각형 평면 기준)
    _vector e0 = v1 - v0;
    _vector e1 = v2 - v0;
    _vector n = XMVector3Normalize(XMVector3Cross(e0, e1));

    XMStoreFloat3(oNormal, n);

    // 히트 포인트는 삼각형 중심 근사
    _vector avg = (v0 + v1 + v2) / 3.f;
    XMStoreFloat3(oHit, avg);

    *oPen = 0.001f; // 근사 침투값
    return true;
}

_bool CQuadTree::TestOBBTriangle(const BoundingOrientedBox& obb, const _float3& a, const _float3& b, const _float3& c,
                                 OUT _float3* oHit, OUT _float3* oNormal, OUT _float* oPen)
{
    _vector v0 = XMLoadFloat3(&a);
    _vector v1 = XMLoadFloat3(&b);
    _vector v2 = XMLoadFloat3(&c);

    // 삼각형을 감싸는 AABB 계산
    _vector vMin = XMVectorMin(XMVectorMin(v0, v1), v2);
    _vector vMax = XMVectorMax(XMVectorMax(v0, v1), v2);

    BoundingBox triBox;
    BoundingBox::CreateFromPoints(triBox, vMin, vMax);

    if (!obb.Intersects(triBox))
        return false;

    _vector e0 = v1 - v0;
    _vector e1 = v2 - v0;
    _vector n = XMVector3Normalize(XMVector3Cross(e0, e1));

    XMStoreFloat3(oNormal, n);
    _vector avg = (v0 + v1 + v2) / 3.f;
    XMStoreFloat3(oHit, avg);
    *oPen = 0.001f;
    return true;
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Created : CQuadTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	__super::Free();

	for (auto& pQuadTree : m_Children)
		Safe_Release(pQuadTree);
}
