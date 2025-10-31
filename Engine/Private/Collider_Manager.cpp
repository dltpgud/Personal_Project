#include "GameInstance.h"
#include "Actor.h"
#include "Skill.h"
#include "iostream"
Collider_Manager::Collider_Manager() : m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT Collider_Manager::Initialize()
{
    m_SpatialGrid.SetWorld({-300.f, -300.f}, {600.f, 600.f}, 10);
    return S_OK;
}

HRESULT Collider_Manager::Add_Collider(_int Damage, CCollider* Collider)
{
    m_ColliderDamage = Damage;
    m_ColliderList.push_back(Collider);
    Safe_AddRef(Collider);
    return S_OK;
}

HRESULT Collider_Manager::Add_GameObject_To_ColGroup(class CGameObject* Obj, const _uint& Type)
{
    m_GameObjeList[Type].push_back(Obj);
    Safe_AddRef(Obj);
    return S_OK;
}

HRESULT Collider_Manager::Check_Collider_PlayerCollison()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();
    if (nullptr == pPlayer)
        return E_FAIL;

    if (0 == m_ColliderList.size())
        return S_OK;

    for (auto& Collider : m_ColliderList)
    {
        if (nullptr == Collider)
            continue;

        if (true == Collider->Intersect(pPlayer->Get_Collider()))
        {
            pPlayer->Check_Coll();
            pPlayer->Set_CurrentHP(m_ColliderDamage);
        }
        Safe_Release(Collider);
    }
    m_ColliderList.clear();
    return S_OK;
}

HRESULT Collider_Manager::Check_Inetrect_Player()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();

    if (nullptr == pPlayer)
        return E_FAIL;

    if (0 == m_GameObjeList[COL_INTERECT].size())
        return S_OK;

    for (auto& iter : m_GameObjeList[COL_INTERECT])
    {
        if (nullptr == iter)
            continue;

        iter->Get_Collider()->CollUpdate(pPlayer);

        for (auto& monster : m_GameObjeList[COL_MONSTER])
        {
            iter->Get_Collider()->CollUpdate(dynamic_cast<CActor*>(monster));
        }

        Safe_Release(iter);
    }

    m_GameObjeList[COL_INTERECT].clear();

    return S_OK;
}

void Collider_Manager::All_Collison_check(_float fTimedelta)
{
    m_ModelCache.clear(); 
    m_SpatialGrid.ClearDynamic();
    m_SpatialGrid.UpdateDynamicGrid(m_GameObjeList[COL_MONSTER]);
    m_SpatialGrid.UpdateDynamicGrid(m_GameObjeList[COL_MONSTER_SKILL]);
    
    MonsterSkill_To_Mash_Collison(fTimedelta);
    Monster_To_Monster_Collision();
    Check_Collider_PlayerCollison();

    Player_To_Monster_Bullet_Collison();
    Check_Inetrect_Player();
    if (m_bIsColl)
    {
        if (false == Player_To_Monster_Ray_Collison_Check())
        {
            Player_To_Mash_Collison_for_Decal();
        }
        m_bIsColl = false;
    }

    for (auto& coll : m_ColliderList) Safe_Release(coll);
    m_ColliderList.clear();

    for (_int i = 0; i < COL_END; i++)
    {
        if (i != COL_STATIC)
        {

            for (auto& Obj : m_GameObjeList[i]) Safe_Release(Obj);

            m_GameObjeList[i].clear();
        }
    }

}

_bool Collider_Manager::Player_To_Monster_Ray_Collison_Check()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();
    if (!pPlayer)
        return false;

    _vector RayPos{}, RayDir{};
    m_pGameInstance->Make_Ray(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                              m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir, true);

    // 1️⃣ 공간분할 쿼리: Ray 근처 몬스터만 후보로
    vector<CGameObject*> vCandidates;
    m_SpatialGrid.QueryNearby(RayPos, 150.f, vCandidates, COL_MONSTER); // 150.f는 시야 거리 반경

    if (vCandidates.empty())
        return false;

    // 2️⃣ Ray와의 충돌 후보 찾기
    CActor* pPickedObj = nullptr;
    _float fBestDist = FLT_MAX;

    for (auto& Obj : vCandidates)
    {
        if (!Obj)
            continue;

        CCollider* pCol = Obj->Get_Collider();
        if (!pCol)
            continue;

        _float fDist{};
        if (pCol->RayIntersects(RayPos, RayDir, fDist))
        {
            if (fDist > 0.f && fDist < fBestDist)
            {
                fBestDist = fDist;
                pPickedObj = dynamic_cast<CActor*>(Obj);
            }
        }
    }

    // 3️⃣ 실제 피킹된 몬스터가 없다면 종료
    if (!pPickedObj)
        return false;

    // 4️⃣ 정밀 피킹: 본/메시 단위 충돌 확인
    _vector FinalPos{}, vNormal{};
    if (pPickedObj->Part_Intersects(RayPos, RayDir, FinalPos, vNormal))
    {
        pPickedObj->Check_Coll();
        pPlayer->CreateEffect(RayPos, RayDir, FinalPos, vNormal);
        return true;
    }

    return false;
}

HRESULT Collider_Manager::Player_To_Monster_Bullet_Collison()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();
    if (!pPlayer)
        return E_FAIL;

    _vector playerPos = pPlayer->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

    vector<CGameObject*> vNearbySkills;
    m_SpatialGrid.QueryNearby(playerPos, 1.f, vNearbySkills, COL_MONSTER_SKILL);

    for (auto& pSkillObj : vNearbySkills)
    {
        CSkill* pSkill = dynamic_cast<CSkill*>(pSkillObj);
        if (!pSkill)
            continue;

        _bool bHit = true;
        
        if (CSkill::STYPE_SHOCKWAVE == dynamic_cast<CSkill*>(pSkill)->Get_SkillType())
        {
            cout << vNearbySkills.size() << endl;
            if (true == dynamic_cast<CSkill*>(pSkill)->Comput_SafeZone(pPlayer->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION)))
            {
                bHit = false;
            }
            else
            {
                if (false == pPlayer->Get_onCell())
                    bHit = false;
                else
                    bHit = true;
            }
        }

        if (true == bHit)
        {
            if (true == pSkill->Get_Collider()->Intersect(pPlayer->Get_Collider()))
            {
                pPlayer->Set_CurrentHP(dynamic_cast<CSkill*>(pSkill)->Get_Damage());

                if (dynamic_cast<CSkill*>(pSkill)->Get_SkillType() == CSkill::STYPE_STURN)
                    pPlayer->Stun_Routine();
                else
                    pPlayer->Check_Coll();

                if (dynamic_cast<CSkill*>(pSkill)->Get_ActorType() != CSkill::BOSS_MONSTER)
                {
                    static_cast<CSkill*>(pSkill)->Dead_Rutine();
                }
            }
        }
    }

    return S_OK;
}

HRESULT Collider_Manager::Player_To_Mash_Collison_for_Decal()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();
    if (!pPlayer)
        return E_FAIL;

    _vector RayPos{}, RayDir{};
    m_pGameInstance->Make_Ray(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                              m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir, true);

    vector<CGameObject*> vCandidates;
    m_SpatialGrid.QueryNearby(RayPos, 200.f, vCandidates,COL_STATIC);

    _float fBestDist = FLT_MAX;
    _float fTerrainDist = FLT_MAX;
    _vector vHitPos{}, vHitNormal{};
    _float3 fTerrainPos{}, fTerrainNormal{};
    CGameObject* pHitObj = nullptr;
    CGameObject* pTerrainObj = nullptr;

    // --- Terrain 피킹 캐싱: Terrain당 1회 ---

    for (auto& Obj : vCandidates)
    {
        if (!Obj)
            continue;

        // --- Collider 없는 경우: Terrain 검사 ---
        CCollider* pCol = Obj->Get_Collider();
        if (!pCol)
        {
            auto* pTerrain = dynamic_cast<CVIBuffer_Terrain*>(Obj->Find_Component(TEXT("Com_Buffer")));
            if (pTerrain)
            {
                // 캐시 확인
                if (m_ModelCache.find(pTerrain) == m_ModelCache.end())
                {
                    _float3 fPos{}, fNormal{};
                    _float fDist{};
                    if (pTerrain->Picking_OnTerrain_QuadTree(RayPos, RayDir, Obj->Get_Transform(), &fDist, &fNormal,&fPos))
                    {
                        // 유효한 피킹만 캐시 저장
                        if (fDist > 0.f && fDist < FLT_MAX)
                            m_ModelCache[pTerrain] = {fPos, fNormal};
                    }
                }

                // 캐시에서 최적 결과 갱신
                auto it = m_ModelCache.find(pTerrain);
                if (it != m_ModelCache.end())
                {
                    _float3 fPos = it->second.first;
                    _float3 fNormal = it->second.second;

                    _float fDist = XMVectorGetX(XMVector3Length(XMVectorSubtract(RayPos, XMLoadFloat3(&fPos))));

                    if (fDist < fTerrainDist)
                    {
                        fTerrainDist = fDist;
                        fTerrainPos = fPos;
                        fTerrainNormal = fNormal;
                        pTerrainObj = Obj;
                    }
                }
            }
            continue;
        }

        // --- Collider 있는 오브젝트 (일반 피킹) ---
        _float fDist{};
        if (pCol->RayIntersects(RayPos, RayDir, fDist))
        {
            if (fDist < fBestDist)
            {
                fBestDist = fDist;
                pHitObj = Obj;
            }
        }
    }

    // --- 모델 정밀 피킹 ---
    _bool bHit = false;
    _bool bTerrainHit = false;

    while (pHitObj)
    {
        CModel* pModel = dynamic_cast<CModel*>(pHitObj->Find_Component(TEXT("Com_Model")));
        if (pModel)
        {
            _vector vExactHit{}, vNormalExact{};
            if (pModel->RayIntersect(RayPos, RayDir, pHitObj->Get_Transform(), vExactHit, vNormalExact))
            {
                vHitPos = vExactHit;
                vHitNormal = vNormalExact;
                bHit = true;
                break;
            }
        }

        _float nextBestDist = FLT_MAX;
        CGameObject* pNextObj = nullptr;

        for (auto& Next : vCandidates)
        {
            if (!Next || Next == pHitObj)
                continue;

            CCollider* pNextCol = Next->Get_Collider();
            if (!pNextCol)
                continue;

            _float fDist{};
            if (pNextCol->RayIntersects(RayPos, RayDir, fDist))
            {
                if (fDist < nextBestDist && fDist > fBestDist)
                {
                    nextBestDist = fDist;
                    pNextObj = Next;
                }
            }
        }

        if (!pNextObj)
            break;

        pHitObj = pNextObj;
        fBestDist = nextBestDist;
    }

    // --- Terrain 피킹 (모델 실패 시) ---
    if (!bHit && pTerrainObj)
    {
        bTerrainHit = true;
        vHitPos = XMVectorSet(fTerrainPos.x, fTerrainPos.y, fTerrainPos.z, 1.f);
        vHitNormal = XMVectorSet(fTerrainNormal.x, fTerrainNormal.y, fTerrainNormal.z, 0.f);
    }

    if (!bHit && !bTerrainHit)
        return S_OK;

    // --- 데칼 생성 ---
    DECAL_DESC* Desc = pPlayer->Get_DecalDesc();
    Desc->vPos = vHitPos;
    Desc->vNormal = vHitNormal;
    Desc->vDir = XMVector3Normalize(RayDir);
    Desc->iType = DECAL_DESC::TYPE_BOX;

    m_pGameInstance->Add_Decal(Desc->Key, Desc);
    pPlayer->CreateEffect(RayPos, RayDir, vHitPos, vHitNormal);

    return S_OK;
}
HRESULT Collider_Manager::MonsterSkill_To_Mash_Collison(_float fTimedelta)
{
    // 1. 공간 분할 쿼리: SkillObj와 가까운 Static 객체들만 대상으로 충돌 체크
    vector<CGameObject*> vCandidates;
    for (auto& SkillObj : m_GameObjeList[COL_MONSTER_SKILL])
    {
        _uint iActorType = static_cast<CSkill*>(SkillObj)->Get_ActorType();
        _uint iSkillType = static_cast<CSkill*>(SkillObj)->Get_SkillType();
        _vector SkillPos = SkillObj->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

        // 2. SkillObj 위치 근처의 Static 객체들만 쿼리 (범위: 150.f)
        m_SpatialGrid.QueryNearby(SkillPos, 1.f, vCandidates, COL_STATIC);

        // 3. 쿼리된 객체들에 대해 충돌 검사
        for (auto& DecalObj : vCandidates)
        {
            if (nullptr == DecalObj)
                continue;

            CCollider* pCol = DecalObj->Get_Collider();
            if (nullptr == pCol && iSkillType != CSkill::STYPE_LASER && iSkillType != CSkill::STYPE_SHOCKWAVE)
            {
                // Collider가 없으면 Terrain 검사
                CVIBuffer_Terrain* buffer =
                    static_cast<CVIBuffer_Terrain*>(DecalObj->Find_Component(TEXT("Com_Buffer")));
                _vector vNormal{}, vWorldPos{};

                if (buffer)
                {
                    _float3 fPos{}, fNormal{};
                    _float fDist{};
                    if (buffer->Intersect_OnTerrain_QuadTree(SkillObj->Get_Collider(), DecalObj->Get_Transform(),
                                                             &vNormal, &vWorldPos))
                    {
                        static_cast<CSkill*>(SkillObj)->Dead_Rutine();
                        static_cast<CSkill*>(SkillObj)->CreateEffect(XMVectorZero(), XMVectorZero(), vWorldPos,
                                                                     vNormal);
                        continue;
                    }
                }
            }
            else if (iSkillType != CSkill::STYPE_LASER && iSkillType != CSkill::STYPE_SHOCKWAVE)
            {
                // Collider가 있을 경우 충돌 검사
                if (pCol && SkillObj->Get_Collider()->Intersect(pCol))
                {
                    CModel* pModel = dynamic_cast<CModel*>(DecalObj->Find_Component(TEXT("Com_Model")));
                    if (pModel)
                    {
                        _vector vExactHit{}, vNormalExact{};
                        _vector RayPos{}, RayDir{};
                        SkillObj->Get_Transform()->Get_Ray(RayPos, RayDir);

                        if (pModel->RayIntersect(RayPos, RayDir, DecalObj->Get_Transform(), vExactHit, vNormalExact))
                        {
                            static_cast<CSkill*>(SkillObj)->Dead_Rutine();
                            continue;
                        }
                    }
                }
            }

            if (iActorType == CSkill::BOSS_MONSTER)
            {
                    // 데칼 생성 처리
               DECAL_DESC* Desc = static_cast<CSkill*>(SkillObj)->Get_DecalDesc();
               if (Desc->bOnce && Desc->bActive)
                   continue;
               CCollider* DecalCol = DecalObj->Get_Collider();
               if (DecalCol && SkillObj->Get_Collider()->Intersect(DecalCol))
               {
                   Desc->iType = DECAL_DESC::TYPE_SSD;
                   m_pGameInstance->Trigger_Effect(Desc->Key, Desc);
               }
               if (nullptr == DecalCol)
               {
                   // Terrain에 대한 데칼 위치 계산
                   _float3 fNormal = {};
                   _float3 fPos = {0.f, 0.f, 0.f};
                   _float fTerrainDist{};
                   _float3 fDir{};
                   XMStoreFloat3(&fDir, Desc->vDir);
                   if (fDir.x == 0.f && fDir.y == 0.f && fDir.z == 0.f)
                       Desc->vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
                   CVIBuffer_Terrain* buffer =
                       static_cast<CVIBuffer_Terrain*>(DecalObj->Find_Component(TEXT("Com_Buffer")));
                   if (iSkillType == CSkill::STYPE_LASER)
                   {
                       if (buffer->Picking_OnTerrain_QuadTree(Desc->vPos, XMVector3Normalize(Desc->vDir),
                                                              DecalObj->Get_Transform(), &fTerrainDist, &fNormal,
                                                              &fPos))
                       {
                           Desc->vNormal = XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f);
                           Desc->vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 0.f);
                           Desc->iType = DECAL_DESC::TYPE_BOX;
                           m_pGameInstance->Add_Decal(Desc->Key, Desc, fTimedelta);
                       }
                   }
               }
            }
        }
    }

    if (m_pGameInstance->Get_DIKeyDown(DIK_1))
        cout << iTime << endl;
    return S_OK;
}

void Collider_Manager::Clear()
{
    for (auto& coll : m_ColliderList) Safe_Release(coll);
    m_ColliderList.clear();

    for (_int i = 0; i < COL_END; i++)
    {
        for (auto& Obj : m_GameObjeList[i]) Safe_Release(Obj);
        m_GameObjeList[i].clear();
    }
}

HRESULT Collider_Manager::Find_Cell()
{
    m_pGameInstance->Get_Player()->Set_onCell(true);
    m_pGameInstance->Get_Player()->Find_CurrentCell();

    for (auto& Monster : m_GameObjeList[COL_MONSTER])
    {
        dynamic_cast<CActor*>(Monster)->Find_CurrentCell();
        Safe_Release(Monster);
    }
    m_GameObjeList[COL_MONSTER].clear();

    m_SpatialGrid.Clear();
    m_SpatialGrid.BuildStaticGrid(m_GameObjeList[COL_STATIC]);
    return S_OK;
}

HRESULT Collider_Manager::Monster_To_Monster_Collision()
{
    auto& grid = m_SpatialGrid.DynamicGrid();
    if (grid.empty())
        return S_OK;

    const _float pushFactor = 0.3f;

    // 셀 인덱스 순서대로만 검사 → 중복 제거
    const int cellCount = static_cast<int>(grid.size());
    for (int i = 0; i < cellCount; ++i)
    {
        auto& cell = grid[i];
        if (cell.dynamics.empty())
            continue;

        // --- 같은 셀 내 충돌 검사 ---
        const size_t count = cell.dynamics.size();
        for (size_t a = 0; a < count; ++a)
        {
            if (!cell.dynamics[a])
                continue;
         
            CActor* pA = dynamic_cast<CActor*>(cell.dynamics[a]);
            if (!pA)
                continue;

            CCollider* pACol = pA->Get_Collider();
            CTransform* pATrans = pA->Get_Transform();
            if (!pACol || !pATrans)
                continue;

            _vector posA = pATrans->Get_TRANSFORM(CTransform::T_POSITION);
            const _float radiusA = pACol->Get_iCurRadius();

            // --- 같은 셀 내부 ---
            for (size_t b = a + 1; b < count; ++b)
            {
                CActor* pB = dynamic_cast<CActor*>(cell.dynamics[b]);
                if (!pB)
                    continue;

                CCollider* pBCol = pB->Get_Collider();
                CTransform* pBTrans = pB->Get_Transform();
                if (!pBCol || !pBTrans)
                    continue;

                _vector posB = pBTrans->Get_TRANSFORM(CTransform::T_POSITION);
                _vector diff = posB - posA;
                diff = XMVectorSetY(diff, 0.f);

                _float distSq = XMVectorGetX(XMVector3LengthSq(diff));
                const _float minDist = radiusA + pBCol->Get_iCurRadius();
                const _float minDistSq = minDist * minDist;

                // 너무 멀면 검사 스킵
                if (distSq > minDistSq)
                    continue;

                _float dist = sqrtf(distSq);
                if (dist < 0.001f)
                {
                    diff = XMVectorSet((rand() % 100) / 100.f + 0.1f, 0.f, (rand() % 100) / 100.f + 0.1f, 0.f);
                    dist = XMVectorGetX(XMVector3Length(diff));
                }

                const _float overlap = (minDist - dist) * pushFactor;
                const _vector dir = XMVector3Normalize(diff);

                pATrans->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSetW(posA - dir * overlap, 1.f));
                pBTrans->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSetW(posB + dir * overlap, 1.f));
            }

            // --- 인접 셀 충돌 검사 ---
            int ix, iz;
            if (!m_SpatialGrid.WorldToCell(posA, ix, iz))
                continue;

            int neighborIdx[9];
            int neighborCount = 0;
            m_SpatialGrid.GatherNeighborCells(ix, iz, neighborIdx, neighborCount);

            for (int n = 0; n < neighborCount; ++n)
            {
                int neighborCellIdx = neighborIdx[n];
                if (neighborCellIdx <= i)
                    continue; // 이미 처리된 셀은 스킵 (중복 제거)

                auto& neighbor = grid[neighborCellIdx];
                if (neighbor.dynamics.empty())
                    continue;

                for (auto* objB : neighbor.dynamics)
                {
                    CActor* pB = dynamic_cast<CActor*>(objB);
                    if (!pB)
                        continue;

                    CCollider* pBCol = pB->Get_Collider();
                    CTransform* pBTrans = pB->Get_Transform();
                    if (!pBCol || !pBTrans)
                        continue;

                    _vector posB = pBTrans->Get_TRANSFORM(CTransform::T_POSITION);
                    _vector diff = posB - posA;
                    diff = XMVectorSetY(diff, 0.f);

                    _float distSq = XMVectorGetX(XMVector3LengthSq(diff));
                    const _float minDist = radiusA + pBCol->Get_iCurRadius();
                    const _float minDistSq = minDist * minDist;

                    if (distSq > minDistSq)
                        continue;

                    _float dist = sqrtf(distSq);
                    const _float overlap = (minDist - dist) * pushFactor;
                    const _vector dir = XMVector3Normalize(diff);

                    pATrans->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSetW(posA - dir * overlap, 1.f));
                    pBTrans->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSetW(posB + dir * overlap, 1.f));
                }
            }
        }
    }

    return S_OK;
}


Collider_Manager* Collider_Manager::Create()
{
    Collider_Manager* pInstance = new Collider_Manager();

    if (FAILED(pInstance->Initialize()))
    {

        MSG_BOX("Failed to Created : Collider_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Collider_Manager::Free()
{
    __super::Free();

    for (auto& coll : m_ColliderList) Safe_Release(coll);
    m_ColliderList.clear();

    for (_int i = 0; i < COL_END; i++)
    {
        for (auto& Obj : m_GameObjeList[i]) Safe_Release(Obj);
        m_GameObjeList[i].clear();
    }
    Safe_Release(m_pGameInstance);
}
