#include "GameInstance.h"
#include "Actor.h"
#include "Skill.h"
#include "iostream"
Collider_Manager::Collider_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pGameInstance{CGameInstance::GetInstance()}
#ifdef _DEBUG
      ,m_pDevice{pDevice}, m_pContext{pContext}
#endif
{
    Safe_AddRef(m_pGameInstance);

#ifdef _DEBUG
    Safe_AddRef(m_pInputLayout);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
#endif

}

HRESULT Collider_Manager::Initialize()
{

#ifdef _DEBUG
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
    if (nullptr == m_pBatch)
        return E_FAIL;

    m_pEffect = new BasicEffect(m_pDevice);
    if (nullptr == m_pEffect)
        return E_FAIL;

    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCode = {nullptr};
    size_t iShaderByteCodeLength = {};

    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

    if(FAILED( m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
                                 pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
        return E_FAIL;

#endif

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

        Safe_Release(iter);
    }

    m_GameObjeList[COL_INTERECT].clear();

    return S_OK;
}

void Collider_Manager::All_Collison_check(_float fTimedelta)
{
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

   
    vector<CGameObject*> vCandidates;
    m_SpatialGrid.QueryNearby(RayPos, 150.f, vCandidates, COL_MONSTER); // 150.f는 시야 거리 반경

    if (vCandidates.empty())
        return false;

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
    m_SpatialGrid.QueryNearby(playerPos, 150.f, vNearbySkills, COL_MONSTER_SKILL);

    for (auto& pSkillObj : vNearbySkills)
    {
        CSkill* pSkill = dynamic_cast<CSkill*>(pSkillObj);
        if (!pSkill)
            continue;

        _bool bHit = true;
        
        if (CSkill::STYPE_SHOCKWAVE == dynamic_cast<CSkill*>(pSkill)->Get_SkillType())
        {
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

        _float fNearestDist = FLT_MAX;
    _vector vHitPos{}, vHitNormal{};
    CGameObject* pNearestObj = nullptr;
    _bool bTerrainHit = false;

    CGameObject* pTerrainObj = nullptr;
    CVIBuffer_Terrain* pTerrainBuffer = nullptr;


    _vector RayPos{}, RayDir{};
    m_pGameInstance->Make_Ray(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                              m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir, true);

    vector<CGameObject*> vCandidates;
    m_SpatialGrid.QueryNearby(RayPos, 200.f, vCandidates, COL_STATIC);

    for (auto& Obj : vCandidates)
    {
        if (!Obj)
            continue;
        auto* pBuf = dynamic_cast<CVIBuffer_Terrain*>(Obj->Find_Component(TEXT("Com_Buffer")));
        if (Obj&& pBuf)
        {
            if (!pTerrainBuffer)
            {
                pTerrainBuffer = pBuf;
                pTerrainObj = Obj;
            }
            else
            {
                _float fPrev = XMVectorGetX(XMVector3Length(RayPos - pTerrainObj->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION)));
                _float fCurr = XMVectorGetX(XMVector3Length(RayPos - Obj->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION)));
                if (fCurr < fPrev)
                {
                    pTerrainBuffer = pBuf;
                    pTerrainObj = Obj;
                }
            }
            continue;
        }

        _float fCurrDist = FLT_MAX;
        _vector vCurrPos{}, vCurrNormal{};
        _bool bHit = false;

        if (CCollider* pCol = Obj->Get_Collider())
        {
            if (pCol->RayIntersects(RayPos, RayDir, fCurrDist))
            {
                if (CModel* pModel = dynamic_cast<CModel*>(Obj->Find_Component(TEXT("Com_Model"))))
                {
                    _vector vExactPos{}, vExactNormal{};
                    if (pModel->RayIntersect(RayPos, RayDir, Obj->Get_Transform(), vExactPos, vExactNormal))
                    {
                        bHit = true;
                        fCurrDist = XMVectorGetX(XMVector3Length(vExactPos - RayPos));
                        vCurrPos = vExactPos;
                        vCurrNormal = vExactNormal;
                    }
                }
            }
        }
        else
        {
            if (CModel* pModel = dynamic_cast<CModel*>(Obj->Find_Component(TEXT("Com_Model"))))
            {
                _vector vExactPos{}, vExactNormal{};
                if (pModel->RayIntersect(RayPos, RayDir, Obj->Get_Transform(), vExactPos, vExactNormal))
                {
                    bHit = true;
                    fCurrDist = XMVectorGetX(XMVector3Length(vExactPos - RayPos));
                    vCurrPos = vExactPos;
                    vCurrNormal = vExactNormal;
                }
            }
        }

        if (bHit && fCurrDist < fNearestDist)
        {
            fNearestDist = fCurrDist;
            vHitPos = vCurrPos;
            vHitNormal = vCurrNormal;
            pNearestObj = Obj;
            bTerrainHit = false;
        }
    }

    if (pTerrainBuffer)
    {
        _float3 fPos{}, fNormal{};
        _float fDist{};
        if (pTerrainBuffer->Picking_OnTerrain_QuadTree(RayPos, RayDir, pTerrainObj->Get_Transform(), &fDist, &fNormal,
                                                       &fPos))
        {
            if (fDist < fNearestDist)
            {
                fNearestDist = fDist;
                vHitPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
                vHitNormal = XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f);
                bTerrainHit = true;
                pNearestObj = pTerrainObj;
            }
        }
    }

    if (!pNearestObj)
        return S_OK;

    DECAL_DESC* Desc = pPlayer->Get_DecalDesc();
    Desc->vPos = vHitPos;
    Desc->vNormal = vHitNormal;
    Desc->vDir = XMVector3Normalize(RayDir);
    Desc->iType = DECAL_DESC::TYPE_BOX;

    m_pGameInstance->Trigger_Effect(Desc->Key, Desc);
    pPlayer->CreateEffect(RayPos, RayDir, vHitPos, vHitNormal);

    return S_OK;
}


HRESULT Collider_Manager::MonsterSkill_To_Mash_Collison(_float fTimedelta)
{
    vector<CGameObject*> vCandidates;
    for (auto& SkillObj : m_GameObjeList[COL_MONSTER_SKILL])
    {
        _uint iActorType = static_cast<CSkill*>(SkillObj)->Get_ActorType();
        _uint iSkillType = static_cast<CSkill*>(SkillObj)->Get_SkillType();
        _vector SkillPos = SkillObj->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

        m_SpatialGrid.QueryNearby(SkillPos, 1.f, vCandidates, COL_STATIC);

        // 3. 쿼리된 객체들에 대해 충돌 검사
        for (auto& DecalObj : vCandidates)
        {
            if (nullptr == DecalObj)
                continue;

            CCollider* pCol = DecalObj->Get_Collider();
            if (nullptr == pCol && iSkillType != CSkill::STYPE_LASER && iSkillType != CSkill::STYPE_SHOCKWAVE)
            {
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
                        DECAL_DESC* Desc = static_cast<CSkill*>(SkillObj)->Get_DecalDesc();
                        Desc->iType = DECAL_DESC::TYPE_BOX;
                        Desc->vNormal = vNormal;
                        Desc->vPos = vWorldPos;
                        m_pGameInstance->Trigger_Effect(Desc->Key, Desc);
                        static_cast<CSkill*>(SkillObj)->CreateEffect(XMVectorZero(), XMVectorZero(), vWorldPos);
                        static_cast<CSkill*>(SkillObj)->Dead_Rutine();
                        
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

                        _float3 fRayDir = {XMVectorGetX(RayDir), XMVectorGetY(RayDir), XMVectorGetX(RayDir)};
                        
                        if (fRayDir.x != 0 && fRayDir.y != 0 && fRayDir.z != 0)
                        {
                            if (pModel->RayIntersect(RayPos, RayDir, DecalObj->Get_Transform(), vExactHit,
                                                     vNormalExact))
                            {
                              DECAL_DESC* Desc = static_cast<CSkill*>(SkillObj)->Get_DecalDesc();
                              Desc->iType = DECAL_DESC::TYPE_BOX;
                              Desc->vNormal = vNormalExact;
                              Desc->vPos = vExactHit;
                              m_pGameInstance->Trigger_Effect(Desc->Key, Desc);
                              static_cast<CSkill*>(SkillObj)->CreateEffect(RayPos, RayDir, vExactHit);
                              static_cast<CSkill*>(SkillObj)->Dead_Rutine();
                          
                              continue;
                            }
                        }
                    }
                }
            }

            if (iActorType == CSkill::BOSS_MONSTER)
            {
               DECAL_DESC* Desc = static_cast<CSkill*>(SkillObj)->Get_DecalDesc();
               if (Desc->bOnce && Desc->bActive)
                   continue;
               CCollider* DecalCol = DecalObj->Get_Collider();

               if (iSkillType != CSkill::STYPE_SHOCKWAVE)
               {
                   if (DecalCol && SkillObj->Get_Collider()->Intersect(DecalCol))
                   {
                       Desc->iType = DECAL_DESC::TYPE_SSD;
                       m_pGameInstance->Trigger_Effect(Desc->Key, Desc);
                   }
                   if (nullptr == DecalCol)
                   {
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
                               m_pGameInstance->Trigger_Effect(Desc->Key, Desc, fTimedelta);
                           }
                       }
                   }
               }
            }
        }
    }

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

HRESULT Collider_Manager::Find_Cell(const _float2& vMin, const _float2& vMax, _float cellSize)
{
    m_SpatialGrid.SetWorld(vMin, vMax,  cellSize);

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

    const int cellCount = static_cast<int>(grid.size());
    for (int i = 0; i < cellCount; ++i)
    {
        auto& cell = grid[i];
        if (cell.Obj.empty())
            continue;

        const size_t count = cell.Obj.size();
        for (size_t a = 0; a < count; ++a)
        {
            if (!cell.Obj[a])
                continue;
         
            CActor* pA = dynamic_cast<CActor*>(cell.Obj[a]);
            if (!pA)
                continue;

            CCollider* pACol = pA->Get_Collider();
            CTransform* pATrans = pA->Get_Transform();
            if (!pACol || !pATrans)
                continue;

            _vector posA = pATrans->Get_TRANSFORM(CTransform::T_POSITION);
            const _float radiusA = pACol->Get_iCurRadius();

            for (size_t b = a + 1; b < count; ++b)
            {
                CActor* pB = dynamic_cast<CActor*>(cell.Obj[b]);
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
                if (neighbor.Obj.empty())
                    continue;

                for (auto* objB : neighbor.Obj)
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

#ifdef _DEBUG
HRESULT Collider_Manager::Render()
{
    if ( nullptr == m_pEffect)
        return E_FAIL;

    // 쉐이더에 행렬 값 던져 주고
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

    // 정점들을 장치에 바인딩해준다.
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pEffect->Apply(m_pContext);

    m_pBatch->Begin();

    const auto worldMin = m_SpatialGrid.GetWorldMin();
    const auto worldMax = m_SpatialGrid.GetWorldMax();
    const _float cellSize = m_SpatialGrid.GetCellSize();

    const _float y = 0.1f;
    const XMVECTORF32 color = Colors::AliceBlue;

    // 세로선
    for (float x = worldMin.x; x <= worldMax.x; x += cellSize)
    {
        m_pBatch->DrawLine(VertexPositionColor(XMVectorSet(x, y, worldMin.y,1.f), color),
                           VertexPositionColor(XMVectorSet(x, y, worldMax.y,1.f), color));
    }

    // 가로선
    for (float z = worldMin.y; z <= worldMax.y; z += cellSize)
    {
        m_pBatch->DrawLine(VertexPositionColor(XMVectorSet(worldMin.x, y, z,1.f), color),
                           VertexPositionColor(XMVectorSet(worldMax.x, y, z,1.f), color));
    }
    m_pBatch->End();

    return S_OK;
}

#endif

Collider_Manager* Collider_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    Collider_Manager* pInstance = new Collider_Manager(pDevice, pContext);

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

#ifdef _DEBUG
    Safe_Delete(m_pBatch);
    Safe_Delete(m_pEffect);
    Safe_Release(m_pInputLayout);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
#endif
    Safe_Release(m_pGameInstance);
}
