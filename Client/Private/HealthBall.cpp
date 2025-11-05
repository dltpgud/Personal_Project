#include "stdafx.h"
#include "HealthBall.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_StateMachine.h"
#include "Player_HpUI.h"
CHealthBall::CHealthBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CHealthBall::CHealthBall(const CHealthBall& Prototype) : CGameObject{Prototype}
{
}

HRESULT CHealthBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHealthBall::Initialize(void* pArg)
{
    CHealthBall_DESC* pDesc = static_cast<CHealthBall_DESC*>(pArg);
    if (FAILED(__super::Initialize(pDesc)))
       return E_FAIL; ;

   m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, pDesc->vPos);

   _float theta = XMConvertToRadians(rand() % 360); 
   _float phi = XMConvertToRadians(rand() % 25 + 20);
   _float x = cosf(theta) * sinf(phi);
   _float y = cosf(phi);
   _float z = sinf(theta) * sinf(phi);
   _float3 RandomDir = _float3(x, y, z);
   
   _float3 dir = RandomDir;
   
    m_vDir = XMVector3Normalize(XMLoadFloat3(&dir));
    m_pScale = _float2(0.1f, 0.1f);
    
    m_fPrePos = {0.f, 0.f, 0.f};
    m_fCurPos = {0.f, 0.f, 0.f};

    m_pTransformCom->Set_MoveSpeed(5.f);
    m_bStop = false;
    // 고유한 트레일 인덱스 할당 (64개 제한)
    m_iTrailIndex =  static_cast<CEffect_TrailStream*>(m_pGameInstance->Find_EffectStream(L"CuTrail"))->AllocateTrail();
    return S_OK;
}

void CHealthBall::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION);
    _vector DirToPlayer = vPlayerPos - vPos;
    float fDist = XMVectorGetX(XMVector3Length(DirToPlayer));

    if (m_bStop == false)
    {
        m_pTransformCom->Go_jump_Dir(fTimeDelta, m_vDir, 0.5f, nullptr, &m_bStop);
    }

    if (fDist < 15 && m_bStop)
        m_pTransformCom->Set_MoveSpeed(20.f);

    if (fDist < 20.f ) 
    {
        DirToPlayer = XMVector3Normalize(DirToPlayer);
        m_pTransformCom->GO_Dir(fTimeDelta, DirToPlayer);
    }
    if (fDist < 1.f)
    {
        static_cast<CPlayer_HpUI*>(m_pGameInstance->Find_Clone_UIObj(L"PlayerHP"))->Set_HPGage(5);
        static_cast<CEffect_TrailStream*>(m_pGameInstance->Find_EffectStream(L"CuTrail"))->ReleaseTrail(m_iTrailIndex);
        m_iLifeState = OBJ_POOL;
    }
}

void CHealthBall::Update(_float fTimeDelta)
{  
    __super::Update(fTimeDelta);
}

void CHealthBall::Late_Update(_float fTimeDelta)
{
    _float3 fPos;
    XMStoreFloat3(&fPos, m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION));
    CEffect_TrailStream::SPAWN_REQUEST req{};
    req.trailIndex = m_iTrailIndex;
    req.headPos = fPos;
    req.addLife = 1.0f;
    req.width = 0.3f;   
    req.color = _float4(0.f,1.f,0.f,1.f);
    m_pGameInstance->Trigger_Effect(L"CuTrail", &req);

    if (false == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION), 1.5f))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
  
    __super::Late_Update(fTimeDelta);
}

HRESULT CHealthBall::Render()
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

HRESULT CHealthBall::Add_Components()
{
    CBounding_Sphere::BOUND_SPHERE_DESC CBounding_Sphere{};
    _float3 Center{}, Extents{};
    CBounding_Sphere.fRadius = 0.3f;
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

HRESULT CHealthBall::Bind_ShaderResources()
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_RgbStart", &m_Clolor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_RgbEnd", &m_Clolor, sizeof(_float4))))
        return E_FAIL;

  return S_OK;
}

CHealthBall* CHealthBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHealthBall* pInstance = new CHealthBall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHealthBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHealthBall::Clone(void* pArg)
{
    CHealthBall* pInstance = new CHealthBall(*this);
   
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CHealthBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHealthBall::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
