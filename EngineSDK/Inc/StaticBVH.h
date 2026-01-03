#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)
class CGameObject;
class CCollider;
class CModel;
class CVIBuffer_Terrain;
class CTransform;

class CStaticBVH : public CBase
{
public:
    enum  EPrimType 
    {
        ModelMesh,   
        Terrain,
        PrimType_END
    };

    struct Entry
    {
        AABB bounds{};
        EPrimType type = EPrimType::PrimType_END;
        CGameObject* object = nullptr;

        CCollider* col = nullptr;
        CModel* model = nullptr;
        CVIBuffer_Terrain* terrain = nullptr;
        CTransform* transform = nullptr;
    };

private:
    struct Node
    {
        AABB bounds{};
        _int left = -1;
        _int right = -1;
        _int entryIndex = -1; // leaf일 때만 유효
        _bool isLeaf = false;
    };

public:
    CStaticBVH() = default;
    virtual ~CStaticBVH() = default;
    void Clear();
    void Build(const vector<Entry>& staticEntries);
    _bool Raycast(const _vector& vRayPos, const _vector& vRayDir, OUT HitResult& out, _float maxDist = FLT_MAX,OUT _int* Type = nullptr) const;


    #ifdef _DEBUG
public:
    struct DebugNodeInfo
    {
        AABB bounds;
        int left = -1;
        int right = -1;
        int entryIndex = -1;
    };

    void GetDebugNodes(std::vector<DebugNodeInfo>& out) const;
    void GetDebugEntryBounds(std::vector<AABB>& out) const;
#endif


private:
    _int BuildRecursive(_int start, _int end);
    void TraverseRay(_int nodeIdx, const _vector& vRayPos, const _vector& vRayDir, _float& closest,
                     OUT HitResult& out,
                     OUT _int* Type ) const;

private:
    AABB    MergeAABB(const AABB& a, const AABB& b);
    _bool   RayIntersectAABB(const _vector& vRayPos, const _vector& vRayDir, const AABB& box, _float tMax) const;
    _float3 Center(const AABB& a);
    _int    LongestAxis(const AABB& a);

private:
   vector<Entry> m_entries;
   vector<Node> m_nodes;
};

END