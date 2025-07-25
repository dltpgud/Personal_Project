#include "stdafx.h"
#include "PlayerUI.h"
#include "GameInstance.h"

CPlayerUI::CPlayerUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{pDevice, pContext}
{
}

CPlayerUI::CPlayerUI(const CPlayerUI& Prototype) : CUI{Prototype}
{
}

HRESULT CPlayerUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayerUI::Initialize(void* pArg)
{
    CUI_DESC* pDesc = static_cast<CUI_DESC*>(pArg);

    pDesc->fX = 145.f;
    pDesc->fY = 640.f;
    pDesc->fSizeX = 450.f;
    pDesc->fSizeY = 150.f;
    pDesc->UID = CUI::UIID_PlayerHP;
    pDesc->Update = true;
       pDesc->fSpeedPerSec = 0.f;
    pDesc->fRotationPerSec = 0.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
    m_pTransformCom->Set_Scaling(pDesc->fSizeX, pDesc->fSizeY, 1.f);
    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,
                                   XMVectorSet(pDesc->fX - g_iWinSizeX * 0.5f, -pDesc->fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

    m_fPrXPos = pDesc->fX;
    m_fPrYPos = pDesc->fY;
    if (FAILED(Add_Components()))
        return E_FAIL;


    m_fHealthHP = 1.f;
    return S_OK;
}

void CPlayerUI::Priority_Update(_float fTimeDelta)
{
  if (m_IsShaking) {
      if (m_fShakingTime > 0.f)
      {
          m_fX += m_fShaking_X;
          m_fY += m_fShaking_Y;
  
      }
      if (m_fShakingTime <= 0.f)
      {
          m_fX = m_fPrXPos;
          m_fY = m_fPrYPos;
      }
  }
}

void CPlayerUI::Update(_float fTimeDelta)
{
    if (m_iGageCount != -1)
    {
        m_iGageCount--;
        m_fHP += m_fHealthHP;
   
        m_pGameInstance->Get_Player()->Set_HealthCurrentHP(m_fHealthHP);

        if (true == m_pGameInstance->Get_Player()->IsFullHP() || m_iGageCount == 0)
                m_iGageCount = -1;  
    }
}

void CPlayerUI::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;
}

HRESULT CPlayerUI::Render()
{
    __super::Render();

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float Cur = m_fHP / m_fMaxHP;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_hp", &Cur, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_hp_pluse", &m_fHP_Pluse, sizeof(_float))))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom_HpBar->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
        return E_FAIL;
    if (FAILED(m_pTextureCom_HpBar->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
        return E_FAIL;
    if (FAILED(m_pTextureCom_HpBar->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 2)))
        return E_FAIL;

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

    m_pGameInstance->Render_Text(TEXT("Robo"), m_tfHP, _float2(m_fX + 200.f, m_fY + 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f);
    m_pGameInstance->Render_Text(TEXT("Robo"), L"/", _float2(m_fX + 250.f, m_fY + 10.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f);
 m_pGameInstance->Render_Text(TEXT("Robo"), m_tfMaxHp, _float2(m_fX + 265.f, m_fY+10.f ), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f);

    return S_OK;
}


HRESULT CPlayerUI::Add_Components()
{

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_HpBar"),
                                      TEXT("Com_Texture_PlayerHpBar"),
                                      reinterpret_cast<CComponent**>(&m_pTextureCom_HpBar))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
                                      reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CPlayerUI* CPlayerUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayerUI* pInstance = new CPlayerUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayerUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerUI::Clone(void* pArg)
{
    CPlayerUI* pInstance = new CPlayerUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CPlayerUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerUI::Free()
{

    __super::Free();

    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pTextureCom_HpBar);

    Safe_Release(m_pShaderCom);
}
