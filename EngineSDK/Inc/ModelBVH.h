#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CModel;
class CMesh;

class CModelBVH : public CBase
{
public:
    struct Hit
    {
        _bool hit = false;
        _float t = FLT_MAX; // local ray parameter
        _vector posL = XMVectorZero();
        _vector nL = XMVectorZero();
        void* user = nullptr; // 필요하면 mesh/object 포인터 넣기
        uint32_t meshIndex = 0;
        uint32_t triIndex = 0; // "tri entry index" (내부)
    };

public:
    CModelBVH() = default;
    ~CModelBVH() = default;

    void Clear();

    // 모델의 "bind pose(or static) 로컬 버텍스" 기반으로 1회 빌드
    // (애니메이션은 bind pose로 build하는 게 정석)
    _bool BuildFromModelBindPose(const CModel& model, const vector<CMesh*>& Mesh);

    // 로컬 레이(oL, dL은 local space, dL은 normalize 권장)
    _bool RaycastLocal(_uint Type, const vector<CMesh*>& Mesh, const _vector& oL, const _vector& dL, Hit& out,
                       _float tMax = FLT_MAX) const;

    _bool Empty() const
    {
        return m_nodes.empty();
    }
    size_t GetNodeCount() const
    {
        return m_nodes.size();
    }
    size_t GetTriCount() const
    {
        return m_tris.size();
    }

private:
    struct Tri
    {
        AABB bounds;         // local triangle AABB (bind pose)
        uint32_t i0, i1, i2; // index into mesh vertex array
        uint32_t meshIndex;  // which mesh
        // 정렬용 (center)
        _float3 center;
    };

    struct Node
    {
        AABB bounds;
        _int left = -1;
        _int right = -1;

        _int triStart = 0;
        _int triCount = 0;

        _bool IsLeaf() const
        {
            return left < 0 && right < 0;
        }
    };

private:
    // --- Build ---
    _int BuildNode(_int start, _int end, _int depth);
    AABB ComputeBounds(_int start, _int end) const;
    _int LongestAxis(const AABB& a) const;

    // --- Ray ---
    _bool RayAABB_Slab(const _vector& o, const _vector& d, const AABB& b, _float tMax) const;
    void TraverseRay(_uint Type, const vector<CMesh*>& Mesh, _int nodeIdx, const _vector& o, const _vector& d,
                     Hit& io, _float tMax) const;

    // leaf에서 삼각형 정밀 테스트 (정적/바인드포즈)
    _bool IntersectTriangle_BindPose(const vector<CMesh*>& Mesh, const Tri& tri, const _vector& oL,
                                     const _vector& dL,
                                     _float& outT, _vector& outN) const;
    _bool IntersectTriangle_Skinned(const CMesh* mesh, const Tri& tri, const _vector& oL, const _vector& dL,
                                   _float& outT,
                              _vector& outN) const;
    static AABB MergeAABB(const AABB& a, const AABB& b);

private:
    vector<Tri> m_tris;
    vector<Node> m_nodes;

    // leaf에 들어갈 삼각형 최대 개수 (튜닝값)
    int m_leafTriLimit = 4;
};


END