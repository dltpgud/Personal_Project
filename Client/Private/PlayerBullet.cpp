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
    CPlayerBullet_DESC* pDesc = static_cast<CPlayerBullet_DESC*>(pArg);
        
        m_vTagetPos = pDesc->vTagetPos;
        m_Local = pDesc->Local;
        m_WorldPtr = pDesc->WorldPtr;
        m_fScale = pDesc->fScale;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (0.f == m_fLifeTime)
        m_fLifeTime = 1.f;

    return S_OK;
}

void CPlayerBullet::Priority_Update(_float fTimeDelta)
{
    if (true == m_bStart)
    {
        m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
    }
    __super::Priority_Update(fTimeDelta);

}

void CPlayerBullet::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CPlayerBullet::Late_Update(_float fTimeDelta)
{
    if (false == m_bStart) {
        _vector  vHPos = XMVector3TransformCoord(m_Local, XMLoadFloat4x4(m_WorldPtr));
        m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, vHPos);
        _vector Dir = m_vTagetPos - vHPos;
        Dir = XMVectorSetW(Dir, 0.f);
        m_vDir = XMVector3Normalize(Dir);
        m_bStart = true;
    }

    if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION), 1.5f))
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
            return;
    }
    __super::Late_Update(fTimeDelta);
}

void CPlayerBullet::Dead_Rutine()
{
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

HRESULT CPlayerBullet::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PlayerBullet"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Point"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_PSize", &m_fScale, sizeof(_float2))))
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
    CPlayerBullet* pInstance =  new CPlayerBullet(*this);
  
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
