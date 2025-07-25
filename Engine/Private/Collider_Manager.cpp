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
;     CSkill* pBullet = static_cast<CSkill*>(MonsterBullet);
    if (nullptr == MonsterBullet)
    {
        MSG_BOX("No pMonsterBullet");
        return E_FAIL;
    }

    m_MonsterBullet.push_back(pBullet);
    Safe_AddRef(pBullet);
    return S_OK;
}

HRESULT Collider_Manager::Add_Collider(_float Damage, CCollider* Collider)
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
            pPlayer->Set_bColl(true);
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

    for (auto& Collider : m_interctiveList)
    {
       if (nullptr == Collider)
           continue;

        Collider->Get_Collider()->Intersect(pPlayer->Get_Collider());
    
        Safe_Release(Collider);
    }
    m_interctiveList.clear();

    return S_OK;
}

void Collider_Manager::All_Collison_check()
{
    Check_Collider_PlayerCollison();
    Player_To_Monster_Bullet_Collison();
    Player_To_Monster_Collison_Check();
    Check_Inetrect_Player();
    if (m_bIsColl)
    {
        Player_To_Monster_Ray_Collison_Check();
        m_bIsColl = false;
    }


}

HRESULT Collider_Manager::Player_To_Monster_Collison_Check()
{
     CActor* pPlayer = m_pGameInstance->Get_Player();
     if (false == pPlayer)
		return E_FAIL;
   
        if (0 == m_MonsterList.size())
            return S_OK;
   
        for (auto& Monster : m_MonsterList)
        {
            if (nullptr == Monster)
                continue;
   
            if (true == Monster->Get_Collider()->Intersect(pPlayer->Get_Collider()))
            {
                pPlayer->Set_bColl(true);
                pPlayer->Set_CurrentHP(1.f);
            }
        }  

        
   return S_OK;
}

HRESULT Collider_Manager::Player_To_Monster_Ray_Collison_Check()
{
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
        Safe_Release(Monster);
     }

    if (pPickedObj)
    {
        pPickedObj->Set_CurrentHP(m_pGameInstance->Get_Player()->Weapon_Damage());
        pPickedObj->Set_bColl(true);
    }
   
    m_MonsterList.clear();

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
          if (CSkill::TYPE_BILLYBOOM == pMonsterBullet->Get_ActorType() && CSkill::STYPE_SHOCKWAVE == pMonsterBullet->Get_SkillType())
          {
              if (true == pMonsterBullet->Comput_SafeZone(pPlayer->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION)))
              {
                  bHit = false;
              }
              else
              {
                  if (true == pPlayer->Get_bJump())
                      bHit = false;
                  else
                      bHit = true;
              }
          }

          if (true == bHit)
          {

              if (true == pMonsterBullet->Get_Collider()->Intersect(pPlayer->Get_Collider()))
              {
                  if (pMonsterBullet->Get_SkillType() == CSkill::STYPE_STURN)
                      pPlayer->Set_bStun(true);


                  pPlayer->Set_bColl(true);
                  pPlayer->Set_CurrentHP(pMonsterBullet->Get_Damage());


                  if (false == CSkill::STYPE_SHOCKWAVE == pMonsterBullet->Get_SkillType())
                      pMonsterBullet->Set_DeadSkill(true);

                 // pMonsterBullet->Set_Dead(true);
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

HRESULT Collider_Manager::changeCellType(_int type)
{
    m_pGameInstance->Get_Player()->Set_NavigationType(type);
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
