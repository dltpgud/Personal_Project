#include "stdafx.h"
#include "Player_StateUI.h"
#include "GameInstance.h"
#include "Player_StateNode.h"
CPlayer_StateUI::CPlayer_StateUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{pDevice, pContext}
{
}

CPlayer_StateUI::CPlayer_StateUI(const CPlayer_StateUI& Prototype) : CUI{Prototype}
{
}

HRESULT CPlayer_StateUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer_StateUI::Initialize(void* pArg)
{
    CPlayerEffectUI_DESC* pDesc = static_cast<CPlayerEffectUI_DESC*>(pArg);
    pDesc->fX = g_iWinSizeX*0.5f;
    pDesc->fY = g_iWinSizeY*0.5f;
    pDesc->fSizeX = g_iWinSizeX;
    pDesc->fSizeY = g_iWinSizeY;
    pDesc->UID = UIID_PlayerState;
    pDesc->Update = false;
    pDesc->fSpeedPerSec = 0.f;
    pDesc->fRotationPerSec = 0.f;
    m_pPlayerState = pDesc->State;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(pDesc->fSizeX, pDesc->fSizeY, 1.f);
    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,XMVectorSet(pDesc->fX - g_iWinSizeX * 0.5f, -pDesc->fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fShakingTime = 0.45f;
    m_alpha = {0.1f, 0.2f, 0.3f};

    return S_OK;
}

void CPlayer_StateUI::Priority_Update(_float fTimeDelta)
{ 
}

void CPlayer_StateUI::Update(_float fTimeDelta)
{
    if (*m_pPlayerState & CPlayer_StateNode::MOV_SPRINT)
    {
        m_bDisCard = true;
        m_iType = SPRINT;
        m_iPass = 3;
    }
    else if ((*m_pPlayerState & CPlayer_StateNode::MOV_HIT) || (*m_pPlayerState & CPlayer_StateNode::MOV_FALL))
    {
        m_bDisCard = false;
        m_iType = DEFULT;
        m_iPass = 0;
        m_pTransformCom->Set_Scaling(g_iWinSizeX, g_iWinSizeY, 0.f);
    }
    else if (*m_pPlayerState & CPlayer_StateNode::MOV_HEALTH)
    {
        m_bDisCard = false;
        m_iType = COLOR;
        m_fStateColor = {0.f, 1.f, 0.f};
        m_iPass = 4;
        m_pTransformCom->Set_Scaling(g_iWinSizeX, g_iWinSizeY, 0.f);
    }
    else if (*m_pPlayerState & CPlayer_StateNode::MOV_STURN)
    {
        m_bDisCard = false;
        m_iType = COLOR;
        m_fStateColor = {1.f, 0.f, 0.1f};
        m_iPass = 4;
        m_pTransformCom->Set_Scaling(g_iWinSizeX, g_iWinSizeY, 0.f);
    }
}

void CPlayer_StateUI::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;

    if (FAILED(m_pGameInstance->ADD_UI_ShakingList(this)))
        return;
}

HRESULT CPlayer_StateUI::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom[m_iType]->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;


    if (false == m_bDisCard) {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_StateColor", &m_fStateColor, sizeof(_float3))))
                return E_FAIL;
        
        m_pShaderCom->Begin(m_iPass);

        m_pVIBufferCom->Bind_Buffers();

        m_pVIBufferCom->Render();
    }
    else
    if (true == m_bDisCard)
    {
        for (_int i = 0; i < 3; i++) {
            Set_scale(i);
            if (FAILED(m_pShaderCom->Bind_RawValue("g_Discard", &i, sizeof(_int))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_alpha, sizeof(_float3))))
                return E_FAIL;

            if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
                return E_FAIL;

            m_pShaderCom->Begin(m_iPass);

            m_pVIBufferCom->Bind_Buffers();

            m_pVIBufferCom->Render();
        }
    }

    return S_OK;
}

void CPlayer_StateUI::ShakingEvent(_float fTimeDelta)
{
    if (false == m_bDisCard)
        return;

    if (m_fShakingTime > 0.f)
    {
        m_fShakingTime -= fTimeDelta;
        m_fShaking_X = 10.f * 0.5f * 0.1f;
        m_fShaking_Y = 10.f * 0.5f * 0.1f;
        m_ScaleX += m_fShaking_X * 100.f;
        m_ScaleY += m_fShaking_Y * 100.f;
   
        m_alpha.x -= fTimeDelta;
        m_alpha.y -= fTimeDelta;
        m_alpha.z -= fTimeDelta;
    }
   
    if (m_fShakingTime <= 0.f)
    {
        m_fShakingTime = 0.45f;
        m_fShaking_X = 0.f;
        m_fShaking_Y = 0.f;
        m_ScaleX = 0.f;
        m_ScaleY = 0.f;
   
        m_alpha.x = 0.1f;
        m_alpha.y = 0.2f;
        m_alpha.z = 0.3f;
    }
}

void CPlayer_StateUI::Set_scale(_int i)
{
    if(i == 0)
    {
        m_pTransformCom->Set_Scaling(g_iWinSizeX+100.f +m_ScaleX, g_iWinSizeY+50.f+ m_ScaleY, 0.f);
    }
    if (i == 1)
    {
        m_pTransformCom->Set_Scaling(g_iWinSizeX + 100.f + m_ScaleX + m_ScaleX*0.8f, g_iWinSizeY + 50.f + m_ScaleY, 0.f);
    }
    if (i == 2)
    {
        m_pTransformCom->Set_Scaling(g_iWinSizeX + 100.f + m_ScaleX + m_ScaleX * 0.5f, g_iWinSizeY + 50.f + m_ScaleY, 0.f);
    }           
}


HRESULT CPlayer_StateUI::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sprint"), TEXT("Com_Texture_Sprint"),
                                      reinterpret_cast<CComponent**>(&m_pTextureCom[SPRINT]))))
        return E_FAIL;
   
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UIEffect"),TEXT("Com_Texture_HIT"),
                                      reinterpret_cast<CComponent**>(&m_pTextureCom[DEFULT]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UIEffect"), TEXT("Com_Texture_HEALTH"),
                                      reinterpret_cast<CComponent**>(&m_pTextureCom[COLOR]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
                                      reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CPlayer_StateUI* CPlayer_StateUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer_StateUI* pInstance = new CPlayer_StateUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayerEffectUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer_StateUI::Clone(void* pArg)
{
    CPlayer_StateUI* pInstance = new CPlayer_StateUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CPlayerEffectUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_StateUI::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);

    for (_int i = 0; i < TYPE_END; i++) {
        Safe_Release(m_pTextureCom[i]);
    }

    Safe_Release(m_pShaderCom);
}
