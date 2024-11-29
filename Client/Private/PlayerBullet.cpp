#include "stdafx.h"
#include "PlayerBullet.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Weapon.h"
CPlayerBullet::CPlayerBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSkill{pDevice, pContext}
{
}

CPlayerBullet::CPlayerBullet(const CPlayerBullet& Prototype) : CSkill{Prototype}
{
}

HRESULT CPlayerBullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayerBullet::Initialize(void* pArg)
{
    if (nullptr != pArg) {
        CPlayerBullet_DESC* pDesc = static_cast<CPlayerBullet_DESC*>(pArg);
        m_pTagetPos = pDesc->pTagetPos;
        m_vPlayerAt = pDesc->vPlayerAt;
        m_Local = pDesc->Local;
        m_WorldPtr = pDesc->WorldPtr;
        m_iWeaponType = pDesc->iWeaponType;
        if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;
    }else
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;


    if (FAILED(BIND_BULLET_TYPE()))
        return E_FAIL;

  
    if (0.f == m_fLifeTime)
        m_fLifeTime = 1.f;
    return S_OK;
}

_int CPlayerBullet::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        return OBJ_DEAD;
    }  
    __super::Priority_Update(fTimeDelta);



    if (true == m_bStart) {
        m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
    }

    return OBJ_NOEVENT;

}

void CPlayerBullet::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CPlayerBullet::Late_Update(_float fTimeDelta)
{
    if (false == m_bStart) {
        _vector vHPos = XMVector3TransformCoord(m_Local, XMLoadFloat4x4(m_WorldPtr));

        m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, vHPos);
        _vector Dir = m_pTagetPos - m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
        Dir = XMVectorSetW(Dir, 0.f);
        m_vDir = XMVector3Normalize(Dir);

        m_bStart = true;
    }

    
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
        return;
    __super::Late_Update(fTimeDelta);
}

void CPlayerBullet::Dead_Rutine(_float fTimeDelta)
{

    m_DeadSum += fTimeDelta;
    m_iTexNum = 1;

   if (m_DeadSum > 0.4f)
        m_bDead = true;

}

HRESULT CPlayerBullet::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pTextureCom[m_iTexNum]->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayerBullet::BIND_BULLET_TYPE()
{

    switch (m_iWeaponType)
    {
    case CWeapon::HendGun : 
        m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
        m_pScale.x = 0.13f;
        m_pScale.y = 0.13f;
        m_Clolor[CSkill::COLOR::CSTART] = { 0.f, 0.5f, 1.f, 0.5f };
        m_Clolor[CSkill::COLOR::CEND] =   { 0.f, 1.f, 1.f,1.f };
        break;

    case CWeapon::AssaultRifle :
        m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
        m_pScale.x = 0.1f;
        m_pScale.y = 0.1f;
        m_Clolor[CSkill::COLOR::CSTART] = { 0.f, 0.5f, 1.f, 0.5f };
        m_Clolor[CSkill::COLOR::CEND] = { 0.f, 1.f, 0.f,1.f };
        break;
   
    case CWeapon::MissileGatling :
        m_pTransformCom->Set_Scaling(1.3f, 1.3f, 1.3f);
        m_pScale.x = 0.2f;
        m_pScale.y = 0.2f;
        m_Clolor[CSkill::COLOR::CSTART] = { 0.5f, 0.5f, 0.5f, 0.5f };
        m_Clolor[CSkill::COLOR::CEND] = { 1.f, 1.f, 0.f,1.f };
        break;
   
    case CWeapon::HeavyCrossbow :
        m_pTransformCom->Set_Scaling(1.f, 1.f, 1.f);
        m_pScale.x = 0.16f;
        m_pScale.y = 0.16f;
        m_Clolor[CSkill::COLOR::CSTART] = { 0.f, 0.5f, 1.f, 0.5f };
        m_Clolor[CSkill::COLOR::CEND] = { 0.f, 1.f, 1.f,1.f };
        break;
      
    }
   return S_OK;
}

HRESULT CPlayerBullet::Add_Components()
{
    CBounding_Sphere::BOUND_SPHERE_DESC		CBounding_Sphere{};
    _float3 Center{}, Extents{};
    CBounding_Sphere.fRadius = 0.1f;
    CBounding_Sphere.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CBounding_Sphere)))
        return E_FAIL;
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PlayerBullet"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Point"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBufferPoint"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PlayerBulletBoom"),
        TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
        reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;

    return S_OK;
}



HRESULT CPlayerBullet::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_PSize", &m_pScale, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RgbStart", &m_Clolor[CSkill::COLOR::CSTART], sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RgbEnd", &m_Clolor[CSkill::COLOR::CEND], sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}


CPlayerBullet* CPlayerBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayerBullet* pInstance = new CPlayerBullet(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayerBullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerBullet::Clone(void* pArg)
{
    CPlayerBullet* pInstance = new CPlayerBullet(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CPlayerBullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerBullet::Free()
{
    __super::Free();
    for(_int i = 0 ; i <2; i++ )
    Safe_Release(m_pTextureCom[i]);
    Safe_Release(m_pVIBufferCom);
}
