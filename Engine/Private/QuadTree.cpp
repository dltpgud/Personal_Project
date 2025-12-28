#include "..\Public\QuadTree.h"
#include "Collider.h"
#include "GameInstance.h"
#include <algorithm>


CQuadTree::CQuadTree() : m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB, const _float3* pVerticesPos)
{
    m_iCorners[CORNER_LT] = iLT; // 왼쪽 위
    m_iCorners[CORNER_RT] = iRT; // 오른쪽 위
    m_iCorners[CORNER_RB] = iRB; // 오른쪽 아래
    m_iCorners[CORNER_LB] = iLB; // 왼쪽 아래

    /////// 쿼드 트리 피킹용 캐싱
    _float3 minPt = pVerticesPos[iLT];
    _float3 maxPt = pVerticesPos[iLT];

    for (_int i = 1; i < 4; ++i)
    {
        const _float3& v = pVerticesPos[m_iCorners[i]];
        minPt.x = min(minPt.x, v.x);
        minPt.y = min(minPt.y, v.y);
        minPt.z = min(minPt.z, v.z);
        maxPt.x = max(maxPt.x, v.x);
        maxPt.y = max(maxPt.y, v.y);
        maxPt.z = max(maxPt.z, v.z);
    }

    m_Bounds.Center = _float3((minPt.x + maxPt.x) * 0.5f, (minPt.y + maxPt.y) * 0.5f, (minPt.z + maxPt.z) * 0.5f);
    m_Bounds.Extents = _float3((maxPt.x - minPt.x) * 0.5f, (maxPt.y - minPt.y) * 0.5f, (maxPt.z - minPt.z) * 0.5f);

    ////////

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
    m_Children[CORNER_LT] = CQuadTree::Create(m_iCorners[CORNER_LT], iTC, m_iCenter, iLC,pVerticesPos);
    m_Children[CORNER_RT] = CQuadTree::Create(iTC, m_iCorners[CORNER_RT], iRC, m_iCenter,pVerticesPos);
    m_Children[CORNER_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[CORNER_RB], iBC,pVerticesPos);
    m_Children[CORNER_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[CORNER_LB],pVerticesPos);

	return S_OK;
}

void CQuadTree::Culling( const _float3* pVerticesPos, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrixInv)
{
    //  isDraw() 함수로 카메라와의 거리를 계산해서 LOD를 결정
    if (nullptr == m_Children[CORNER_LT] || true == isDraw(pVerticesPos, WorldMatrixInv))
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
            m_pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[0]]), 1.f), fSafetyMargin),
            m_pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[1]]), 1.f), fSafetyMargin),
            m_pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[2]]), 1.f), fSafetyMargin),
            m_pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[3]]), 1.f), fSafetyMargin),
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
    if (true == m_pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
    {
        for (auto& pChild : m_Children)
        {
            if (nullptr != pChild)
                pChild->Culling(pVerticesPos, pIndices, pNumIndices, WorldMatrixInv);
        }
    }
}

_bool CQuadTree::isDraw( const _float3 * pVerticesPos, _fmatrix WorldMatrixInv)
{
    _vector vCamPosition = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

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
    // -------------------------------------------------
    // 1) 캐싱된 AABB로 교차 + 거리 pruning
    // -------------------------------------------------
    _float boxDist = 0.f;
    if (!m_Bounds.Intersects(RayPos, RayDir, boxDist))
        return false;

    if (boxDist > fBestDist)
        return false;

    // -------------------------------------------------
    // 2) 리프 노드: 삼각형 2개 테스트
    // -------------------------------------------------
    if (m_Children[0] == nullptr)
    {
        const _uint iLT = m_iCorners[CORNER_LT];
        const _uint iRT = m_iCorners[CORNER_RT];
        const _uint iRB = m_iCorners[CORNER_RB];
        const _uint iLB = m_iCorners[CORNER_LB];

        _bool hit = false;

        // tri 1 : LT, RT, RB
        hit |= RayIntersectsTriangle(RayPos, RayDir, pVerticesPos[iLT], pVerticesPos[iRT], pVerticesPos[iRB], fBestDist,
                                     vHitPos, vHitNormal);

        // tri 2 : LT, RB, LB
        hit |= RayIntersectsTriangle(RayPos, RayDir, pVerticesPos[iLT], pVerticesPos[iRB], pVerticesPos[iLB], fBestDist,
                                     vHitPos, vHitNormal);

        return hit;
    }

    // -------------------------------------------------
    // 3) 내부 노드: 자식 AABB로 후보 수집
    // -------------------------------------------------
    ChildPick candidates[4];
    _int count = 0;

    for (_int i = 0; i < 4; ++i)
    {
        CQuadTree* child = m_Children[i];
        if (!child)
            continue;

        _float cd = 0.f;
        if (!child->m_Bounds.Intersects(RayPos, RayDir, cd))
            continue;

        if (cd > fBestDist)
            continue;

        candidates[count++] = {child, cd};
    }

    // -------------------------------------------------
    // 4) 거리 기준 정렬 (최대 4개 → selection sort)
    // -------------------------------------------------
    for (_int i = 0; i < count; ++i)
    {
        _int best = i;
        for (_int j = i + 1; j < count; ++j)
            if (candidates[j].dist < candidates[best].dist)
                best = j;

        if (best != i)
            swap(candidates[i], candidates[best]);
    }

    // -------------------------------------------------
    // 5) 가까운 자식부터 재귀
    // -------------------------------------------------
    _bool hit = false;
    for (_int i = 0; i < count; ++i)
    {
        if (candidates[i].dist > fBestDist)
            break;

        if (candidates[i].node->Picking_Ray(pVerticesPos, RayPos, RayDir, fBestDist, vHitPos, vHitNormal))
            hit = true;
    }

    return hit;
}

_bool CQuadTree::RayIntersectsTriangle(_vector RayPos, _vector RayDir, const _float3& v0, const _float3& v1,
                                       const _float3& v2, _float& ioBestDist, _float3& outHitPos, _float3& outNormal)
{
   _float dist = 0.f;

   if (!TriangleTests::Intersects(RayPos, RayDir, XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), dist))
       return false;

   if (dist < 0.f || dist >= ioBestDist)
       return false;

   ioBestDist = dist;

   // hit position
   _vector hitP = RayPos + RayDir * dist;
   XMStoreFloat3(&outHitPos, hitP);

   // normal
   _vector e0 = XMLoadFloat3(&v1) - XMLoadFloat3(&v0);
   _vector e1 = XMLoadFloat3(&v2) - XMLoadFloat3(&v0);
   _vector n = XMVector3Normalize(XMVector3Cross(e0, e1));

   // Ray 반대 방향으로 노멀 정렬 (Y-up 가정 제거)
   if (XMVectorGetX(XMVector3Dot(n, RayDir)) > 0.f)
       n = XMVectorNegate(n);

   XMStoreFloat3(&outNormal, n);
   return true;
}


CQuadTree* CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB, const _float3* pVerticesPos)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB, pVerticesPos)))
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

    Safe_Release(m_pGameInstance);
}
