#include "stdafx.h"
#include "BossHP.h"
#include "GameInstance.h"

CBossHPUI::CBossHPUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{pDevice, pContext}
{
}

CBossHPUI::CBossHPUI(const CBossHPUI& Prototype) : CUI{Prototype}
{
}

HRESULT CBossHPUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossHPUI::Initialize(void* pArg)
{
    CBossHPUI_DESC* Desc = static_cast <CBossHPUI_DESC*>(pArg);
    Desc->fX = g_iWinSizeX * 0.5f;
    Desc->fY = 50.f;
    Desc->fZ = 0.1f;
    Desc->fSizeX = 650.f;
    Desc->fSizeY = 25.f;
    Desc->UID = UIID_BossHP ;
    Desc->fSpeedPerSec = 0.f;
    Desc->fRotationPerSec = 0.f;
    m_fHP = Desc->fHP;
    m_fMaxHP = Desc->fMaxHP;

    if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(Desc->fSizeX, Desc->fSizeY, 1.f);
    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,
                                   XMVectorSet(Desc->fX - g_iWinSizeX * 0.5f, -Desc->fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
    m_fPrXPos = Desc->fX;
    m_fPrYPos = Desc->fY;
    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fHP_Pluse = m_fHP  / m_fMaxHP;
    m_fRatio = m_fHP * 0.001f / m_fMaxHP;
    
    m_fHealthHP = 1.f;
    return S_OK;
}

void CBossHPUI::Priority_Update(_float fTimeDelta)
{
}

void CBossHPUI::Update(_float fTimeDelta)
{
    if (m_fHP_Pluse != m_fHP / m_fMaxHP)
    {
        m_fHP_Pluse -= m_fRatio;

        if (m_fHP_Pluse <= m_fHP / m_fMaxHP)
        {
            m_fHP_Pluse = (m_fHP / m_fMaxHP);
        }
    }
}

void CBossHPUI::Late_Update(_float fTimeDelta)
{
    m_CurRito = m_fHP / m_fMaxHP; 
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this))) 
        return;
    if (FAILED(m_pGameInstance->ADD_UI_ShakingList(this)))
        return;
}

HRESULT CBossHPUI::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_hp",  &m_CurRito , sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_hp_pluse",&m_fHP_Pluse, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",0)))
        return E_FAIL;

    m_pShaderCom->Begin(2);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CBossHPUI::ShakingEvent(_float fTimeDelta)
{
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
    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,
                                   XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
}


HRESULT CBossHPUI::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_BossHP"),
                                      TEXT("Com_Texture"),
                                      reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
                                      reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CBossHPUI* CBossHPUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBossHPUI* pInstance = new CBossHPUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBossHPUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBossHPUI::Clone(void* pArg)
{
    CBossHPUI* pInstance = new CBossHPUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBossHPUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBossHPUI::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
