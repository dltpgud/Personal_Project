#include "stdafx.h"
#include "Trigger.h"
#include "GameInstance.h"
#include "Monster.h"
CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CTrigger::CTrigger(const CTrigger& Prototype) : CGameObject{Prototype}
{
}

HRESULT CTrigger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
    CTrigger_DESC* pDesc = static_cast<CTrigger_DESC*>(pArg);
    m_iLEVEL = pDesc->CuriLevelIndex;
    m_iCoType = pDesc->iColType;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_TriggerZone(pDesc->iTriggerType)))
        return E_FAIL;
    
    return S_OK;
}
void CTrigger::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CTrigger::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CTrigger::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_Interctive(this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CTrigger::Render()
{   
    return S_OK;
}

HRESULT CTrigger::Add_Components()
{
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};
    CBounding_AABB::BOUND_AABB_DESC AABBDesc{};
    CBounding_Sphere::BOUND_SPHERE_DESC SphereDesc{};
    
    switch (m_iCoType)
    {
    case CCollider::TYPE_OBB:
        OBBDesc.vRotation = m_fRot;
        OBBDesc.vExtents  = m_fExtents;
        OBBDesc.vCenter   = m_fCenter;
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
                                          TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom),
                                          &OBBDesc)))
            return E_FAIL;
        break;

    case CCollider::TYPE_AABB:
        AABBDesc.vExtents = m_fExtents;
        AABBDesc.vCenter  = m_fCenter;
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
                                          TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom),
                                          &AABBDesc)))
            return E_FAIL;
        break;

   case CCollider::TYPE_SPHERE:
       SphereDesc.fRadius = m_fRadius;
       SphereDesc.vCenter = m_fCenter;
            if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
                                              TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom),
                                         &SphereDesc)))
           return E_FAIL;
        break;
    default: break;
    }
    
    return S_OK;
}

HRESULT CTrigger::Set_TriggerZone(_int Type)
{
    switch (Type)
    {
    case TYPE_ASTAR:
     
        if (m_iCoType == CCollider::TYPE_AABB || m_iCoType == CCollider::TYPE_OBB) {
            m_fExtents.y = 15.f;
        }
        Set_Coll_Info();

        m_pColliderCom->SetTriggerCallback([&](CActor * other, _bool bColliding, _bool bPlayer) {
 
              if (bColliding && bPlayer)
              {
                  if (m_bTriggered)
                      return; 
             
                  m_bTriggered = true;
             
                  list<CGameObject*> Monster = m_pGameInstance->Get_ALL_GameObject(m_iLEVEL, TEXT("Layer_Monster"));
             
                  _float3 fPos{};
                  for (auto& monster :Monster)
                  {
                      XMStoreFloat3(&fPos, monster->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
             
                      if (true == m_pColliderCom->IsInside(fPos))
                          dynamic_cast<CMonster*>(monster)->Wake_up();
                  }
              }
              else if (false == bColliding && true == bPlayer)
              {
                  m_bTriggered = false;
             
                  list<CGameObject*> Monster = m_pGameInstance->Get_ALL_GameObject(m_iLEVEL, TEXT("Layer_Monster"));
             
                  _float3 fPos{};
                  for (auto& monster : Monster)
                  {
                      auto pMonster = dynamic_cast<CMonster*>(monster);
                      XMStoreFloat3(&fPos, pMonster->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
                      pMonster -> Compute_Length();
                      if (true == m_pColliderCom->IsInside(fPos) && (*pMonster->Get_fAttackLength()) >= 70.f)
                      {
                          pMonster->Seeping();
                      }
                  }
              }

        });
        break;
    
    case TYPE_LAVA: 
        
       m_pColliderCom->SetTriggerCallback(
            [this](CActor* other, _bool bColliding, _bool bPlayer)
            {
                if (bColliding)
                    other->SetTriggerFlag(CActor::Trigger_Terrain, true);
            });
    
        break;
    
    default: break;
    }

     return S_OK;
}


HRESULT CTrigger::Set_Coll_Info()
{
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};
    CBounding_AABB::BOUND_AABB_DESC AABBDesc{};
    CBounding_Sphere::BOUND_SPHERE_DESC SphereDesc{};
    switch (m_iCoType)
    {
    case CCollider::TYPE_OBB:
        OBBDesc.vRotation = m_fRot;
        OBBDesc.vExtents = m_fExtents;
        OBBDesc.vCenter = m_fCenter;
        m_pColliderCom->Set_Info(&OBBDesc);   
        break;

    case CCollider::TYPE_AABB:
        AABBDesc.vExtents =m_fExtents;
        AABBDesc.vCenter = m_fCenter;
        m_pColliderCom->Set_Info(&AABBDesc);
        break;

    case CCollider::TYPE_SPHERE:
        SphereDesc.fRadius = m_fRadius;
        SphereDesc.vCenter = m_fCenter;
        m_pColliderCom->Set_Info(&SphereDesc);
        break;
    default: break;
    }

    return S_OK;
}

CTrigger* CTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTrigger* pInstance = new CTrigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTrigger");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTrigger::Clone(void* pArg)
{
    CTrigger* pInstance = new CTrigger(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CTrigger");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTrigger::Free()
{
    __super::Free();
}
