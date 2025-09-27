#include "GameInstance.h"
#include "Actor.h"
#include "Skill.h"
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
    Monster_To_Monster_Collision();
    Check_Collider_PlayerCollison();
    Player_To_Monster_Bullet_Collison();
    Check_Inetrect_Player();
    if (m_bIsColl)
    {
        if (true == Player_To_Monster_Ray_Collison_Check())
        {
            Player_To_Mash_Collison_for_Decal();
        }
        m_bIsColl = false;
    }

    MonsterBullet_To_Mash_Collison_for_Decal();

    for (auto& Monster : m_GameObjeList[COL_MONSTER]) Safe_Release(Monster);
    m_GameObjeList[COL_MONSTER].clear();
    for (auto& Decal : m_GameObjeList[COL_DECAL]) Safe_Release(Decal);
    m_GameObjeList[COL_DECAL].clear();
}

_bool Collider_Manager::Player_To_Monster_Ray_Collison_Check()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();
    if (false == pPlayer)
        return false;
   
    _vector RayPos{}, RayDir{};

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
                     vPos = RayPos + RayDir * fNewDist;
                 }
             }
         }
     }

    if (pPickedObj)
    {
        pPickedObj->Check_Coll();
        return false;
    }

    return true;
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
                  pMonsterBullet->Set_Dead(true);
              }
          }
      }
  }

  return S_OK;
}

HRESULT Collider_Manager::Player_To_Mash_Collison_for_Decal()
{
    CGameObject* Obj{};
    _float fNewDist = {0xffff};
    _float fDist = 0.f;
    _vector RayPos{}, RayDir{};
    _vector NewRayPos{}, NewRayDir{};
    m_pGameInstance->Make_Ray(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                              m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir, true);

    for (auto& Decal : m_GameObjeList[COL_DECAL])
    {
        if (true == Decal->Get_Collider()->RayIntersects(RayPos, RayDir, fDist))
        {
            if (fDist < fNewDist)
            {
                if (fDist != 0)
                {
                    fNewDist = fDist;
                    Obj = Decal;
                    NewRayPos = RayPos;
                    NewRayDir = RayDir;
                }
            }
        }
    }
    if (Obj)
    {
        Obj->CreateDecal(NewRayPos, NewRayDir);
    }

    return S_OK;
}

HRESULT Collider_Manager::MonsterBullet_To_Mash_Collison_for_Decal()
{
    CGameObject* Obj{};
 
    _float fDist = 0.f;
    _vector RayPos{}, RayDir{};
    _vector NewRayPos{}, NewRayDir{};
    for (auto& Bullet : m_GameObjeList[COL_MONSTER_SKILL]) 
    {  
      for (auto& Decal : m_GameObjeList[COL_DECAL])
      {
          if (true == Bullet->Get_Collider()->Intersect(Decal->Get_Collider()))
          {
              static_cast<CSkill*>(Bullet)->Get_Ray(&RayPos, &RayDir);
              _float fDist{};
              //cout << XMVectorGetX(RayDir) << " " << XMVectorGetY(RayDir) << " " << XMVectorGetZ(RayDir) << endl;
              if (true == Bullet->Get_Collider()->RayIntersects(RayPos,RayDir,fDist))
              {
                  Obj = Decal;
              }

              Bullet->Set_Dead(true);
          }
      }
        Safe_Release(Bullet);
    }
    m_GameObjeList[COL_MONSTER_SKILL].clear();

    if (Obj)
    {
        Obj->CreateDecal(RayPos, RayDir);
    }


    return S_OK;
}

void Collider_Manager::Clear()
{
    for (auto& Collider : m_ColliderList)
        Safe_Release(Collider);
    m_ColliderList.clear();

    for (_int i = 0; i < COL_END; i++)
    {
        for (auto& Obj : m_GameObjeList[i]) Safe_Release(Obj);
        m_GameObjeList[i].clear();
    };
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
    if (m_GameObjeList[COL_MONSTER].size() < 2)
        return S_OK;

    for (auto itA = m_GameObjeList[COL_MONSTER].begin(); itA != m_GameObjeList[COL_MONSTER].end(); ++itA)
    {
        if (m_GameObjeList[COL_MONSTER].size() < 2)
            return S_OK;

        for (auto itA = m_GameObjeList[COL_MONSTER].begin(); itA != m_GameObjeList[COL_MONSTER].end(); ++itA)
        {
            CActor* pA = dynamic_cast<CActor*>(*itA);
            if (!pA)
                continue;

            auto itB = itA;
            ++itB;
            for (; itB != m_GameObjeList[COL_MONSTER].end(); ++itB)
            {
                CActor* pB = dynamic_cast<CActor*>(*itB);
                if (!pB)
                    continue;

                if (pA->Get_Collider()->Intersect(pB->Get_Collider()))
                {
                    _vector posA = pA->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
                    _vector posB = pB->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

                    _vector diff = posB - posA;
                    diff = XMVectorSetY(diff, 0.f); // Y 고정
                    _float dist = XMVectorGetX(XMVector3Length(diff));

                    if (dist < 0.001f)
                    {
                        // 같은 위치일 경우 랜덤 방향
                        diff = XMVectorSet((rand() % 100) / 100.f + 0.1f, 0.f, (rand() % 100) / 100.f + 0.1f, 0.f);
                        dist = XMVectorGetX(XMVector3Length(diff));
                    }

                    _float radiusA = pA->Get_Collider()->Get_iCurRadius();
                    _float radiusB = pB->Get_Collider()->Get_iCurRadius();
                    _float minDist = radiusA + radiusB;

                    if (dist < minDist)
                    {
                        _float overlap = (minDist - dist) * 0.3f; // 부드럽게 밀기
                        _vector dir = XMVector3Normalize(diff);

                        _vector pushA = -dir * overlap;
                        _vector pushB = dir * overlap;

                        posA = XMVectorSetW(posA + pushA, 1.f);
                        posB = XMVectorSetW(posB + pushB, 1.f);

                        pA->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, posA);
                        pB->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, posB);
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
