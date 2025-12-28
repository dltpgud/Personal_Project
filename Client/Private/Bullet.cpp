#include "stdafx.h"
#include "Bullet.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BillyBoom.h"

CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSkill{pDevice, pContext}
{
    m_DecalDesc.bOnce = false;
    m_DecalDesc.Key = TEXT("Base");
    m_DecalDesc.fSize = 0.5f;
    m_DecalDesc.fDepth = 0.5f;
    m_DecalDesc.bNormal = true;
    m_DecalDesc.fLifeTime = 2.f;
}

CBullet::CBullet(const CBullet& Prototype) : CSkill{Prototype}
{
}

HRESULT CBullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
   CBULLET_DESC* pDesc = static_cast<CBULLET_DESC*>(pArg);
     m_pTagetPos                     = pDesc->pTagetPos;
     m_pScale                        = pDesc->fScale;
     
   if (FAILED(__super::Initialize(pDesc)))
       return E_FAIL; 

    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, m_vPos);

    m_vDir = m_pTagetPos - m_vPos;
    m_vDir = XMVector3Normalize(m_vDir);
    m_fLifeTime = 20.f;
  
    m_iTrailIndex = static_cast<CEffect_TrailStream*>(m_pGameInstance->Find_EffectStream(L"SpriteTexTrail"))->AllocateTrail();

    m_DecalDesc.iTexIndex = 1;

    if (m_iSkillType == CSkill::STYPE_BERRLE)
    {
        m_DecalDesc.fSize = 2.5f;
        m_DecalDesc.fDepth = 2.5f;
    }
    return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

        m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
}

void CBullet::Update(_float fTimeDelta)
{
        _float3 fPos;
        XMStoreFloat3(&fPos, m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION));
      CEffect_TrailStream::  SPAWN_REQUEST req{};
        req.Valid = m_iLifeState == OBJ_POOL ? 0 : 1;
        req.trailIndex = m_iTrailIndex;
        req.headPos = fPos;
        req.addLife = 1.0f;
        req.width = m_iSkillType == BOSS_MONSTER ? 3.5f: 0.9f;
        req.color = m_Color[CSkill::COLOR::CEND];
        req.frameIndex = 1;
        req.isSegment = 0;
        m_pGameInstance->Trigger_Effect(L"SpriteTexTrail", &req);

    __super::Update(fTimeDelta);
}

void CBullet::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_ColGroup(this, Collider_Manager::CollGroup::COL_MONSTER_SKILL)))
        return;

    if (false == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION), 1.5f))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
    
    __super::Late_Update(fTimeDelta);
}

HRESULT CBullet::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CBullet::Dead_Rutine()
{
   static_cast<CEffect_TrailStream*>(m_pGameInstance->Find_EffectStream(L"SpriteTexTrail"))->ReleaseTrail(m_iTrailIndex);
    m_iLifeState = OBJ_POOL;

}

HRESULT CBullet::CreateEffect(_vector RayStartPos, _vector RayDir, _vector RayEndPos, _vector vNomal, void* pArg)
{
    _float3 fPos;
    XMStoreFloat3(&fPos, RayEndPos);
    CEffect_TrailStream::SPAWN_REQUEST req{};
    req.Valid = m_iLifeState == OBJ_POOL ? 0 : 1;
    req.trailIndex = m_iTrailIndex;
    req.headPos = fPos;
    req.addLife = 1.0f;
    req.width = m_iSkillType == BOSS_MONSTER ? 3.5f : 0.9f;
    req.color = m_Color[CSkill::COLOR::CEND];
    req.frameIndex = 1;
    req.isSegment = 0;
    m_pGameInstance->Trigger_Effect(L"SpriteTexTrail", &req);
    return S_OK;
}

HRESULT CBullet::Add_Components()
{
    CBounding_Sphere::BOUND_SPHERE_DESC CBounding_Sphere{};
    _float3 Center{}, Extents{};
    CBounding_Sphere.fRadius = 0.4f;
    CBounding_Sphere.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
                                      reinterpret_cast<CComponent**>(&m_pColliderCom), &CBounding_Sphere)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterBullet"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Point"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBufferPoint"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL; 

  return S_OK;
}

HRESULT CBullet::Bind_ShaderResources()
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
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RgbStart", &m_Color[CSkill::COLOR::CSTART], sizeof(_float4))))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RgbEnd", &m_Color[CSkill::COLOR::CEND], sizeof(_float4))))
        return E_FAIL;

  return S_OK;
}

CBullet* CBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBullet* pInstance = new CBullet(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBullet::Clone(void* pArg)
{
    CBullet* pInstance = new CBullet(*this);
   
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBullet::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTrailTextureCom);
}
