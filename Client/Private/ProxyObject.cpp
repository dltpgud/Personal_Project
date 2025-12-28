#include "stdafx.h"
#include "ProxyObject.h"
#include "GameInstance.h"

CProxyObject::CProxyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CProxyObject::CProxyObject(const CProxyObject& Prototype) : CGameObject{Prototype}
{
}

HRESULT CProxyObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProxyObject::Initialize(void* pArg)
{
    CProxyObject::MashInstanceDataCPU* pDesc = static_cast<CProxyObject::MashInstanceDataCPU*>(pArg);
    __super::Initialize(pDesc);

    if (FAILED(__super::Add_Component(pDesc->CuriLevelIndex, pDesc->ModelTag, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        MSG_BOX("Set_Model failed");
        return E_FAIL;
    }

    m_pTransformCom->Set_TRANSFORM(CTransform::T_RIGHT, pDesc->WorldMatrix.r[0]);
    m_pTransformCom->Set_TRANSFORM(CTransform::T_UP, pDesc->WorldMatrix.r[1]);
    m_pTransformCom->Set_TRANSFORM(CTransform::T_LOOK, pDesc->WorldMatrix.r[2]);
    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, pDesc->WorldMatrix.r[3]);

    _float3 fCenter, fExtend;
    AABB LocalAABB;
    m_pModelCom->Center_Ext(&fCenter, &fExtend, &LocalAABB);
    m_WorldAABB = m_pGameInstance->TransformAABB(LocalAABB, m_pTransformCom->Get_WorldMatrix());
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};
    OBBDesc.vExtents = fExtend;
    OBBDesc.vCenter = fCenter;
    OBBDesc.vRotation = {0.f, 0.f, 0.f};
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"),
                                      reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

   m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

   m_pGameInstance->Add_GameObject_To_ColGroup(this, Collider_Manager::COL_STATIC);

    return S_OK;
}
void CProxyObject::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CProxyObject::Update(_float fTimeDelta)
{
   
}

void CProxyObject::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CProxyObject::Render()
{   
    return S_OK;
}



CProxyObject* CProxyObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProxyObject* pInstance = new CProxyObject(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CProxyObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProxyObject::Clone(void* pArg)
{
    CProxyObject* pInstance = new CProxyObject(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CProxyObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProxyObject::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);

}
