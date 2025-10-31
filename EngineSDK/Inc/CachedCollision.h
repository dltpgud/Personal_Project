#pragma once
#include "Base.h"
#include "GameObject.h"
BEGIN(Engine)

class CGameObject;
struct CollisionCacheKey
{
    CGameObject* pObjA;
    CGameObject* pObjB;

    _bool operator==(const CollisionCacheKey& other) const
    {
        return pObjA == other.pObjA && pObjB == other.pObjB;
    }

    struct HashFunction
    {
        size_t operator()(const CollisionCacheKey& key) const
        {
            return std::hash<CGameObject*>{}(key.pObjA) ^ std::hash<CGameObject*>{}(key.pObjB);
        }
    };
};

// 캐시된 충돌 결과
struct CachedCollisionResult
{
    _bool isColliding;
    _float3 collisionPos;
    _float3 collisionNormal;
    // 기타 충돌 결과 추가 가능
};

class CollisionCacheManager
{
private:
    unordered_map<CollisionCacheKey, CachedCollisionResult, CollisionCacheKey::HashFunction> m_CollisionCache;

public:
    // 캐시된 결과를 반환하거나 계산 후 저장
    _bool GetOrCompute(CGameObject* pObjA, CGameObject* pObjB,
                      function<_bool(CGameObject*, CGameObject*, CachedCollisionResult&)> computeCollision)
    {
        // 캐시 키 생성
        CollisionCacheKey key{pObjA, pObjB};

        // 캐시된 값이 있으면 반환
        auto it = m_CollisionCache.find(key);
        if (it != m_CollisionCache.end())
        {
            return true; // 캐시된 결과를 사용
        }

        // 캐시된 값이 없으면 계산하고 캐시 저장
        CachedCollisionResult result;
        bool isColliding = computeCollision(pObjA, pObjB, result);
        if (isColliding)
        {
            m_CollisionCache[key] = result;
        }

        return isColliding;
    }

    // 캐시 초기화 (프레임마다)
    void ClearCache()
    {
        m_CollisionCache.clear();
    }

  const  CachedCollisionResult& GetCache(CGameObject* pObjA, CGameObject* pObjB)
    {
        CollisionCacheKey key{pObjA, pObjB};

        // 캐시에서 값을 찾음
        auto it = m_CollisionCache.find(key);
        if (it != m_CollisionCache.end())
        {
            // 캐시된 값을 반환
            return it->second;
        }

        // 캐시된 값이 없으면 nullptr 반환
        return {};
    }
};
END