#pragma once
#include "Base.h"

BEGIN(Engine)

class CGameObject;

struct AABB
{
    _float3 min;
    _float3 max;
};

inline _bool AABBOverlap(const AABB& a, const AABB& b)
{
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) && (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// 레이 vs AABB (t-range: [0, maxDist])
// rayDir은 반드시 정규화된 월드 방향 벡터라고 가정.
inline  _bool RayHitsAABB(const _float3& rayOrigin, const _float3& rayDir, _float maxDist, const AABB& box)
{
    _float tmin = 0.0f;
    _float tmax = maxDist;

    // 각 축 검사
    for (int i = 0; i < 3; ++i)
    {
        _float origin = ((&rayOrigin.x)[i]);
        _float dir = ((&rayDir.x)[i]);
        _float bmin = ((&box.min.x)[i]);
        _float bmax = ((&box.max.x)[i]);

        // dir이 0에 가까우면 평행한 축
        if (fabsf(dir) < 1e-6f)
        {
            // 레이가 그 축에서 박스 범위 밖이면 히트 불가능
            if (origin < bmin || origin > bmax)
                return false;
            // 평행이면 tmin/tmax는 그대로 유지
        }
        else
        {
            _float invD = 1.0f / dir;
            _float t0 = (bmin - origin) * invD;
            _float t1 = (bmax - origin) * invD;
            if (t0 > t1)
                std::swap(t0, t1);

            if (t0 > tmin)
                tmin = t0;
            if (t1 < tmax)
                tmax = t1;

            if (tmax < tmin)
                return false;
        }
    }

    return tmin <= tmax && tmin <= maxDist;
}

// 옥트리에 넣을 엔트리
struct OctreeEntry
{
    class CGameObject* pObj = nullptr;
    AABB bounds;
    _uint colType = 0;
};

class COctreeNode
{
public:
    COctreeNode(const AABB& bounds, _uint depth, _uint maxDepth, _uint maxObjectsPerNode);
    ~COctreeNode();

    // 프레임마다 새로 짓기 때문에 동적 해제는 ~OctreeNode에서 전부 처리
    void ClearRecursive(); // 자식/객체 전체 제거

    void Insert(const OctreeEntry& entry);
    void QueryAABB(const AABB& region, vector<CGameObject*>& outObjects) const;
    void QueryRay(const _float3& origin, const _float3& dir, _float maxDist,
                  vector<CGameObject*>& outObjects) const;

private:
    void subdivide();
    _bool isLeaf() const
    {
        return m_isLeaf;
    }

    // 노드 정보
    AABB m_bounds;
    _uint m_depth = 0;
    _uint m_maxDepth = 0;
    _uint m_maxObjects = 0;

    _bool m_isLeaf = true;

    // 이 노드에 직접 담긴 객체들(leaf일 때만 의미있게 유지)
    vector<OctreeEntry> m_objects;

    // 자식 8개
    COctreeNode* m_children[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
};
END