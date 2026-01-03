#include "GameInstance.h"
#include "Actor.h"
#include "Skill.h"

Collider_Manager::Collider_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pGameInstance{CGameInstance::GetInstance()}
#ifdef _DEBUG
      ,m_pDevice{pDevice}, m_pContext{pContext}
#endif
{
   Safe_AddRef(m_pGameInstance);

#ifdef _DEBUG
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

    if(FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
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
    }
;
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
    }

    return S_OK;
}

void Collider_Manager::All_Collison_check(_float fTimedelta)
{
  m_SpatialGrid.Clear();
  m_SpatialGrid.UpdateDynamicGrid(m_GameObjeList[COL_MONSTER]);
  m_SpatialGrid.UpdateDynamicGrid(m_GameObjeList[COL_MONSTER_SKILL]);
   
  MonsterSkill_To_Mash(fTimedelta); 
  Player_To_MonsterSkill();         
  Monster_To_Monster();
 
  Check_Inetrect_Player();
  Check_Collider_PlayerCollison();
 
  if (m_bIsColl)
  {
      if (false == PlayerWeapon_To_Monster())
      {
          PlayerWapon_To_Mash();
      }
      m_bIsColl = false;
  }
 
  Clear();
}

_bool Collider_Manager::PlayerWeapon_To_Monster()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();
    if (!pPlayer)
        return false;

    _vector RayPos{}, RayDir{};
    m_pGameInstance->Make_Ray(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                              m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir, true);
    
    vector<CGameObject*> vCandidates;
                 
    m_SpatialGrid.QueryAABB(pPlayer->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION),  RayPos + RayDir * pPlayer->Get_EffectiveRange(), 0, vCandidates);

    if (vCandidates.empty())
        return false;
    
    HitResult Hit;

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
            if (fDist > 0.f && fDist < Hit.distance)
            {
                Hit.distance = fDist;
                Hit.object = Obj;
                Hit.hit = true;
            }
        }
    }
    
    if (!Hit.hit)
        return false;
    
    CActor*pPickedObj = dynamic_cast<CActor*>(Hit.object);

    if (!pPickedObj)
        return false;


     
    if (pPickedObj->Part_Intersects(RayPos, RayDir, Hit.position, Hit.normal))
    {
         pPickedObj->Check_Coll();
         DECAL_DESC* Desc = pPlayer->Get_DecalDesc();
         Desc->vPos = Hit.position;
         Desc->vNormal = Hit.normal;
         Desc->vDir = XMVector3Normalize(RayDir);
         Desc->iType = DECAL_DESC::TYPE_BOX;

         m_pGameInstance->Trigger_Effect(Desc->Key, Desc);

         pPlayer->CreateEffect(RayPos, RayDir, Hit.position, Hit.normal);
         return true;
    }
    
     return false;
}

HRESULT Collider_Manager::Player_To_MonsterSkill()
{
   CActor* pPlayer = m_pGameInstance->Get_Player();
   if (!pPlayer)
       return E_FAIL;
 
   _vector playerPos = pPlayer->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
 
   vector<CGameObject*> vNearbySkills;
   _vector RayPos{}, RayDir{}, PrePos{}, CurPos{};
   _float  RayRen{};
 
   pPlayer->Get_Transform()->Get_Ray(RayPos, RayDir, &RayRen, &PrePos, &CurPos);
   m_SpatialGrid.QueryAABB(PrePos, CurPos, RayRen, vNearbySkills);

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
               else {}
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

HRESULT Collider_Manager::PlayerWapon_To_Mash()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();
    if (!pPlayer)
        return E_FAIL;

    _vector RayPos{}, RayDir{};
    m_pGameInstance->Make_Ray(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                              m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir, true);
    HitResult hit;

  
    DECAL_DESC* Desc = pPlayer->Get_DecalDesc();
    if (!m_StaticBVH.Raycast(RayPos, RayDir, hit))
        return E_FAIL;
    
    Desc->vPos = hit.position;
    Desc->vNormal = hit.normal;
    Desc->vDir = XMVector3Normalize(RayDir);
    Desc->iType = DECAL_DESC::TYPE_BOX;

    m_pGameInstance->Trigger_Effect(Desc->Key, Desc);
    pPlayer->CreateEffect(RayPos, RayDir, hit.position, hit.normal);

    return S_OK;
}

HRESULT Collider_Manager::MonsterSkill_To_Mash(_float fTimedelta)
{
    for (auto& SkillObj : m_GameObjeList[COL_MONSTER_SKILL])
    {
        _uint iSkillType = static_cast<CSkill*>(SkillObj)->Get_SkillType();
        CCollider* SkillCollider = SkillObj->Get_Collider();
        DECAL_DESC* Desc = static_cast<CSkill*>(SkillObj)->Get_DecalDesc();
        _bool bCreate = false;

        _vector RayPos{}, RayDir{},PrePos, CurPos;
        _float RayLen{};
        SkillObj->Get_Transform()->Get_Ray(RayPos, RayDir, &RayLen, &PrePos, &CurPos);

        _float radius = SkillCollider->Get_iCurRadius();

        if (iSkillType == CSkill::STYPE_LASER)
        {
            bCreate = true;
            RayPos = Desc->vPos;
            RayDir = Desc->vDir;
            radius *= 2.f;
        }
        _float MaxRayLen = RayLen + radius + 0.01f;
     
        HitResult hit;
        _int Type{};


        if (!m_StaticBVH.Raycast(RayPos, RayDir, hit, MaxRayLen, &Type))
            continue;

       
        if (hit.hit && iSkillType != CSkill::STYPE_SHOCKWAVE && iSkillType != CSkill::STYPE_LASER)
        {
            CCollider* HitCol = hit.object->Get_Collider();

            if (HitCol&& Type == CStaticBVH::EPrimType::ModelMesh)
            {  
                if (SkillCollider->Intersect(HitCol))
                {
                    bCreate = true;
                    static_cast<CSkill*>(SkillObj)->Dead_Rutine();
                }   
            }
            else // Terrain 후보
            {
                _float3 skillPos, hitPos;
                XMStoreFloat3(&skillPos, SkillObj->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
                XMStoreFloat3(&hitPos, hit.position);

                _float bottomY = skillPos.y - radius*2.f;
                if (bottomY <= hitPos.y + 0.01f)
                {
                    bCreate = true;
                    static_cast<CSkill*>(SkillObj)->Dead_Rutine();
                }
            }
        }

        if (hit.hit && true == bCreate && iSkillType != CSkill::STYPE_SHOCKWAVE)
        {
            Desc->iType = DECAL_DESC::TYPE_BOX;
            Desc->vNormal = hit.normal;
            Desc->vPos = hit.position;
            m_pGameInstance->Trigger_Effect(Desc->Key, Desc, iSkillType == CSkill::STYPE_LASER ? fTimedelta : 0.0f);
            static_cast<CSkill*>(SkillObj)->CreateEffect(RayPos, RayDir, hit.position);
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

HRESULT Collider_Manager::Init_World(const _float2& vMin, const _float2& vMax, _float cellSize)
{
    m_SpatialGrid.Clear();
    m_SpatialGrid.SetWorld(vMin, vMax,  cellSize);
  
    BuildStaticBVH();

    m_pGameInstance->Get_Player()->Set_onCell(true);
    m_pGameInstance->Get_Player()->Find_CurrentCell();

    for (auto& Monster : m_GameObjeList[COL_MONSTER])
    {
        dynamic_cast<CActor*>(Monster)->Find_CurrentCell();
        Safe_Release(Monster);
    }
    m_GameObjeList[COL_MONSTER].clear();

    return S_OK;
}

HRESULT Collider_Manager::Monster_To_Monster()
{
    auto& grid = m_SpatialGrid.DynamicGrid();
    if (grid.empty())
        return S_OK;

    const _float pushFactor = 0.3f;

    const int cellCount = static_cast<_int>(grid.size());
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

            _int ix, iz;
            if (!m_SpatialGrid.WorldToCell(posA, ix, iz))
                continue;

            _int neighborIdx[9];
            _int neighborCount = 0;
            m_SpatialGrid.GatherNeighborCells(ix, iz, neighborIdx, neighborCount);

            for (_int n = 0; n < neighborCount; ++n)
            {
                _int neighborCellIdx = neighborIdx[n];
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
    
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
   
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

    //BVH 디버그 라인
  // vector<CStaticBVH::DebugNodeInfo> nodes;
  // m_StaticBVH.GetDebugNodes(nodes);
  //
  // const XMVECTORF32 nodeColor = Colors::Yellow; // 노드
  // const XMVECTORF32 leafColor = Colors::Lime;   // leaf 강조
  //
  // for (const auto& n : nodes)
  // {
  //     if (n.entryIndex != -1)
  //         DrawAABBWire(m_pBatch, n.bounds, leafColor);
  //     else
  //         DrawAABBWire(m_pBatch, n.bounds, nodeColor);
  // }

    m_pBatch->End();
    
    return S_OK;
 }
 void Collider_Manager::DrawAABBWire(PrimitiveBatch<VertexPositionColor>* batch, const AABB& b,
                                     const XMVECTORF32& color)
 { // 8 corners
     const _vector p[8] = {
         XMVectorSet(b.min.x, b.min.y, b.min.z, 1.f), // 0
         XMVectorSet(b.max.x, b.min.y, b.min.z, 1.f), // 1
         XMVectorSet(b.max.x, b.max.y, b.min.z, 1.f), // 2
         XMVectorSet(b.min.x, b.max.y, b.min.z, 1.f), // 3
         XMVectorSet(b.min.x, b.min.y, b.max.z, 1.f), // 4
         XMVectorSet(b.max.x, b.min.y, b.max.z, 1.f), // 5
         XMVectorSet(b.max.x, b.max.y, b.max.z, 1.f), // 6
         XMVectorSet(b.min.x, b.max.y, b.max.z, 1.f), // 7
     };

     auto L = [&](int a, int c)
     { batch->DrawLine(VertexPositionColor(p[a], color), VertexPositionColor(p[c], color)); };

     // bottom
     L(0, 1);
     L(1, 2);
     L(2, 3);
     L(3, 0);
     // top
     L(4, 5);
     L(5, 6);
     L(6, 7);
     L(7, 4);
     // vertical
     L(0, 4);
     L(1, 5);
     L(2, 6);
     L(3, 7);
 }
#endif

void Collider_Manager::BuildStaticBVH()
{
    vector<CStaticBVH::Entry> vecEntries;
    vecEntries.reserve(m_GameObjeList[COL_STATIC].size());

    for (auto* obj : m_GameObjeList[COL_STATIC])
    {
        if (!obj)
            continue;

        CStaticBVH::Entry  Entrie;
        Entrie.object = obj;
        Entrie.transform = obj->Get_Transform(); 
        Entrie.col = obj->Get_Collider();
        Entrie.bounds = obj->Get_Collider()->Get_WorldAABB();
        if (auto* buf = static_cast<CVIBuffer_Terrain*>(obj->Find_Component(TEXT("Com_Buffer"))))
        {
            Entrie.type = CStaticBVH::EPrimType::Terrain;
            Entrie.terrain = buf;
        }
        else if (auto* model = dynamic_cast<CModel*>(obj->Find_Component(TEXT("Com_Model"))))
        {
            Entrie.type = CStaticBVH::EPrimType::ModelMesh;
            Entrie.model = model;
        }

        vecEntries.push_back(Entrie);
    }

    m_StaticBVH.Build(vecEntries);
}

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

    Safe_Release(m_pGameInstance);

#ifdef _DEBUG
    Safe_Delete(m_pBatch);
    Safe_Delete(m_pEffect);
    Safe_Release(m_pInputLayout);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
#endif
 
}
