#include "stdafx.h"
#include "PlayerEffectUI.h"
#include "GameInstance.h"

CPlayerEffectUI::CPlayerEffectUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{pDevice, pContext}
{
}

CPlayerEffectUI::CPlayerEffectUI(const CPlayerEffectUI& Prototype) : CUI{Prototype}
{
}

HRESULT CPlayerEffectUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayerEffectUI::Initialize(void* pArg)
{

    CPlayerEffectUI_DESC* pDesc = static_cast<CPlayerEffectUI_DESC*>(pArg);
    pDesc->fX = g_iWinSizeX*0.5f;
    pDesc->fY = g_iWinSizeY*0.5f;
    pDesc->fSizeX = g_iWinSizeX;
    pDesc->fSizeY = g_iWinSizeY;
    pDesc->UID = CUI::UIID_PlayerState;

    pDesc->Update = true;

    pDesc->fSpeedPerSec = 0.f;
    pDesc->fRotationPerSec = 0.f;
    m_iPlayerState = pDesc->State;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
    m_pTransformCom->Set_Scaling(pDesc->fSizeX, pDesc->fSizeY, 1.f);
    m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION,
                                   XMVectorSet(pDesc->fX - g_iWinSizeX * 0.5f, -pDesc->fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fShakingTime = 0.45f;
    m_alpha[0] = 0.1f;
    m_alpha[1] = 0.2f;
    m_alpha[2] = 0.3f;

    return S_OK;
}

_int CPlayerEffectUI::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;
 

    return OBJ_NOEVENT;
}

void CPlayerEffectUI::Update(_float fTimeDelta)
{
    if (true == m_bDisCard)
    {

        if (m_fShakingTime > 0.f)
        {

            m_fShakingTime -= fTimeDelta;
            m_fShaking_X = 10.f * 0.5f * 0.1f;
            m_fShaking_Y = 10.f * 0.5f * 0.1f;
            m_ScaleX += m_fShaking_X * 100.f;
            m_ScaleY += m_fShaking_Y * 100.f;

            m_alpha[0] -= fTimeDelta;
            m_alpha[1] -= fTimeDelta;
            m_alpha[2] -= fTimeDelta;
        }

        if (m_fShakingTime <= 0.f)
        {
            m_fShakingTime = 0.45f;
            m_fShaking_X = 0.f;
            m_fShaking_Y = 0.f;
            m_ScaleX = 0.f;
            m_ScaleY = 0.f;

            m_alpha[0] = 0.1f;
            m_alpha[1] = 0.2f;
            m_alpha[2] = 0.3f;
        }
    }
   
}

void CPlayerEffectUI::Late_Update(_float fTimeDelta)
{
    if (*m_iPlayerState == CPlayer::STATE_UI_SPRINT )
    {  
        m_bDisCard = true;
        m_iType = SPRINT;
        m_iPass = 3;
    }
    else
    if (*m_iPlayerState == CPlayer::STATE_UI_HIT)
    {   
        m_bDisCard = false;
        m_iType = HIT;
        m_iPass = 0;
        m_pTransformCom->Set_Scaling(g_iWinSizeX , g_iWinSizeY , 0.f);
    }
    else
    if (*m_iPlayerState == CPlayer::STATE_UI_HEALTH)
    {     
        m_bDisCard = false;
        m_iType = HEALTH;       
        m_iPass = 4;
        m_pTransformCom->Set_Scaling(g_iWinSizeX, g_iWinSizeY, 0.f);
    }
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;
}

HRESULT CPlayerEffectUI::Render()
{
    if (m_iType == IDlE)
        return S_OK;

    if (false == m_bDisCard) {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;


        if (FAILED(m_pTextureCom[m_iType]->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
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
            if (FAILED(m_pShaderCom->Bind_Int("g_Discard", i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &_float3{ m_alpha[0], m_alpha[1],m_alpha[2] }, sizeof(_float3))))
                return E_FAIL;

            if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
                return E_FAIL;
            if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
                return E_FAIL;


            if (FAILED(m_pTextureCom[m_iType]->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
                return E_FAIL;

            m_pShaderCom->Begin(m_iPass);

            m_pVIBufferCom->Bind_Buffers();

            m_pVIBufferCom->Render();
        }
    }

    return S_OK;
}

void CPlayerEffectUI::Set_scale(_int i)
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


HRESULT CPlayerEffectUI::Add_Components()
{

    /* For.Com_Texture */

        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sprint"),
            TEXT("Com_Texture_Sprint"),
            reinterpret_cast<CComponent**>(&m_pTextureCom[SPRINT]))))
            return E_FAIL;
    

        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UIEffect"),
            TEXT("Com_Texture_HIT"),
            reinterpret_cast<CComponent**>(&m_pTextureCom[HIT]))))
            return E_FAIL;
        
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UIEffect"),
            TEXT("Com_Texture_HEALTH"),
            reinterpret_cast<CComponent**>(&m_pTextureCom[HEALTH]))))
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

CPlayerEffectUI* CPlayerEffectUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayerEffectUI* pInstance = new CPlayerEffectUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayerEffectUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerEffectUI::Clone(void* pArg)
{
    CPlayerEffectUI* pInstance = new CPlayerEffectUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CPlayerEffectUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerEffectUI::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    for (_int i = 0; i < TYPE_END; i++) {
        Safe_Release(m_pTextureCom[i]);
    }


    Safe_Release(m_pShaderCom);
}
