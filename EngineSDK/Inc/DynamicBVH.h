#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CGameObject;
class CCollider;
 class CDynamicBVH : public CBase
 {
 public:
     struct Entry
     {
         CGameObject* object = nullptr; // 동적 오브젝트
         CCollider* collider = nullptr; // 오브젝트의 충돌체
         AABB bounds;                   // 오브젝트의 AABB
         bool isActive = true;          // 오브젝트가 활성 상태인지 여부
     };

     struct Node
     {
         AABB bounds;         // 이 노드의 AABB
         int entryIndex = -1; // 해당 노드가 가리키는 Entry의 인덱스
         int left = -1;       // 왼쪽 자식 노드 인덱스
         int right = -1;      // 오른쪽 자식 노드 인덱스
         int parentIdx = -1;  // 부모 노드 인덱스
     };

 private:
     std::vector<Entry> m_entries; // 동적 오브젝트 목록
     std::vector<Node> m_nodes;    // BVH 트리 노드들

 public:
     // 초기화 및 데이터 삭제
     void Clear();

     // 동적 오브젝트들로 BVH 트리 생성
     void Build(const std::vector<Entry>& dynamicEntries);

     // 동적 오브젝트들의 AABB 갱신
     void UpdateDynamicObjects();

     // AABB와의 충돌 검사
     bool Raycast(const _vector& vRayPos, const _vector& vRayDir, HitResult& out, _float maxDist = FLT_MAX) const;
     void TraverseRay(int nodeIdx, const _vector& vRayPos, const _vector& vRayDir, float& closest,
                      HitResult& out) const;
     _int BuildRecursive(_int start, _int end, _int parentIdx);
     // 동적 오브젝트의 AABB 갱신된 노드의 경계 상자 갱신
     void UpdateNodeBounds(const Entry& entry);
     _bool RayIntersectAABB(const _vector& vRayPos, const _vector& vRayDir, const AABB& box, _float tMax) const;
     // AABB 병합
     static AABB MergeAABB(const AABB& a, const AABB& b);

     // AABB의 교차 여부 확인

     bool OverlapAABB(const AABB& a, const AABB& b) const;

     // AABB의 중심 계산
     static _float3 Center(const AABB& a);

     // 가장 긴 축을 찾는 함수
     static _int LongestAxis(const AABB& a);

     // 동적 오브젝트 추가
     //void AddDynamicObject(CGameObject* obj);
 };

END