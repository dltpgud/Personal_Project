#include "ModelBVH.h"
#include "Model.h"
#include "Mesh.h"

// -------- util --------
static inline XMFLOAT3 MakeCenter(const AABB& b)
{
    return XMFLOAT3((b.min.x + b.max.x) * 0.5f, (b.min.y + b.max.y) * 0.5f, (b.min.z + b.max.z) * 0.5f);
}

AABB CModelBVH::MergeAABB(const AABB& a, const AABB& b)
{
    AABB o;
    o.min = XMFLOAT3(min(a.min.x, b.min.x), min(a.min.y, b.min.y), min(a.min.z, b.min.z));
    o.max = XMFLOAT3(max(a.max.x, b.max.x), max(a.max.y, b.max.y), max(a.max.z, b.max.z));
    return o;
}

void CModelBVH::Clear()
{
    m_tris.clear();
    m_nodes.clear();
}

bool CModelBVH::BuildFromModelBindPose(const CModel& model, const vector<CMesh*>& Mesh)
{
    Clear();
    constexpr float SKIN_MARGIN = 0.5f; // 모델 스케일에 맞게 0.2~1.0 튜닝
    // 1) tri 수집
    const uint32_t meshCount = (uint32_t)Mesh.size();
    m_tris.reserve(1024);

    for (uint32_t mi = 0; mi < meshCount; ++mi)
    {
        const CMesh* mesh = Mesh[mi];

        const uint32_t idxCount = (uint32_t)mesh->Get_iNumIndices();
        if (idxCount < 3)
            continue;

        const uint32_t triCount = idxCount / 3;

        for (uint32_t t = 0; t < triCount; ++t)
        {
            const uint32_t i0 = mesh->Get_pIndices(t * 3 + 0);
            const uint32_t i1 = mesh->Get_pIndices(t * 3 + 1);
            const uint32_t i2 = mesh->Get_pIndices(t * 3 + 2);

            const _float3 A = model.GetVertexPos(mesh, i0);
            const _float3 B = model.GetVertexPos(mesh, i1);
            const _float3 C = model.GetVertexPos(mesh, i2);

            AABB bb;
            bb.min = _float3(min(A.x, min(B.x, C.x)), min(A.y, min(B.y, C.y)), min(A.z, min(B.z, C.z)));
            bb.max = _float3(max(A.x, max(B.x, C.x)), max(A.y, max(B.y, C.y)), max(A.z, max(B.z, C.z)));
    

            bb.min.x -= SKIN_MARGIN;
            bb.min.y -= SKIN_MARGIN;
            bb.min.z -= SKIN_MARGIN;
            bb.max.x += SKIN_MARGIN;
            bb.max.y += SKIN_MARGIN;
            bb.max.z += SKIN_MARGIN;

            Tri tri;
            tri.bounds = bb;
            tri.i0 = i0;
            tri.i1 = i1;
            tri.i2 = i2;
            tri.meshIndex = mi;
            tri.center = MakeCenter(bb);
            m_tris.push_back(tri);
        }
    }

    if (m_tris.empty())
        return false;

    // 2) BVH build
    m_nodes.reserve(m_tris.size() * 2);
    BuildNode(0, (int)m_tris.size(), 0);
    return true;
}

_int CModelBVH::BuildNode(_int start, _int end, _int /*depth*/)
{
    Node node;
    node.bounds = ComputeBounds(start, end);

    const _int nodeIdx = (_int)m_nodes.size();
    m_nodes.push_back(node);

    const _int count = end - start;
    if (count <= m_leafTriLimit)
    {
        m_nodes[nodeIdx].triStart = start;
        m_nodes[nodeIdx].triCount = count;
        return nodeIdx;
    }

    const _int axis = LongestAxis(m_nodes[nodeIdx].bounds);
    const _int mid = (start + end) / 2;

    std::nth_element(m_tris.begin() + start, m_tris.begin() + mid, m_tris.begin() + end,
                     [axis](const Tri& a, const Tri& b)
                     {
                         const _float* ca = (const _float*)&a.center;
                         const _float* cb = (const _float*)&b.center;
                         return ca[axis] < cb[axis];
                     });

    m_nodes[nodeIdx].left = BuildNode(start, mid, 0);
    m_nodes[nodeIdx].right = BuildNode(mid, end, 0);
    return nodeIdx;
}

AABB CModelBVH::ComputeBounds(_int start, _int end) const
{
    AABB bb = m_tris[start].bounds;

    constexpr _float NODE_MARGIN = 0.5f;

    bb.min.x -= NODE_MARGIN;
    bb.min.y -= NODE_MARGIN;
    bb.min.z -= NODE_MARGIN;
    bb.max.x += NODE_MARGIN;
    bb.max.y += NODE_MARGIN;
    bb.max.z += NODE_MARGIN;

    for (int i = start + 1; i < end; ++i) bb = MergeAABB(bb, m_tris[i].bounds);
    return bb;
}

int CModelBVH::LongestAxis(const AABB& a) const
{
    const _float ex = a.max.x - a.min.x;
    const _float ey = a.max.y - a.min.y;
    const _float ez = a.max.z - a.min.z;

    if (ex >= ey && ex >= ez)
        return 0;
    if (ey >= ez)
        return 1;
    return 2;
}


_bool CModelBVH::RayAABB_Slab(const _vector& vRayPos, const _vector& vRayDir, const AABB& box, _float tMax) const
{
    _float3 o{}, d{};
    XMStoreFloat3(&o, vRayPos);
    XMStoreFloat3(&d, vRayDir);

    _float tmin = 0.0f;
    _float tmax = tMax;

    auto slab = [&](_float o1, _float d1, _float minv, _float maxv) -> _bool
    {
        const _float eps = 1e-8f;
        if (std::fabs(d1) < eps)
        {
            return (o1 >= minv && o1 <= maxv);
        }
        const _float invD = 1.0f / d1;
        _float t0 = (minv - o1) * invD;
        _float t1 = (maxv - o1) * invD;
        if (t0 > t1)
            std::swap(t0, t1);
        tmin = max(tmin, t0);
        tmax = min(tmax, t1);
        return tmin <= tmax;
    };

    if (!slab(o.x, d.x, box.min.x, box.max.x))
        return false;
    if (!slab(o.y, d.y, box.min.y, box.max.y))
        return false;
    if (!slab(o.z, d.z, box.min.z, box.max.z))
        return false;
    return true;
}

_bool CModelBVH::RaycastLocal(_uint Type, const vector<CMesh*>& Mesh, const _vector& oL, const _vector& dL,
                              Hit& out, _float tMax) const
{
    out = Hit{};
    if (m_nodes.empty())
        return false;

    // dL은 normalize 권장(너가 상위에서 normalize하면 가장 좋음)
    TraverseRay(Type,Mesh, 0, oL, dL, out, tMax);
    return out.hit;
}

void CModelBVH::TraverseRay(_uint Type,
    const vector<CMesh*>& Mesh,
    _int nodeIdx,
    const _vector& oL,
    const _vector& dL,
    Hit& io,
    _float tMax) const
{
    const Node& n = m_nodes[nodeIdx];

    // 현재 최단거리(io.t)를 tMax로 같이 사용해서 가지치기 강화
    const _float clip = min(tMax, io.t);
    if (!RayAABB_Slab(oL, dL, n.bounds, clip))
        return;

    if (n.IsLeaf())
    {
      
        for (int i = 0; i < n.triCount; ++i)
        {
            const Tri& tri = m_tris[n.triStart + i];

            float t = 0.f;
            _vector nrm = XMVectorZero();
            bool hit = false;

            const CMesh* mesh = Mesh[tri.meshIndex];

            // 🔥 핵심 분기
            if (Type == CModel::TYPE_ANIM)
            {
                hit = IntersectTriangle_Skinned(mesh, tri, oL, dL, t, nrm);
            }
            else
            {
                hit = IntersectTriangle_BindPose(Mesh, tri, oL, dL, t, nrm);
            }

            if (hit && t > 0.f && t < io.t && t <= tMax)
            {
                io.hit = true;
                io.t = t;
                io.posL = oL + dL * t;
                io.nL = nrm;
                io.meshIndex = tri.meshIndex;
                io.triIndex = (uint32_t)(n.triStart + i);
            }
        }
        return;
    }

    // 내부 노드: 양쪽 재귀
    TraverseRay(Type,Mesh, n.left, oL, dL, io, tMax);
    TraverseRay(Type,Mesh, n.right, oL, dL, io, tMax);
}


// 정적/바인드포즈 삼각형 정밀 테스트
_bool CModelBVH::IntersectTriangle_BindPose(const vector<CMesh*>& Mesh, const Tri& tri,
                                           const _vector& oL, const _vector& dL,
                                           _float& outT, _vector& outN) const
{
    const CMesh* mesh = Mesh[tri.meshIndex];

    const _float3 A3 = *mesh->Get_pPos(tri.i0);
    const _float3 B3 = *mesh->Get_pPos(tri.i1);
    const _float3 C3 = *mesh->Get_pPos(tri.i2);

    const _vector A = XMLoadFloat3(&A3);
    const _vector B = XMLoadFloat3(&B3);
    const _vector C = XMLoadFloat3(&C3);

    // backface cull은 선택(너가 원하면 on)
    const _vector n = XMVector3Normalize(XMVector3Cross(B - A, C - A));
    if (XMVectorGetX(XMVector3Dot(n, dL)) > 0.f)
        return false;

    _float t = 0.f;
    if (!DirectX::TriangleTests::Intersects(oL, dL, A, B, C, t))
        return false;

    outT = t;
    outN = n;
    return true;
}


_bool CModelBVH::IntersectTriangle_Skinned( const CMesh* mesh, const Tri& tri, const _vector& oL,
                                          const _vector& dL, _float& outT, _vector& outN) const
{
    // ★ 여기서만 GetVetexPosAnim 사용
    const _float3 A3 = mesh->GetVetexPosAnim(tri.i0);
    const _float3 B3 = mesh->GetVetexPosAnim(tri.i1);
    const _float3 C3 = mesh->GetVetexPosAnim(tri.i2);

    const _vector A = XMLoadFloat3(&A3);
    const _vector B = XMLoadFloat3(&B3);
    const _vector C = XMLoadFloat3(&C3);

  

    const _vector n = XMVector3Normalize(XMVector3Cross(B - A, C - A));
    float nd = XMVectorGetX(XMVector3Dot(n, dL));
    printf("n·d = %f\n", nd);


    float t = 0.f;
    if (!DirectX::TriangleTests::Intersects(oL, dL, A, B, C, t))
        return false;

    outT = t;
    outN = n;
    return true;
}
