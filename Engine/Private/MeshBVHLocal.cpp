
#include "MeshBVHLocal.h"
#include "Model.h"
#include "Mesh.h"

HRESULT MeshBVHLocal::Build_BVH_Local(class CMesh* mesh)
{
    m_OwnerMesh = mesh;
    m_BVHTri.clear();
    m_TriOrder.clear();
    m_BVHNodes.clear();
    m_BVHRoot = -1;

    const _uint triCount = mesh->Get_iNumIndexices() / 3;
    m_BVHTri.resize(triCount);
    m_TriOrder.resize(triCount);

    for (_uint t = 0; t < triCount; ++t)
    {
       
        const _uint i0 = m_OwnerMesh->Get_pIndices(t * 3 + 0);
        const _uint i1 = m_OwnerMesh->Get_pIndices(t * 3 + 1);
        const _uint i2 = m_OwnerMesh->Get_pIndices(t * 3 + 2);
       
        const _float3 A = m_OwnerMesh->GetVertexPosBindPose(i0);
        const _float3 B = m_OwnerMesh->GetVertexPosBindPose(i1);
        const _float3 C = m_OwnerMesh->GetVertexPosBindPose(i2);

        _float3 centroid{(A.x + B.x + C.x) / 3.f, (A.y + B.y + C.y) / 3.f, (A.z + B.z + C.z) / 3.f};

        m_BVHTri[t] = BVHTri{i0, i1, i2, centroid};
        m_TriOrder[t] = t;
    }

    // 재귀 빌드
    auto buildRec = [&](auto&& self, _uint start, _uint count) -> _int
    {
        _float3 bMin{+FLT_MAX, +FLT_MAX, +FLT_MAX};
        _float3 bMax{-FLT_MAX, -FLT_MAX, -FLT_MAX};

        _float3 cMin{+FLT_MAX, +FLT_MAX, +FLT_MAX};
        _float3 cMax{-FLT_MAX, -FLT_MAX, -FLT_MAX};

        for (_uint k = 0; k < count; ++k)
        {
            const _uint triId = m_TriOrder[start + k];
            const BVHTri& tr = m_BVHTri[triId];

            const _float3 A =m_OwnerMesh-> GetVertexPosBindPose(tr.i0);
            const _float3 B =m_OwnerMesh-> GetVertexPosBindPose(tr.i1);
            const _float3 C =m_OwnerMesh-> GetVertexPosBindPose(tr.i2);

            bMin = Min3(bMin, Min3(A, Min3(B, C)));
            bMax = Max3(bMax, Max3(A, Max3(B, C)));

            cMin = Min3(cMin, tr.centroid);
            cMax = Max3(cMax, tr.centroid);
        }

        BVHNode node;
        node.bounds = MakeBoxFromMinMax(bMin, bMax);

        const _int myIndex = (_int)m_BVHNodes.size();
        m_BVHNodes.push_back(node);

        // leaf 조건
        if (count <= m_BVHLeafSize)
        {
            m_BVHNodes[myIndex].start = start;
            m_BVHNodes[myIndex].count = count;
            return myIndex;
        }

        // 2) split axis: centroid bounds의 가장 긴 축
        const _float3 cExt{cMax.x - cMin.x, cMax.y - cMin.y, cMax.z - cMin.z};
        int axis = 0;
        if (cExt.y > cExt.x)
            axis = 1;
        if ((axis == 0 ? cExt.z : cExt.z) > (axis == 0 ? cExt.x : cExt.y))
            axis = 2;

        const _uint mid = start + count / 2;

        auto key = [&](const _uint triId) -> float
        {
            const _float3& c = m_BVHTri[triId].centroid;
            return (axis == 0) ? c.x : (axis == 1) ? c.y : c.z;
        };

        nth_element(m_TriOrder.begin() + start, m_TriOrder.begin() + mid, m_TriOrder.begin() + (start + count),
                    [&](const _uint a, const _uint b) { return key(a) < key(b); });

        const _uint leftCount = mid - start;
        const _uint rightCount = count - leftCount;

        const _int L = self(self, start, leftCount);
        const _int R = self(self, mid, rightCount);

        m_BVHNodes[myIndex].left = L;
        m_BVHNodes[myIndex].right = R;

        return myIndex;
    };

    m_BVHRoot = buildRec(buildRec, 0, triCount);

   return S_OK;
}

_bool MeshBVHLocal::RayIntersect_BVH_Local( _vector oL, _vector dL, OUT HitResult& outBest) const
{
    if (m_BVHRoot < 0 || m_BVHNodes.empty())
        return false;
    
    priority_queue<PQItem, std::vector<PQItem>, Compare> pq;

    _float rootDist = 0.f;
    if (!m_BVHNodes[m_BVHRoot].bounds.Intersects(oL, dL, rootDist))
        return false;

    pq.push({m_BVHRoot, rootDist});

    while (!pq.empty())
    {
        PQItem it = pq.top();
        pq.pop();

        if (outBest.hit && it.dist >= outBest.distance)
            break;

        const BVHNode& n = m_BVHNodes[it.node];

        if (n.IsLeaf())
        {
            for (_uint k = 0; k < n.count; ++k)
            {
                const _uint triId = m_TriOrder[n.start + k];
                const BVHTri& tr = m_BVHTri[triId];

                const _float3 A = m_OwnerMesh->GetVertexPosLocal_Current(tr.i0);
                const _float3 B = m_OwnerMesh->GetVertexPosLocal_Current(tr.i1);
                const _float3 C = m_OwnerMesh->GetVertexPosLocal_Current(tr.i2);

                const _vector vA = XMLoadFloat3(&A);
                const _vector vB = XMLoadFloat3(&B);
                const _vector vC = XMLoadFloat3(&C);

                float tDist = 0.f;
                if (TriangleTests::Intersects(oL, dL, vA, vB, vC, tDist))
                {
                    if (tDist <= 0.f)
                        continue;
                    if (outBest.hit && tDist >= outBest.distance)
                        continue;

                    outBest.hit = true;
                    outBest.distance = tDist;
                    outBest.position = XMVectorAdd(oL, XMVectorScale(dL, tDist));

                    const _vector e0 = XMVectorSubtract(vB, vA);
                    const _vector e1 = XMVectorSubtract(vC, vA);
                    outBest.normal = XMVector3Normalize(XMVector3Cross(e0, e1));
                }
            }
            continue;
        }


        const _int L = n.left;
        const _int R = n.right;

        _float dLHit = 0.f, dRHit = 0.f;
        const _bool hitL = (L >= 0) ? m_BVHNodes[L].bounds.Intersects(oL, dL, dLHit) : false;
        const _bool hitR = (R >= 0) ? m_BVHNodes[R].bounds.Intersects(oL, dL, dRHit) : false;

        
        if (hitL && (!outBest.hit || dLHit < outBest.distance))
            pq.push({L, dLHit});
        if (hitR && (!outBest.hit || dRHit < outBest.distance))
            pq.push({R, dRHit});
    }


    return outBest.hit;
}

void MeshBVHLocal::Refit_BVH_Local()
{
        if (m_BVHRoot < 0 || m_BVHNodes.empty())
            return;

        // nodeIdx 리턴: 해당 노드의 최신 bounds
        auto refitRec = [&](auto&& self, _int nodeIdx) -> BoundingBox
        {
            BVHNode& node = m_BVHNodes[nodeIdx];

            if (node.IsLeaf())
            {
                _float3 bMin{+FLT_MAX, +FLT_MAX, +FLT_MAX};
                _float3 bMax{-FLT_MAX, -FLT_MAX, -FLT_MAX};

                for (_uint k = 0; k < node.count; ++k)
                {
                    const _uint triId = m_TriOrder[node.start + k];
                    const BVHTri& tr = m_BVHTri[triId];

                    const _float3 A = m_OwnerMesh->GetVertexPosLocal_Current(tr.i0);
                    const _float3 B = m_OwnerMesh->GetVertexPosLocal_Current(tr.i1);
                    const _float3 C = m_OwnerMesh->GetVertexPosLocal_Current(tr.i2);

                    bMin.x = min(bMin.x, min(A.x, min(B.x, C.x)));
                    bMin.y = min(bMin.y, min(A.y, min(B.y, C.y)));
                    bMin.z = min(bMin.z, min(A.z, min(B.z, C.z)));

                    bMax.x = max(bMax.x, max(A.x, max(B.x, C.x)));
                    bMax.y = max(bMax.y, max(A.y, max(B.y, C.y)));
                    bMax.z = max(bMax.z, max(A.z, max(B.z, C.z)));
                }

                const _float3 c{(bMin.x + bMax.x) * 0.5f, (bMin.y + bMax.y) * 0.5f, (bMin.z + bMax.z) * 0.5f};
                const _float3 e{(bMax.x - bMin.x) * 0.5f, (bMax.y - bMin.y) * 0.5f, (bMax.z - bMin.z) * 0.5f};

                node.bounds = BoundingBox(c, e);
                return node.bounds;
            }

            // internal: 자식부터 갱신한 후 merge
            const _int L = node.left;
            const _int R = node.right;

            BoundingBox bL, bR;

            _bool hasL = (L >= 0);
            _bool hasR = (R >= 0);

            if (hasL)
                bL = self(self, L);
            if (hasR)
                bR = self(self, R);

            if (hasL && hasR)
            {
                BoundingBox merged;
                BoundingBox::CreateMerged(merged, bL, bR);
                node.bounds = merged;
            }
            else if (hasL)
            {
                node.bounds = bL;
            }
            else if (hasR)
            {
                node.bounds = bR;
            }

            return node.bounds;
        };    

        refitRec(refitRec, m_BVHRoot);
}

MeshBVHLocal* MeshBVHLocal::Create(CMesh* mesh)
{
    MeshBVHLocal* pInstance = new MeshBVHLocal();

    if (FAILED(pInstance->Build_BVH_Local(mesh)))
    {
        MSG_BOX("Failed To Cloned : MeshBVHLocal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void MeshBVHLocal::Free()
{
    m_OwnerMesh = nullptr;
}
