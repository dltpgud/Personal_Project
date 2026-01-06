#include "stdafx.h"
#include "ShockWave_Bullet.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ShockWave.h"
#include "BillyBoom.h"

CShockWave_Bullet::CShockWave_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSkill{pDevice, pContext}
{
}

CShockWave_Bullet::CShockWave_Bullet(const CShockWave_Bullet& Prototype) : CSkill{Prototype}
{
}

HRESULT CShockWave_Bullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CShockWave_Bullet::Initialize(void* pArg)
{
   CBULLET_DESC* pDesc = static_cast<CBULLET_DESC*>(pArg);
     m_pTagetPos                     = pDesc->pTagetPos;
     m_pScale                        = pDesc->fScale;
     pDesc->iSkillType = CSkill::STYPE_NOMAL;
    if (FAILED(__super::Initialize(pDesc)))
       return E_FAIL; 

    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, m_vPos);

    m_vDir = m_pTagetPos - m_vPos;
    m_vDir = XMVector3Normalize(m_vDir);
    m_fLifeTime = 20.f;
  
    m_iTrailIndex = static_cast<CEffect_TrailStream*>(m_pGameInstance->Find_EffectStream(L"SpriteTexTrail"))->AllocateTrail();
    
    return S_OK;
}

void CShockWave_Bullet::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

     m_pTransformCom->Go_jump_Dir(fTimeDelta,m_vDir, -0.5f);     
}

void CShockWave_Bullet::Update(_float fTimeDelta)
{
    _float3 fPos;
    XMStoreFloat3(&fPos, m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION));
    CEffect_TrailStream::SPAWN_REQUEST req{};
    req.Valid = m_iLifeState == OBJ_POOL ? 0 : 1;
    req.trailIndex = m_iTrailIndex;
    req.headPos = fPos;
    req.addLife = 1.0f;
    req.width = 1.4f;
    req.color = m_Color[CSkill::COLOR::CEND];
    req.frameIndex = 1;
    req.isSegment = 0;
    m_pGameInstance->Trigger_Effect(L"SpriteTexTrail", &req);


    __super::Update(fTimeDelta);
}

void CShockWave_Bullet::Late_Update(_float fTimeDelta)
{

    if (FAILED(m_pGameInstance->Add_GameObject_To_ColGroup(this, Collider_Manager::CollGroup::COL_MONSTER_SKILL)))
        return;

    if (false == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION), 1.5f))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
    
    __super::Late_Update(fTimeDelta);
}

HRESULT CShockWave_Bullet::Render()
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

void CShockWave_Bullet::Dead_Rutine()
{
    m_iLifeState = OBJ_POOL;
}

HRESULT CShockWave_Bullet::CreateEffect(_vector RayStartPos, _vector RayDir, _vector RayEndPos, _vector vNomal, void* pArg)
{
      CShockWave::CShockWave_DESC Desc{};
      Desc.iDamage = m_iDamage;
      Desc.iSkillType = CSkill::SKill::STYPE_SHOCKWAVE;
      Desc.iActorType = CSkill::BOSS_MONSTER;
      Desc.vPos = XMVectorSetY(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION),XMVectorGetY(RayEndPos)-0.3f);
      m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                               L"Prototype_GameObject_ShockWave", &Desc);
  
     return S_OK;
}

HRESULT CShockWave_Bullet::Add_Components()
{
    CBounding_Sphere::BOUND_SPHERE_DESC CBounding_Sphere{};
    _float3 Center{}, Extents{};
    CBounding_Sphere.fRadius = 1.f;
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

HRESULT CShockWave_Bullet::Bind_ShaderResources()
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

CShockWave_Bullet* CShockWave_Bullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CShockWave_Bullet* pInstance = new CShockWave_Bullet(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CShockWave_Bullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CShockWave_Bullet::Clone(void* pArg)
{
    CShockWave_Bullet* pInstance = new CShockWave_Bullet(*this);
   
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CShockWave_Bullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShockWave_Bullet::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTrailTextureCom);
}
