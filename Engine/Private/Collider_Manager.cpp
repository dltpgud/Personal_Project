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
        { iter->Get_Collider()->CollUpdate(dynamic_cast<CActor*>(monster)); }
        
        Safe_Release(iter);
    }

     m_GameObjeList[COL_INTERECT].clear();

    return S_OK;
}

void Collider_Manager::All_Collison_check()
{
   // m_SpatialGrid.Rebuild(m_GameObjeList[COL_DECAL], m_GameObjeList[COL_MONSTER], m_GameObjeList[COL_MONSTER_SKILL]);

    Boss_To_Mash_Collison();
    //Monster_To_Monster_Collision();
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

    Monster_To_Mash_Collison_for_Decal();

    for (auto& Monster : m_GameObjeList[COL_MONSTER]) Safe_Release(Monster);
    m_GameObjeList[COL_MONSTER].clear();
    for (auto& Skill : m_GameObjeList[COL_DECAL]) Safe_Release(Skill);
    m_GameObjeList[COL_DECAL].clear();
    for (auto& Skill : m_GameObjeList[COL_MONSTER_SKILL]) Safe_Release(Skill);
     m_GameObjeList[COL_MONSTER_SKILL].clear();

}

_bool Collider_Manager::Player_To_Monster_Ray_Collison_Check()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();
    if (false == pPlayer)
        return false;
   
    _vector RayPos{}, RayDir{},NewRayPos{}, NewRayDir{};

     m_pGameInstance->Make_Ray(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ), m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir, true);
    
    CActor* pPickedObj{};
    _vector vPos{};
    
     if (0 == m_GameObjeList[COL_MONSTER].size())
         return S_OK;

     _float fDist{};
     _float fNewDist = {0xffff};
     for (auto& Monster : m_GameObjeList[COL_MONSTER])
     {
         if (nullptr == Monster)
             continue;
          

         if (true == Monster->Get_Collider()->Intersect(pPlayer->Get_Collider()))
         {
             pPlayer->Check_Coll();
             pPlayer->Set_CurrentHP(1);
         }

         if (true == Monster->Get_Collider()->RayIntersects(RayPos, RayDir, fDist))
         {
             if (fDist < fNewDist)
             {
                 if (fDist != 0) {
                     fNewDist = fDist;
                     pPickedObj = dynamic_cast<CActor*>(Monster);
                     NewRayPos = RayPos;
                     NewRayDir = RayDir;
                 }
             }
         }
     }

    if (pPickedObj)
    {
        _vector FinalPos{}, vNomal{};
        if (pPickedObj->Part_Intersects(NewRayPos, NewRayDir, FinalPos,vNomal))
        {
           pPickedObj->Check_Coll();
           pPlayer->CreateEffect(NewRayPos, NewRayDir, FinalPos, vNomal);

          return true;
        }else 
          return false;
    }

    return false;
}

HRESULT Collider_Manager::Player_To_Monster_Bullet_Collison() {

   CActor* pPlayer = m_pGameInstance->Get_Player();
   if (nullptr == pPlayer)  
       return E_FAIL;
  
  if (m_GameObjeList[COL_MONSTER_SKILL].size() == 0)
       return S_OK;
  
  for (auto& pMonsterBullet : m_GameObjeList[COL_MONSTER_SKILL])
  {
      if (nullptr != pMonsterBullet) {

          _bool bHit = true;
          if (CSkill::STYPE_SHOCKWAVE == dynamic_cast<CSkill*>(pMonsterBullet)->Get_SkillType())
          {
              if (true == dynamic_cast<CSkill*>(pMonsterBullet)->Comput_SafeZone(pPlayer->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION)))
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
              if (true == pMonsterBullet->Get_Collider()->Intersect(pPlayer->Get_Collider()))
              {
                  pPlayer->Set_CurrentHP(dynamic_cast<CSkill*>(pMonsterBullet)->Get_Damage());

                  if (dynamic_cast<CSkill*>(pMonsterBullet)->Get_SkillType() == CSkill::STYPE_STURN)
                      pPlayer->Stun_Routine();
                  else
                  pPlayer->Check_Coll();

                 if (dynamic_cast<CSkill*>(pMonsterBullet)->Get_ActorType() != CSkill::BOSS_MONSTER)
                 {
                     static_cast<CSkill*>(pMonsterBullet)->Dead_Rutine();
                 }
              }
          }
      }
  }

  return S_OK;
}

HRESULT Collider_Manager::Player_To_Mash_Collison_for_Decal()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();
    CGameObject* Obj = nullptr;
    CGameObject* floorObj = nullptr;
    _float fNewDist = FLT_MAX; 
    _float fDist = 0.f;
    _float fTerrainDist = FLT_MAX;
    _vector RayPos{}, RayDir{};
    _vector vPos{}, vDir{}, HitPos{};
    _float3 fNormal = {};
    _float3 fPos = {0.f, 0.f, 0.f};

    m_pGameInstance->Make_Ray(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                              m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir, true);
 
    for (auto& DecalObj : m_GameObjeList[COL_DECAL])
    {
        CCollider* Collider = DecalObj->Get_Collider();
        if (!Collider)
        {
            floorObj = DecalObj;
           
            fPos = static_cast<CVIBuffer_Terrain*>(floorObj->Find_Component(TEXT("Com_Buffer")))
                       ->Picking_OnTerrain_QuadTree(RayPos, RayDir, floorObj->Get_Transform(), &fTerrainDist, &fNormal);
        }
        if (Collider&& Collider->RayIntersects(RayPos, RayDir, fDist, &vPos))
        {
            if (fDist < fNewDist && fDist < fTerrainDist)
            {
                fNewDist = fDist;
                Obj = DecalObj;
                HitPos = vPos;
                vDir = RayDir;
            }
        }
    }

    DECAL_DESC* Desc = pPlayer->Get_DecalDesc();

    if (floorObj && fTerrainDist <= fNewDist)
    {
        Desc->vNormal = XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f);
        Desc->vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
        Desc->iType = DECAL_DESC::TYPE_BOX;
        m_pGameInstance->Add_Decal(Desc->Key, Desc);

      pPlayer->CreateEffect(RayPos, vDir, XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f),
                                                    XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f));
    }
    else if (Obj && (fTerrainDist > fNewDist))
    {
        CModel* ObjModel = dynamic_cast<CModel*>(Obj->Find_Component(TEXT("Com_Model")));
        _vector Normal{};
        _vector vPoss{};
        _bool IsIntersect = ObjModel->RayIntersect(RayPos, vDir, Obj->Get_Transform(), vPoss, Normal);
        if (ObjModel && IsIntersect)
        {
            Desc->vNormal = Normal;
            Desc->vPos = vPoss;
            Desc->vDir = XMVector3Normalize(vDir);
            Desc->iType = DECAL_DESC::TYPE_BOX;
            m_pGameInstance->Add_Decal(Desc->Key, Desc);
            pPlayer->CreateEffect(RayPos, vDir, vPoss, Normal);
        }
        else if (ObjModel && false == IsIntersect)
        {
            CGameObject* pClosestObj = nullptr;
            _float fClosestDist = FLT_MAX;
            _vector vClosestHit{}, vClosestDir{};

            // 기존 실패한 오브젝트는 제외하고 다시 순회
            for (auto& NextObj : m_GameObjeList[COL_DECAL])
            {
                if (NextObj == Obj) // 이전 실패 오브젝트는 제외
                    continue;

                CCollider* NextCol = NextObj->Get_Collider();
                if (!NextCol)
                    continue;

                _float fDist = 0.f;
                _vector vHitPos{};
                if (NextCol->RayIntersects(RayPos, RayDir, fDist, &vHitPos))
                {
                    if (fDist < fClosestDist)
                    {
                        fClosestDist = fDist;
                        pClosestObj = NextObj;
                        vClosestHit = vHitPos;
                        vClosestDir = RayDir;
                    }
                }
            }
            if (pClosestObj)
            {
                _vector vHitPosExact{}, vNormal{};
                CModel* NextModel = dynamic_cast<CModel*>(pClosestObj->Find_Component(TEXT("Com_Model")));
                if (NextModel && NextModel->RayIntersect(RayPos, RayDir, pClosestObj->Get_Transform(), vHitPosExact, vNormal))
                {
                    Desc->vNormal = vNormal;
                    Desc->vPos = vHitPosExact;
                    Desc->vDir = XMVector3Normalize(vDir);
                    Desc->iType = DECAL_DESC::TYPE_BOX;
                    m_pGameInstance->Add_Decal(Desc->Key, Desc);
                    pPlayer->CreateEffect(RayPos, vDir, vHitPosExact, vNormal);
   
                }
                else if (floorObj)
                {
                    // 교차가 아무것도 없으면 지면에라도 찍기
                    Desc->vNormal = XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f);
                    Desc->vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
                    Desc->iType = DECAL_DESC::TYPE_BOX;
                    Desc->fDepth = 3.f;

                    m_pGameInstance->Add_Decal(Desc->Key, Desc);
                    pPlayer->CreateEffect(RayPos, RayDir,XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f), XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f));
                }
            }
            else if (floorObj)
            {
                // 교차가 아무것도 없으면 지면에라도 찍기
                Desc->vNormal = XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f);
                Desc->vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
                Desc->iType = DECAL_DESC::TYPE_BOX;
                Desc->fDepth = 3.f;

                m_pGameInstance->Add_Decal(Desc->Key, Desc);
                pPlayer->CreateEffect(RayPos, RayDir, XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f), XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f));
            }
        }
        else if (!Obj &&floorObj)
        {
            Desc->vNormal = XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f);
            Desc->vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f);
            Desc->iType = DECAL_DESC::TYPE_BOX;
            Desc->fDepth = 3.f;
            m_pGameInstance->Add_Decal(Desc->Key, Desc);
            pPlayer->CreateEffect(RayPos, vDir, XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f), XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f));
        }
    }
     
    return S_OK;
}

HRESULT Collider_Manager::Boss_To_Mash_Collison()
{
    for (auto& SkillObj : m_GameObjeList[COL_MONSTER_SKILL])
    {
        _uint iActorType = static_cast<CSkill*>(SkillObj)->Get_ActorType();
        _uint iSkillType = static_cast<CSkill*>(SkillObj)->Get_SkillType();
        for (auto& DecalObj : m_GameObjeList[COL_DECAL]) {
        
            CCollider* pCol = DecalObj->Get_Collider();
           
            if (nullptr == pCol && iSkillType != CSkill::STYPE_LASER)
            {
                CVIBuffer_Terrain* buffer =
                    static_cast<CVIBuffer_Terrain*>(DecalObj->Find_Component(TEXT("Com_Buffer")));
                _vector vNormal{}, vWorldPos{};
                if (buffer && buffer->Intersect_OnTerrain_QuadTree(SkillObj->Get_Collider(),DecalObj->Get_Transform(), &vNormal, &vWorldPos))
                {
                    _vector vNormal{}, vWorldPos{};
                    if (buffer && buffer->Intersect_OnTerrain_QuadTree(
                                      SkillObj->Get_Collider(), DecalObj->Get_Transform(), &vNormal, &vWorldPos))
                    {
                        static_cast<CSkill*>(SkillObj)->Dead_Rutine();
                        static_cast<CSkill*>(SkillObj)->CreateEffect(XMVectorZero(), XMVectorZero(), vWorldPos,
                                                                     vNormal);
                    }
                }
            }
            else if (iSkillType != CSkill::STYPE_LASER && iSkillType != CSkill::STYPE_SHOCKWAVE) 
            if (pCol && SkillObj->Get_Collider()->Intersect(pCol))
            {
                static_cast<CSkill*>(SkillObj)->Dead_Rutine();
            }
            
            if (iActorType == CSkill::BOSS_MONSTER) {
                DECAL_DESC* Desc = static_cast<CSkill*>(SkillObj)->Get_DecalDesc();
                if (Desc->bOnce && Desc->bActive)
                    continue;

                CCollider*DecalCol =  DecalObj->Get_Collider();

                if (DecalCol && SkillObj->Get_Collider()->Intersect(DecalCol))
                {
                    Desc->iType = DECAL_DESC::TYPE_SSD;
                    m_pGameInstance->Add_Decal(Desc->Key, Desc);
                }
                cout << m_GameObjeList[COL_MONSTER_SKILL].size() << endl;
                if (nullptr == DecalCol  )
                {
                    _float3 fNormal = {};
                    _float3 fPos = {0.f, 0.f, 0.f};
                    _float fTerrainDist{};

                    _float3 fDir{};
                    XMStoreFloat3(&fDir, Desc->vDir);
                    if (fDir.x == 0.f&& fDir.y == 0.f&& fDir.z == 0.f)
                        Desc->vDir = XMVectorSet(0.f, -1.f, 0.f,0.f);

                    CVIBuffer_Terrain* buffer =  static_cast<CVIBuffer_Terrain*>(DecalObj->Find_Component(TEXT("Com_Buffer")));
                    if (iSkillType == CSkill::STYPE_LASER)
                    {
                        fPos = buffer->Picking_OnTerrain_QuadTree(Desc->vPos, XMVector3Normalize(Desc->vDir),DecalObj->Get_Transform(), &fTerrainDist, &fNormal);
                        Desc->vNormal = XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f);
                        Desc->vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 0.f);
                        Desc->iType = DECAL_DESC::TYPE_BOX;
                        m_pGameInstance->Add_Decal(Desc->Key, Desc);
                    }
                }

                 if (Desc->bOnce)
                    Desc->bActive = true;
            }
        }
    }

    return S_OK;
}

HRESULT Collider_Manager::Monster_To_Mash_Collison_for_Decal()
{
    for (auto& Monster : m_GameObjeList[COL_MONSTER])
    {
        DECAL_DESC* Desc = static_cast<CActor*>(Monster)->Get_DecalDesc();

        for (auto& DecalObj : m_GameObjeList[COL_DECAL])
        {
            if (Desc->bColActive == true)
            {    
                if (Desc->bOnce && Desc->bActive)
                    continue;
                
                CCollider* DecalCol = DecalObj->Get_Collider();
                if (nullptr == DecalCol)
                {
                    _float3 fNormal = {};
                    _float3 fPos = {0.f, 0.f, 0.f};
                    _float fTerrainDist{};
                    _float3 fDir{};
                    XMStoreFloat3(&fDir, Desc->vDir);
                    if (fDir.x == 0.f&& fDir.y == 0.f&& fDir.z == 0.f)
                        Desc->vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

                    fPos = static_cast<CVIBuffer_Terrain*>(DecalObj->Find_Component(TEXT("Com_Buffer")))
                            ->Picking_OnTerrain_QuadTree(Desc->vPos, Desc->vDir, DecalObj->Get_Transform(), &fTerrainDist, &fNormal);

                    Desc->vNormal = XMVectorSet(fNormal.x, fNormal.y, fNormal.z, 0.f);
                    Desc->vPos = XMVectorSet(fPos.x, fPos.y, fPos.z, 0.f);
                    Desc->iType = DECAL_DESC::TYPE_BOX;
                    m_pGameInstance->Add_Decal(Desc->Key, Desc);
                }

                if (Desc->bOnce)
                    Desc->bActive = true;
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
    Safe_Release(m_pGameInstance);
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
    return S_OK;
}

HRESULT Collider_Manager::Monster_To_Monster_Collision()
{
    auto& grid = m_SpatialGrid.Grid();
    if (grid.empty())
        return S_OK;

    for (auto& cell : grid)
    {
        if (cell.monsters.empty())
            continue;

        // 같은 셀 내 충돌
        for (size_t i = 0; i < cell.monsters.size(); ++i)
        {
            CActor* pA = dynamic_cast<CActor*>(cell.monsters[i]);
            if (!pA)
                continue;

            for (size_t j = i + 1; j < cell.monsters.size(); ++j)
            {
                CActor* pB = dynamic_cast<CActor*>(cell.monsters[j]);
                if (!pB)
                    continue;

                if (pA->Get_Collider()->Intersect(pB->Get_Collider()))
                {
                    _vector posA = pA->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
                    _vector posB = pB->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

                    _vector diff = posB - posA;
                    diff = XMVectorSetY(diff, 0.f);
                    _float dist = XMVectorGetX(XMVector3Length(diff));
                    if (dist < 0.001f)
                    {
                        diff = XMVectorSet((rand() % 100) / 100.f + 0.1f, 0.f, (rand() % 100) / 100.f + 0.1f, 0.f);
                        dist = XMVectorGetX(XMVector3Length(diff));
                    }

                    const _float minDist = pA->Get_Collider()->Get_iCurRadius() + pB->Get_Collider()->Get_iCurRadius();
                    if (dist < minDist)
                    {
                        const _float overlap = (minDist - dist) * 0.3f;
                        const _vector dir = XMVector3Normalize(diff);
                        pA->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION,
                                                           XMVectorSetW(posA - dir * overlap, 1.f));
                        pB->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION,
                                                           XMVectorSetW(posB + dir * overlap, 1.f));
                    }
                }
            }

            // 인접 셀 충돌
            int ix, iz;
            const _vector pos = pA->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
            if (!m_SpatialGrid.WorldToCell(pos, ix, iz))
                continue;

            int neighborIdx[9], neighborCount = 0;
            m_SpatialGrid.GatherNeighborCells(ix, iz, neighborIdx, neighborCount);

            for (int n = 0; n < neighborCount; ++n)
            {
                auto& neighbor = grid[neighborIdx[n]];
                if (&neighbor == &cell)
                    continue;

                for (auto* objB : neighbor.monsters)
                {
                    CActor* pB = dynamic_cast<CActor*>(objB);
                    if (!pB)
                        continue;

                    if (pA->Get_Collider()->Intersect(pB->Get_Collider()))
                    {
                        _vector posA = pA->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
                        _vector posB = pB->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
                        _vector diff = posB - posA;
                        diff = XMVectorSetY(diff, 0.f);
                        _float dist = XMVectorGetX(XMVector3Length(diff));

                        const _float minDist =
                            pA->Get_Collider()->Get_iCurRadius() + pB->Get_Collider()->Get_iCurRadius();
                        if (dist < minDist)
                        {
                            const _float overlap = (minDist - dist) * 0.3f;
                            const _vector dir = XMVector3Normalize(diff);
                            pA->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION,
                                                               XMVectorSetW(posA - dir * overlap, 1.f));
                            pB->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION,
                                                               XMVectorSetW(posB + dir * overlap, 1.f));
                        }
                    }
                }
            }
        }
    }

    return S_OK;
}

Collider_Manager* Collider_Manager::Create()
{
    Collider_Manager* pInstance = new Collider_Manager();

    if (FAILED(pInstance->Initialize())){
    
        MSG_BOX("Failed to Created : Collider_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Collider_Manager::Free()
{
    __super::Free();


    for (auto& coll : m_ColliderList)
        Safe_Release(coll);
    m_ColliderList.clear();

    for (_int i = 0; i < COL_END; i++)
    {
        for (auto& Obj : m_GameObjeList[i]) Safe_Release(Obj);
        m_GameObjeList[i].clear();
    }
    Safe_Release(m_pGameInstance);
}
