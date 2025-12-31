#include "StaticBVH.h"
#include "Model.h"
#include "VIBuffer_Terrain.h"
#include "Collider.h"
#include "GameObject.h"
void CStaticBVH::Clear()
{
    m_entries.clear();
    m_nodes.clear();
}

void CStaticBVH::Build(const vector<Entry>& staticEntries)
{
    Clear();
    m_entries = staticEntries;
    
    m_nodes.reserve(m_entries.size() * 2);

    if (m_entries.empty())
        return;

    BuildRecursive(0, (int)m_entries.size());
}

_int CStaticBVH::BuildRecursive(_int start, _int end)
{
    Node node{};

    node.bounds = m_entries[start].bounds;
    for (_int i = start + 1; i < end; ++i)
        node.bounds = MergeAABB(node.bounds, m_entries[i].bounds);

    const _int nodeIdx = (_int)m_nodes.size();
    m_nodes.push_back(node);

    const _int count = end - start;
    if (count == 1)
    {
        m_nodes[nodeIdx].entryIndex = start; // leaf
        return nodeIdx;
    }

    const _int axis = LongestAxis(node.bounds);
    const _int mid = (start + end) / 2;

    std::nth_element( //컨테이너를 ‘부분 정렬’해서 n번째 위치의 원소가 “정렬된 상태였다면 거기에 있어야 할 값”이 되게 만드는 함수
        m_entries.begin() + start, m_entries.begin() + mid, m_entries.begin() + end,
        [&](const Entry& a, const Entry& b)
        {
            const _float3 ca = Center(a.bounds);
            const _float3 cb = Center(b.bounds);
            return (axis == 0 ? ca.x : axis == 1 ? ca.y : ca.z) < (axis == 0 ? cb.x : axis == 1 ? cb.y : cb.z);
        });

    m_nodes[nodeIdx].left = BuildRecursive(start, mid);
    m_nodes[nodeIdx].right = BuildRecursive(mid, end);
    return nodeIdx;
}

_bool CStaticBVH::Raycast(const _vector& vRayPos, const _vector& vRayDir, OUT HitResult& out, _float maxDist,
                          OUT _int* Type ) const
{
  
   if (XMVectorGetX(XMVector3LengthSq(vRayDir)) < 1e-12f)
        return false;

    out = HitResult{};
    if (m_nodes.empty())
        return false;

    _float closest = maxDist;
    TraverseRay(0, vRayPos, vRayDir, closest, out,Type);
    return out.hit;
}

void CStaticBVH::TraverseRay(_int nodeIdx, const _vector& vRayPos, const _vector& vRayDir, _float& closest,
                             OUT HitResult& out, OUT _int* Type) const
{
    const Node& n = m_nodes[nodeIdx];
    if (!RayIntersectAABB(vRayPos, vRayDir, n.bounds, closest))
        return;

    if (n.entryIndex != -1)
    {
        const Entry& entry = m_entries[n.entryIndex];
        if (!entry.object)
            return;

        if (entry.type == EPrimType::ModelMesh && entry.model && entry.transform)
        {
            if (!entry.col)
                return;
           
            _float colDist = FLT_MAX;
            if (!entry.col->RayIntersects(vRayPos, vRayDir, colDist))
                return; 

            if (colDist >= closest)
                return;
            

            _vector hitPos{}, hitNrm{};
            if (entry.model->RayIntersect(vRayPos, vRayDir, entry.transform, hitPos, hitNrm))
            {
               _float dist = XMVectorGetX(XMVector3Length(hitPos - vRayPos));

                if (dist < closest)
                {
                    closest = dist;
                    out.hit = true;
                    out.distance = dist;
                    out.position = hitPos;
                    out.normal = hitNrm;
                    out.object = entry.object;
                    if (Type)
                    *Type = EPrimType::ModelMesh;
                }
            }

            return;
        }

        if (entry.type == EPrimType::Terrain && entry.terrain && entry.transform)
        {
            _float colDist = FLT_MAX;
            if (!entry.col)
                return;

            if (!entry.col->RayIntersects(vRayPos, vRayDir, colDist))
                return;

            if (colDist >= closest)
                return;

            _float t = 0.f;
            _float3 nrm{}, pos{};
            _float3 dir3{};
            XMStoreFloat3(&dir3, vRayDir);

            if (entry.terrain->Picking_OnTerrain_QuadTree(vRayPos, XMVector3Normalize(vRayDir), entry.transform, &t, &nrm, &pos))
            {
                if (t < closest)
                {
                    closest = t;
                    out.hit = true;
                    out.distance = t;
                    out.position = XMVectorSet(pos.x, pos.y, pos.z, 1.f);
                    out.normal = XMVectorSet(nrm.x, nrm.y, nrm.z, 0.f);
                    out.object = entry.object;
                    if (Type)
                    *Type = EPrimType::Terrain;
                }
            }
            return;
        }

        return;
    }

    TraverseRay(n.left, vRayPos, vRayDir, closest, out,Type);
    TraverseRay(n.right, vRayPos, vRayDir, closest, out,Type);
}

AABB CStaticBVH::MergeAABB(const AABB& a, const AABB& b)
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

_float3 CStaticBVH::Center(const AABB& a)
{
    return _float3((a.min.x + a.max.x) * 0.5f, (a.min.y + a.max.y) * 0.5f, (a.min.z + a.max.z) * 0.5f);
}

_int CStaticBVH::LongestAxis(const AABB& a)
{
    const _float ex = a.max.x - a.min.x;
    const _float ey = a.max.y - a.min.y;
    const _float ez = a.max.z - a.min.z;
    if (ex > ey && ex > ez)
        return 0; // X;
    if (ey > ez)
        return 1; //Y
    return 2; //Z
}

_bool CStaticBVH::RayIntersectAABB(const _vector& vRayPos, const _vector& vRayDir , const AABB& box, _float tMax) const
{
     // 구간 교차 여부 확인
    _float3 fRayPos{}, fRayDir{};
     XMStoreFloat3(&fRayPos, vRayPos);
     XMStoreFloat3(&fRayDir, vRayDir);
     
     //머무는 시간 구간
     _float tmin = 0.0f; // 완전히 들어온 가장 늦은 시간
     _float tmax = tMax; // 완전히 나가기 전 가장 이른 기간
     
     //구간 교차 검사
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
     

     //Ray가 X/Y/Z 모든 축 slab을 “같은 시간대에” 통과할 수 있는가?
     if (!slab(fRayPos.x, fRayDir.x, box.min.x, box.max.x))
         return false;
     if (!slab(fRayPos.y, fRayDir.y, box.min.y, box.max.y))
         return false;
     if (!slab(fRayPos.z, fRayDir.z, box.min.z, box.max.z))
         return false;
     return true;
}



