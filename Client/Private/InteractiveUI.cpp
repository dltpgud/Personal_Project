#include "stdafx.h"
#include "InteractiveUI.h"
#include "GameInstance.h"

CInteractiveUI::CInteractiveUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{pDevice, pContext}
{
}

CInteractiveUI::CInteractiveUI(const CInteractiveUI& Prototype) : CUI{Prototype}
{
}

HRESULT CInteractiveUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInteractiveUI::Initialize(void* pArg)
{
    CUI_DESC Desc{};
    Desc.fX = g_iWinSizeX * 0.5f-40.f;
    Desc.fY = g_iWinSizeY * 0.5f;
    Desc.fZ =  0.f;
    Desc.fSizeX = 50.f;
    Desc.fSizeY = 50.f;
    Desc.Update = true;

    Desc.UID = UIID_InteractiveUI;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION,
                                   XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

_int CInteractiveUI::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    if (m_pGameInstance->Get_DIKeyDown(DIK_F))
    {
        
        m_bColor = true;
     
    }


    return OBJ_NOEVENT;
}

void CInteractiveUI::Update(_float fTimeDelta)
{
    if (m_bColor) 
    {
        m_ColorTime += fTimeDelta;
         m_vColor = { 0.f,1.f,0.f,1.f };
        if (m_ColorTime > 0.2f)
        {
            m_bInteractive = true;
            m_ColorTime = 0.f;
            m_bColor = false;
        };
    }
    else
        m_vColor = { 1.f,1.f,1.f,1.f };
    
}

void CInteractiveUI::Late_Update(_float fTimeDelta)
{

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;
}

HRESULT CInteractiveUI::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

    if(nullptr !=m_pText)
    m_pGameInstance->Render_Text(TEXT("Robo"), m_pText, _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f-20.f ), m_vColor, 0.4f);

    return S_OK;
}

_bool CInteractiveUI::Get_Interactive()
{
    return m_bInteractive; 
}



void CInteractiveUI::Set_Interactive(_bool Interactive)
{
    m_bInteractive = Interactive;
}

HRESULT CInteractiveUI::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_KeyBoard_F"), TEXT("Com_Texture_Interactive"),
                                      reinterpret_cast<CComponent**>(&m_pTextureCom))))
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

CInteractiveUI* CInteractiveUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInteractiveUI* pInstance = new CInteractiveUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("InteractiveUI Create Failed");
        Engine::Safe_Release(pInstance);
    }

    return pInstance;
    ;
}

CGameObject* CInteractiveUI::Clone(void* pArg)
{
    CInteractiveUI* pInstance = new CInteractiveUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CInteractiveUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInteractiveUI::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
