#pragma once
#include "Base.h"
#include "OctreeNode.h"
BEGIN(Engine)
class CGameObject;
class COctreePartition
{
public:
    COctreePartition();
    ~COctreePartition();

    // 프레임 시작할 때 한 번 부른다.
    // rootBounds는 월드 전체 범위 (맵 전체 커버하도록 크게)
    HRESULT BeginFrame(const AABB& rootBounds, _uint maxDepth, _uint maxObjectsPerNode);

    // 옥트리에 넣을 객체 등록
    // Collider_Manager에서 m_GameObjeList 돌면서 매 프레임 호출
    void AddObject(class CGameObject* pObj, _uint colType);

    // 모든 AddObject가 끝난 다음 Build() 호출
    void Build();

    // 쿼리
    void QueryAroundPointAABB(const _float3& center, _float halfSize, vector<CGameObject*>& outObjects) const;

    void QueryRay(const _float3& origin, const _float3& dir, _float maxDist,
                  vector<CGameObject*>& outObjects) const;

    // 프레임 끝나면 EndFrame()으로 정리
    void EndFrame();

private:
    // 내부적으로 AddObject할 때 임시로 쌓아둘 리스트
    vector<OctreeEntry> m_pending;

    COctreeNode* m_root = nullptr;
    _uint m_maxDepth = 0;
    _uint m_maxObjectsPerNode = 0;

    // 헬퍼: pObj에서 월드 AABB 뽑기
    OctreeEntry MakeEntryFromObject(class CGameObject* pObj, unsigned int colType) const;
};


END
