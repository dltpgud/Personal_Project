#include "stdafx.h"
#include "Trigger.h"
#include "GameInstance.h"

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
    GAMEOBJ_DESC* pDesc = static_cast<GAMEOBJ_DESC*>(pArg);
    m_iObjectType = pDesc->Object_Type;
    

    size_t iLen = wcslen(pDesc->ProtoName) + 1;
    m_Proto = new wchar_t[iLen];
    lstrcpyW(m_Proto, pDesc->ProtoName);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}
void CTrigger::SetDESC(_float fRadius, _float3 fRot, _float3 vExtents, _float3 fCenter)
{
    m_fRot = fRot;
    m_fExtents = vExtents;
    m_fCenter = fCenter;
    m_fRadius = fRadius;
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};
    CBounding_AABB::BOUND_AABB_DESC AABBDesc{};
    CBounding_Sphere::BOUND_SPHERE_DESC SphereDesc{};
    switch (m_Type)
    {
    case CCollider::TYPE_OBB:
  
        OBBDesc.vRotation = m_fRot;
        OBBDesc.vExtents = m_fExtents;
        OBBDesc.vCenter = m_fCenter;

        m_pColliderCom->Set_Info(&OBBDesc);            
        break;

    case CCollider::TYPE_AABB:

        AABBDesc.vExtents = m_fExtents;
        AABBDesc.vCenter = m_fCenter;
        m_pColliderCom->Set_Info(&AABBDesc);   
        break;

    case CCollider::TYPE_SPHERE:;
        SphereDesc.fRadius = m_fRadius;
        SphereDesc.vCenter = m_fCenter;
        m_pColliderCom->Set_Info(&SphereDesc);   
        break;
    default: break;
    }



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

    __super::Late_Update(fTimeDelta);
}

HRESULT CTrigger::Render()
{
    
    return S_OK;
}

_tchar* CTrigger::Get_ProtoName()
{
    return m_Proto;
}

HRESULT CTrigger::Add_Components()
{
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};
    CBounding_AABB::BOUND_AABB_DESC AABBDesc{};
    CBounding_Sphere::BOUND_SPHERE_DESC SphereDesc{};
    switch (m_Type)
    {
    case CCollider::TYPE_OBB:
        OBBDesc.vRotation = m_fRot;
        OBBDesc.vExtents = m_fExtents;
        OBBDesc.vCenter =   m_fCenter;
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
                                          TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom),
                                          &OBBDesc)))
            return E_FAIL;
        break;

    case CCollider::TYPE_AABB:
        AABBDesc.vExtents = m_fExtents;
        AABBDesc.vCenter = m_fCenter;
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
void CTrigger::Set_Type(_int Type)
{
    m_Type = Type;
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};
    CBounding_AABB::BOUND_AABB_DESC AABBDesc{};
    CBounding_Sphere::BOUND_SPHERE_DESC SphereDesc{};
    switch (m_Type)
    {
    case CCollider::TYPE_OBB:
        OBBDesc.vRotation = m_fRot;
        OBBDesc.vExtents = m_fExtents;
        OBBDesc.vCenter = m_fCenter;
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
                                          TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom),
                                          &OBBDesc)))
            return;
        break;

    case CCollider::TYPE_AABB:
        AABBDesc.vExtents = m_fExtents;
        AABBDesc.vCenter = m_fCenter;
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
                                          TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom),
                                          &AABBDesc)))
            return;
        break;

    case CCollider::TYPE_SPHERE:
        SphereDesc.fRadius = m_fRadius;
        SphereDesc.vCenter = m_fCenter;
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
                                          TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom),
                                          &SphereDesc)))
            return;
        break;
    default: break;
    }



}
HRESULT CTrigger::Bind_ShaderResources()
{

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

        if (m_bClone)
            Safe_Delete_Array(m_Proto);
}
