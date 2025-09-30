#include "stdafx.h"
#include "Trail.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "VIBuffer_Trail.h"

CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CTrail::CTrail(const CTrail& Prototype) : CGameObject{Prototype}
{
}

HRESULT CTrail::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrail::Initialize(void* pArg)
{
    CTrail::CTrail_DESC* pDesc = static_cast<CTrail_DESC*>(pArg);
    m_fstartPos      = pDesc->fstartPoint;
    m_fendPos        = pDesc->fendPoint;
    m_fTrailLength   = pDesc->fTrailLength;
    m_fTrailWidth    = pDesc->fTrailWidth;
    m_iTrailSegments = pDesc->iTrailSegments;
    m_fClolor[CSkill::COLOR::CSTART] = pDesc->fClolor[CSkill::COLOR::CSTART];
    m_fClolor[CSkill::COLOR::CEND]   = pDesc->fClolor[CSkill::COLOR::CEND];
    m_bState = pDesc->pParantObject;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CTrail::Priority_Update(_float fTimeDelta)
{
    
}

void CTrail::Update(_float fTimeDelta)
{
    m_iLifeState = *m_bState;

    m_fTimeSum += fTimeDelta;

    if (m_fTimeSum >= 0.5f) 
    {
        m_fTimeSum = 0.f;
        m_currentFrame = (m_currentFrame + 1) % 4;
    }
  
    __super::Update(fTimeDelta);
}

void CTrail::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CTrail::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(6)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrail::Add_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Flash_output"),
                                      TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Point"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBufferPoint"), TEXT("Com_VIBuffer"),
                                      reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrail::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_StartPos", m_fstartPos, sizeof(_float3))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_EndPos", m_fendPos, sizeof(_float3)))) 
        return E_FAIL;
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom,"g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_TrailSegments", &m_iTrailSegments, sizeof _int)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_TrailLength", &m_fTrailLength, sizeof _float)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_TrailWidth", &m_fTrailWidth, sizeof _float)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_textureSize", &m_textureSize, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_frameSize", &m_frameSize, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_framesPerRow", &m_framesPerRow, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_currentFrame", &m_currentFrame, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof _float4)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RgbStart", &m_fClolor[CSkill::COLOR::CSTART], sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RgbEnd", &m_fClolor[CSkill::COLOR::CEND], sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
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
    Safe_Release(m_pShaderCom);
}
