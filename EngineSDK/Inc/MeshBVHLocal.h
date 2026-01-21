#pragma once
#include "Base.h"

BEGIN(Engine)
class CMesh;


class MeshBVHLocal : public CBase
{
private:
    struct BVHTri
    {
        _uint i0, i1, i2; // index buffer의 vertex index
        _float3 centroid; // bind-pose 기준
    };

    struct BVHNode
    {
        BoundingBox bounds; // bind-pose 기준 bounds
        _int left = -1;
        _int right = -1;

        // leaf일 때만 사용
        _uint start = 0; // m_TriOrder에서 시작
        _uint count = 0; // leaf tri 개수

        _bool IsLeaf() const
        {
            return left < 0 && right < 0;
        }
    };
    struct PQItem
    {
        _int node;
        _float dist;
    };
    struct Compare
    {
        _bool operator()(const PQItem& a, const PQItem& b) const
        {
            return a.dist > b.dist; // 작은 dist 우선
        }
    };


public:
    MeshBVHLocal() = default;
    virtual ~MeshBVHLocal() = default;

    HRESULT Build_BVH_Local( class CMesh* mesh); // 로딩 후 1회
    _bool RayIntersect_BVH_Local( _vector oL, _vector dL, OUT HitResult& outBest) const;
    void Refit_BVH_Local();


private:
    _float3 Min3(const _float3& a, const _float3& b)
    {
        return _float3{min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)};
    }
    _float3 Max3(const _float3& a, const _float3& b)
    {
        return _float3{max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)};
    }

     BoundingBox MakeBoxFromMinMax(const _float3& mn, const _float3& mx)
    {
        _float3 c{(mn.x + mx.x) * 0.5f, (mn.y + mx.y) * 0.5f, (mn.z + mx.z) * 0.5f};
        _float3 e{(mx.x - mn.x) * 0.5f, (mx.y - mn.y) * 0.5f, (mx.z - mn.z) * 0.5f};
        return BoundingBox(c, e);
    }


private:
    vector<BVHTri> m_BVHTri;  // triCount개
    vector<_uint> m_TriOrder; // tri index 순서(빌드 중 재배열)
    vector<BVHNode> m_BVHNodes;
    _int m_BVHRoot = -1;
    _uint m_BVHLeafSize = 8; 
    class CMesh* m_OwnerMesh = nullptr;

public:
    static MeshBVHLocal* Create(class CMesh* mesh);

    virtual void Free() override;

};

END