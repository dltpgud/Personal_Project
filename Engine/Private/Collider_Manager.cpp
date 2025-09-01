#include "Collider_Manager.h"
#include "Actor.h"
#include "Skill.h"
#include "GameInstance.h"

Collider_Manager::Collider_Manager() : m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT Collider_Manager::Initialize()
{
    return S_OK;
}

HRESULT Collider_Manager::Add_Monster( CGameObject* Monster)
{
    CActor* pMonster = static_cast<CActor*>(Monster);
    if (nullptr == pMonster)
    {
        MSG_BOX("No Monster");
        return E_FAIL;
    }

    m_MonsterList.push_back(pMonster);
    Safe_AddRef(pMonster);
    return S_OK;
}

HRESULT Collider_Manager::Add_MonsterBullet( CGameObject* MonsterBullet)
{
    CSkill* pBullet = static_cast<CSkill*>(MonsterBullet);

    if (nullptr == MonsterBullet)
    {
        MSG_BOX("No pMonsterBullet");
        return E_FAIL;
    }

    m_MonsterBullet.push_back(pBullet);
    Safe_AddRef(pBullet);

    return S_OK;
}

HRESULT Collider_Manager::Add_Collider(_int Damage, CCollider* Collider)
{
    m_ColliderDamage = Damage;
    m_ColliderList.push_back(Collider);
    Safe_AddRef(Collider);
    return S_OK;
}

HRESULT Collider_Manager::Add_Interctive(CGameObject* Interctive)
{
    m_interctiveList.push_back(Interctive);
    Safe_AddRef(Interctive);
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

    if (0 == m_interctiveList.size())
        return S_OK;

    for (auto& iter : m_interctiveList)
    {
        if (nullptr == iter)
           continue;
   
        iter->Get_Collider()->CollUpdate(pPlayer);

        for (auto& monster : m_MonsterList) 
        { iter->Get_Collider()->CollUpdate(monster); }
        
        Safe_Release(iter);
    }

    m_interctiveList.clear();

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
        Player_To_Monster_Ray_Collison_Check();
        m_bIsColl = false;
    }

    for (auto& Monster : m_MonsterList) Safe_Release(Monster);
    m_MonsterList.clear();
}

HRESULT Collider_Manager::Player_To_Monster_Ray_Collison_Check()
{
    CActor* pPlayer = m_pGameInstance->Get_Player();
    if (false == pPlayer)
        return E_FAIL;
   
    _vector RayPos{}, RayDir{};

     m_pGameInstance->Make_Ray(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ), m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir, true);
    
    CActor* pPickedObj{};
    _vector vPos{};

     if (0 == m_MonsterList.size())
         return S_OK;

     _float fDist{};
     _float fNewDist = {0xffff};
     
     for (auto& Monster : m_MonsterList)
     {
         if (nullptr == Monster)
             continue;
          

         if (true == Monster->Get_Collider()->Intersect(pPlayer->Get_Collider()))
         {
             pPlayer->Check_Coll();
             pPlayer->Set_CurrentHP(1);
         }

         if (true == Monster->Get_Collider()->RayIntersects(RayPos, RayDir, fDist)) {

             if (fDist < fNewDist)
             {
                 if (fDist != 0) {
                     fNewDist = fDist;
                     pPickedObj = Monster;
                     vPos = RayPos + RayDir * fNewDist;

                 }
             }
         }
     }

    if (pPickedObj)
    {
        pPickedObj->Check_Coll();
    }

    return S_OK;
}

HRESULT Collider_Manager::Player_To_Monster_Bullet_Collison() {

   CActor* pPlayer = m_pGameInstance->Get_Player();
   if (nullptr == pPlayer)  
       return E_FAIL;
  
  if (m_MonsterBullet.size() == 0)
       return S_OK;
  
  for (auto& pMonsterBullet : m_MonsterBullet)
  {
      if (nullptr != pMonsterBullet) {

          _bool bHit = true;
          if (CSkill::STYPE_SHOCKWAVE == pMonsterBullet->Get_SkillType())
          {
              if (true == pMonsterBullet->Comput_SafeZone(pPlayer->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION)))
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
                  pPlayer->Set_CurrentHP(pMonsterBullet->Get_Damage());

                  if (pMonsterBullet->Get_SkillType() == CSkill::STYPE_STURN)
                      pPlayer->Stun_Routine();
                  else
                  pPlayer->Check_Coll();

                 if (pMonsterBullet->Get_ActorType() != CSkill::BOSS_MONSTER)
                  pMonsterBullet->Set_Dead(true);
              }
          }
      }
      Safe_Release(pMonsterBullet);
  }
   m_MonsterBullet.clear();

  return S_OK;
}

void Collider_Manager::Clear()
{
    for (auto& Monster : m_MonsterList)
        Safe_Release(Monster);
    m_MonsterList.clear();


    for (auto& Bullet : m_MonsterBullet)
        Safe_Release(Bullet);
    m_MonsterBullet.clear();

    for (auto& Collider : m_ColliderList)
        Safe_Release(Collider);
    m_ColliderList.clear();

    for (auto& interctive : m_interctiveList)
        Safe_Release(interctive);
    m_interctiveList.clear();
}

HRESULT Collider_Manager::Find_Cell()
{
    m_pGameInstance->Get_Player()->Set_onCell(true);
    m_pGameInstance->Get_Player()->Find_CurrentCell();

    for (auto& Monster : m_MonsterList)
    {
        Monster->Find_CurrentCell();
        Safe_Release(Monster);
    }
    m_MonsterList.clear();
    return S_OK;
}

HRESULT Collider_Manager::Monster_To_Monster_Collision()
{
    if (m_MonsterList.size() < 2)
        return S_OK;

    for (auto itA = m_MonsterList.begin(); itA != m_MonsterList.end(); ++itA)
    {
        if (m_MonsterList.size() < 2)
            return S_OK;

        for (auto itA = m_MonsterList.begin(); itA != m_MonsterList.end(); ++itA)
        {
            CActor* pA = *itA;
            if (!pA)
                continue;

            auto itB = itA;
            ++itB;
            for (; itB != m_MonsterList.end(); ++itB)
            {
                CActor* pB = *itB;
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
    Safe_Release(m_pGameInstance);

    for (auto& Bullet : m_MonsterBullet)
        Safe_Release(Bullet);
    m_MonsterBullet.clear();

    for (auto& Monster : m_MonsterList)
        Safe_Release(Monster);
    m_MonsterList.clear();

    for (auto& coll : m_ColliderList)
        Safe_Release(coll);
    m_ColliderList.clear();
}
