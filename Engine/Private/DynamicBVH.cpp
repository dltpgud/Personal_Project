#include "DynamicBVH.h"
#include "GameObject.h"
#include "Collider.h"


    // 초기화: 동적 오브젝트 목록과 BVH 트리 초기화
    void CDynamicBVH::Clear()
    {
        m_entries.clear();
        m_nodes.clear();
    }

    // 동적 오브젝트로부터 BVH 트리 빌드
    void CDynamicBVH::Build(const std::vector<Entry>& dynamicEntries)
    {
        Clear();
        m_entries = dynamicEntries;

        m_nodes.reserve(m_entries.size() * 2);

        if (m_entries.empty())
            return;

        BuildRecursive(0, (int)m_entries.size(), -1);
    }

    // 재귀적으로 BVH 트리 빌드
    _int CDynamicBVH::BuildRecursive(_int start, _int end, _int parentIdx)
    {
        Node node{};

        // AABB 병합: 여러 객체들의 AABB를 병합하여 해당 노드의 AABB 생성
        node.bounds = m_entries[start].bounds;
        for (_int i = start + 1; i < end; ++i) node.bounds = MergeAABB(node.bounds, m_entries[i].bounds);

        const _int nodeIdx = (_int)m_nodes.size();
        m_nodes.push_back(node);
        m_nodes[nodeIdx].parentIdx = parentIdx;

        const _int count = end - start;
        if (count == 1)
        {
            m_nodes[nodeIdx].entryIndex = start; // Leaf node
            return nodeIdx;
        }

        const _int axis = LongestAxis(node.bounds);
        const _int mid = (start + end) / 2;

        std::nth_element(
            m_entries.begin() + start, m_entries.begin() + mid, m_entries.begin() + end,
            [&](const Entry& a, const Entry& b)
            {
                const XMFLOAT3 ca = Center(a.bounds);
                const XMFLOAT3 cb = Center(b.bounds);
                return (axis == 0 ? ca.x : axis == 1 ? ca.y : ca.z) < (axis == 0 ? cb.x : axis == 1 ? cb.y : cb.z);
            });

        m_nodes[nodeIdx].left = BuildRecursive(start, mid, nodeIdx);
        m_nodes[nodeIdx].right = BuildRecursive(mid, end, nodeIdx);
        return nodeIdx;
    }

    // 동적 오브젝트들의 AABB 갱신
    void CDynamicBVH::UpdateDynamicObjects()
    {
        for (auto& entry : m_entries)
        {
            if (entry.isActive && entry.object)
            {
                entry.bounds = entry.object->Get_WorldAABB(); // AABB 갱신
                UpdateNodeBounds(entry);                      // 해당 노드의 AABB 갱신
            }
        }
    }

    // 노드의 AABB 갱신
    void CDynamicBVH::UpdateNodeBounds(const Entry& entry)
    {
        for (auto& node : m_nodes)
        {
            if (node.entryIndex == &entry - &m_entries[0]) // entryIndex와 node를 비교하여 해당 노드 찾기
            {
                node.bounds = entry.bounds; // AABB 갱신

                int nodeIdx = node.parentIdx;
                while (nodeIdx >= 0)
                {
                    nodeIdx = m_nodes[nodeIdx].parentIdx;
                    if (nodeIdx < 0)
                        break;

                    m_nodes[nodeIdx].bounds = MergeAABB(m_nodes[nodeIdx].bounds, node.bounds); // 부모 노드 AABB 갱신
                }
                return;
            }
        }
    }

    _bool CDynamicBVH::RayIntersectAABB(const _vector& vRayPos, const _vector& vRayDir, const AABB& box,
                                        _float tMax) const
    {

        // 구간 교차 여부 확인
        _float3 fRayPos{}, fRayDir{};
        XMStoreFloat3(&fRayPos, vRayPos);
        XMStoreFloat3(&fRayDir, vRayDir);

        // 머무는 시간 구간
        _float tmin = 0.0f; // 완전히 들어온 가장 늦은 시간
        _float tmax = tMax; // 완전히 나가기 전 가장 이른 기간

        // 구간 교차 검사
        auto slab = [&](_float Pos, _float dir, _float minv, _float maxv) -> _bool
        {
            const _float eps = 1e-8f;
            if (fabsf(dir) < eps) // 축과 거의 평행
            {
                return (Pos >= minv && Pos <= maxv);
            }
            _float invD = 1.0f / dir;
            _float t0 = (minv - Pos) * invD; // 들어온 시간
            _float t1 = (maxv - Pos) * invD; // 나간시간
            if (t0 > t1)
                std::swap(t0, t1); // Ray 방향이 반대면 순서 뒤집고

            // x,y,z, 축에서 가능한 시간 구간
            tmin = max(tmin, t0);
            tmax = min(tmax, t1);

            // 겹치는 구간이 하나라도 있으면 통과
            return tmin <= tmax;
        };

        // Ray가 X/Y/Z 모든 축 slab을 “같은 시간대에” 통과할 수 있는가?
        if (!slab(fRayPos.x, fRayDir.x, box.min.x, box.max.x))
            return false;
        if (!slab(fRayPos.y, fRayDir.y, box.min.y, box.max.y))
            return false;
        if (!slab(fRayPos.z, fRayDir.z, box.min.z, box.max.z))
            return false;
        return true;
    }

    // AABB와의 충돌 검사
    bool CDynamicBVH::Raycast(const _vector& vRayPos, const _vector& vRayDir, HitResult& out, _float maxDist) const
    {
        out = HitResult{};
        if (m_nodes.empty())
            return false;

        _float closest = maxDist;
        TraverseRay(0, vRayPos, vRayDir, closest, out);
        return out.hit;
    }

    // 레이 캐스트를 재귀적으로 탐색하여 충돌 검사
    void CDynamicBVH::TraverseRay(int nodeIdx, const _vector& vRayPos, const _vector& vRayDir, float& closest, HitResult& out) const
    {
        const Node& n = m_nodes[nodeIdx];

        if (!RayIntersectAABB(vRayPos, vRayDir, n.bounds, closest))
            return;

        if (n.entryIndex != -1)
        {
            const Entry& entry = m_entries[n.entryIndex];

            if (!entry.isActive) // 죽은 오브젝트는 건너뛰기
                return;

            if (entry.collider && entry.collider->RayIntersects(vRayPos, vRayDir, closest))
            {
                // 충돌이 발생하면 결과 저장
                out.hit = true;
                out.object = entry.object;
            }
            return;
        }

        // 자식 노드로 재귀적으로 내려가기
        if (n.left != -1)
            TraverseRay(n.left, vRayPos, vRayDir, closest, out);
        if (n.right != -1)
            TraverseRay(n.right, vRayPos, vRayDir, closest, out);
    }

    // AABB 병합
    AABB CDynamicBVH::MergeAABB(const AABB& a, const AABB& b)
    {
        AABB r;
        r.min.x = min(a.min.x, b.min.x);
        r.min.y = min(a.min.y, b.min.y);
        r.min.z = min(a.min.z, b.min.z);
        r.max.x = max(a.max.x, b.max.x);
        r.max.y = max(a.max.y, b.max.y);
        r.max.z = max(a.max.z, b.max.z);
        return r;
    }

    // AABB의 교차 여부 확인
    bool CDynamicBVH::OverlapAABB(const AABB& a, const AABB& b) const
    {
        if (a.max.x < b.min.x || a.min.x > b.max.x)
            return false;
        if (a.max.y < b.min.y || a.min.y > b.max.y)
            return false;
        if (a.max.z < b.min.z || a.min.z > b.max.z)
            return false;
        return true;
    }

    // AABB의 중심 계산
    _float3 CDynamicBVH::Center(const AABB& a)
    {
        return _float3((a.min.x + a.max.x) * 0.5f, (a.min.y + a.max.y) * 0.5f, (a.min.z + a.max.z) * 0.5f);
    }

    // 가장 긴 축을 찾는 함수
    _int CDynamicBVH::LongestAxis(const AABB& a)
    {
        const _float ex = a.max.x - a.min.x;
        const _float ey = a.max.y - a.min.y;
        const _float ez = a.max.z - a.min.z;
        if (ex > ey && ex > ez)
            return 0; // X;
        if (ey > ez)
            return 1; // Y
        return 2;     // Z
    }


