#include "stdafx.h"
#include "Trail.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ShockWave.h"
#include "Shock.h"
#include "BillyBoom.h"
CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSkill{pDevice, pContext}
{
}

CTrail::CTrail(const CTrail& Prototype) : CSkill{Prototype}
{
}

HRESULT CTrail::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrail::Initialize(void* pArg)
{

    CTrail_DESC* pDesc = static_cast<CTrail_DESC*>(pArg);
        m_pTagetPos = pDesc->pTagetPos;
         m_pParentState = pDesc->state;
         m_NPos = pDesc->NPos;
        if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL; 


       
 if (FAILED(BIND_BULLET_TYPE()))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

 
    if (0.f == m_fLifeTime)
        m_fLifeTime = 1.f;

    return S_OK;
}

_int CTrail::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        return OBJ_DEAD;
    }  
    
    __super::Priority_Update(fTimeDelta);


    
    m_fNTimeSum += fTimeDelta;

    if (m_fNTimeSum >= 0.1f)
    {
        m_fNTimeSum -= 0.1f;
        m_currentFrame = (m_currentFrame + 1) % 8;
    }

   
    return OBJ_NOEVENT;

}

void CTrail::Update(_float fTimeDelta)
{ 
        

        m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, *m_NPos);
        __super::Update(fTimeDelta);
}

void CTrail::Late_Update(_float fTimeDelta)
{

    if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION), 1.5f))
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
            return;
    }


    __super::Late_Update(fTimeDelta);
}

HRESULT CTrail::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",0)))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Begin(3)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CTrail::Dead_Rutine(_float fTimeDelta)
{
    m_bDead = true;
}


HRESULT CTrail::Add_Components()
{
   
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Flash_output"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Point"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBufferPoint"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrail::Bind_ShaderResources()
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



     if (FAILED(m_pShaderCom->Bind_RawValue("g_textureSize", &m_textureSize, sizeof(_float2))))
         return E_FAIL;
     if (FAILED(m_pShaderCom->Bind_RawValue("g_frameSize", &m_frameSize, sizeof(_float2))))
         return E_FAIL;
     if (FAILED(m_pShaderCom->Bind_RawValue("g_framesPerRow", &m_framesPerRow, sizeof(_int))))
         return E_FAIL;

     if (FAILED(m_pShaderCom->Bind_RawValue("g_currentFrame", &m_currentFrame, sizeof(_int))))
         return E_FAIL;



    return S_OK;
}

HRESULT CTrail::BIND_BULLET_TYPE()
{

    switch (m_iActorType)
    {
     
    case CSkill::MONSTER::TYPE_GUNPAWN:
        m_pScale.x = 0.2f;
        m_pScale.y = 0.2f;
        m_Clolor[CSkill::COLOR::CSTART] = _float4(0.f, 0.f, 0.f, 0.f);
        m_Clolor[CSkill::COLOR::CEND] = _float4(1.f, 0.f, 0.f, 1.f);
        break;

    case CSkill::MONSTER::TYPE_BOOMBOT:
        m_pScale.x = 0.2f;
        m_pScale.y = 0.2f;
        m_Clolor[CSkill::COLOR::CSTART] = _float4(0.f, 0.f, 0.f, 0.f);
        m_Clolor[CSkill::COLOR::CEND] = _float4(1.f, 0.1f, 1.f, 1.f); // ³ë¶û
        break;

    case CSkill::MONSTER::TYPE_JETFLY:
        m_pScale.x = 0.2f;
        m_pScale.y = 0.2f;
        m_Clolor[CSkill::COLOR::CSTART] = _float4(0.f, 0.f, 0.f, 0.f);
        m_Clolor[CSkill::COLOR::CEND] = _float4(1.f, 0.f, 0.f, 1.f);
        break;

    case CSkill::MONSTER::TYPE_MECANOBOT:
        m_pScale.x = 0.2f;
        m_pScale.y = 0.2f;
        m_Clolor[CSkill::COLOR::CSTART] = _float4(0.f, 0.f, 0.f, 0.f);
        m_Clolor[CSkill::COLOR::CEND] = _float4(1.f, 0.f, 0.f, 1.f);
        break;
    case CSkill::MONSTER::TYPE_BILLYBOOM:
        m_pScale.x = 1.f;
        m_pScale.y = 1.f;
        m_Clolor[CSkill::COLOR::CSTART] = _float4(1.f, 0.f, 0.f, 1.f);
        m_Clolor[CSkill::COLOR::CEND] = _float4(1.f, 1.f, 0.f, 1.f); // ³ë¶û
        break;
    }

    return S_OK;
}


CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTrail* pInstance = new CTrail(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTrail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTrail::Clone(void* pArg)
{
    CTrail* pInstance = new CTrail(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CTrail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTrail::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

}
