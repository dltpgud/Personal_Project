#include "stdafx.h"
#include "Bullet.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ShockWave.h"
#include "BillyBoom.h"
#
CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSkill{pDevice, pContext}
{
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

   if (FAILED(Add_Components(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, m_vPos);

    _vector Dir = m_pTagetPos - m_vPos;
    Dir = XMVectorSetW(Dir, 0.f);
    m_vDir = XMVector3Normalize(Dir);

    m_pNavigationCom->Find_CurrentCell(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION));

    if (0.f == m_fLifeTime)
        m_fLifeTime = 1.f;

    return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    
    if (m_iSkillType == STYPE_SHOCKWAVE)
    {
        m_pTransformCom->Go_jump_Dir(fTimeDelta, m_vDir, 1.f, m_pNavigationCom, &m_bMoveStop);
      
    }
    else
        m_pTransformCom->GO_Dir(fTimeDelta, m_vDir, m_pNavigationCom, &m_bMoveStop);
}

void CBullet::Update(_float fTimeDelta)
{  
    if (true == m_bMoveStop)
            Dead_Rutine();

    __super::Update(fTimeDelta);
}

void CBullet::Late_Update(_float fTimeDelta)
{
    if (false == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION), 1.5f))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
        return;
    
    if (FAILED(m_pGameInstance->Add_MonsterBullet(this)))
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
    if (m_iSkillType == STYPE_SHOCKWAVE)
    {
        CShockWave::CShockWave_DESC Desc{};
        Desc.iDamage = m_iDamage;
        Desc.iSkillType = CSkill::SKill::STYPE_SHOCKWAVE;
        Desc.iActorType = CSkill::BOSS_MONSTER;
        Desc.vPos = m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION);
        m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Layer_Skill"), L"Prototype_GameObject_ShockWave",&Desc);
    }

    m_bDead = true;
}

HRESULT CBullet::Add_Components(void* pArg)
{
    CBULLET_DESC* pDesc = static_cast<CBULLET_DESC*>(pArg);

    CBounding_Sphere::BOUND_SPHERE_DESC	 CBounding_Sphere{};
    _float3 Center{}, Extents{};
    CBounding_Sphere.fRadius = pDesc->fRadius;
    CBounding_Sphere.vCenter = _float3(0.f, 0.f, 0.f);
    
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CBounding_Sphere)))
        return E_FAIL;
    
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
        reinterpret_cast<CComponent**>(&m_pNavigationCom))))
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
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RgbStart", &m_Clolor[CSkill::COLOR::CSTART], sizeof(_float4))))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RgbEnd", &m_Clolor[CSkill::COLOR::CEND], sizeof(_float4))))
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
}
