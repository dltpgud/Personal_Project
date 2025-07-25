#include "stdafx.h"
#include "WeaponUI.h"
#include "GameInstance.h"
#include "Player.h"

CWeaponUI::CWeaponUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{pDevice, pContext}
{
}

CWeaponUI::CWeaponUI(const CWeaponUI& Prototype) : CUI{Prototype}
{
}

HRESULT CWeaponUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeaponUI::Initialize(void* pArg)
{
    CUI_DESC* pDesc = static_cast<CUI_DESC*>(pArg);

    pDesc->fX = 1170.f;
    pDesc->fY = 640.f;
    pDesc->fSizeX = 500.f;
    pDesc->fSizeY = 100.f;
    pDesc->UID = CUI::UIID_PlayerWeaPon;
      pDesc->Update = true;
       pDesc->fSpeedPerSec = 0.f;
    pDesc->fRotationPerSec = 0.f;
    m_fXPos = 1150.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(pDesc->fSizeX, pDesc->fSizeY, 1.f);
    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,
        XMVectorSet(pDesc->fX - g_iWinSizeX * 0.5f, -pDesc->fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_WeaponUI_Pos()))
        return E_FAIL;

    if (FAILED(Set_WeaponUI_Pos2()))
        return E_FAIL;

    return S_OK;
}

void CWeaponUI::Priority_Update(_float fTimeDelta)
{
    if (m_IsShaking)
    {
        if (m_fShakingTime > 0.f)
        {
            for (_int i = 0; i < 6; i++)
            {
                m_Desc[i].fX += m_fShaking_X;
                m_Desc[i].fY += m_fShaking_Y;
            }

            for (_int i = 0; i < 5; i++)
            {
                m_ScecondDesc[i].fX += m_fShaking_X;
                m_ScecondDesc[i].fY += m_fShaking_Y;
            }
        }
        else if (m_fShakingTime <= 0.f)
        {

            for (_int i = 0; i < 6; i++)
            {
                m_Desc[i].fX = m_fPrXPos[i];
                m_Desc[i].fY = m_fPrYPos[i];
            }

            for (_int i = 0; i < 5; i++)
            {
                m_ScecondDesc[i].fX = m_fPrXPos2[i];
                m_ScecondDesc[i].fY = m_fPrYPos2[i];
            }
        }
    }
}

void CWeaponUI::Update(_float fTimeDelta)
{
     if (nullptr == m_pGameInstance->Get_Player())
        return;
     
     m_iPreBullet = m_iCurrentBullet;

      m_iCurrentBullet = static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_Bullet();
     swprintf_s(m_wCurrentBullte,50, L"%u",m_iCurrentBullet);

     if (m_iPreBullet != m_iCurrentBullet)
         m_vColar = { 1.f, 0.f, 0.f,1.f };
     else 
         m_vColar = { 1.f, 1.f, 1.f,1.f };
      m_iMaxBullet = static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_MaxBullte();
     swprintf_s(m_wMaxBullte, 50, L"%u", m_iMaxBullet);
 
     

    if (m_pGameInstance->Get_DIKeyDown(DIK_F1))
    {
        static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Choose_Weapon(CWeapon::HendGun);
        m_WeaPonUI = CWeapon::HendGun;
        m_bChooseWeaPon = true;
    }
    if (m_pGameInstance->Get_DIKeyDown(DIK_F2))
    {
        static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Choose_Weapon(CWeapon::AssaultRifle);
        m_WeaPonUI = CWeapon::AssaultRifle;
        m_bChooseWeaPon = true;
    }
    if (m_pGameInstance->Get_DIKeyDown(DIK_F3))
    {
        static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Choose_Weapon(CWeapon::MissileGatling);
        m_WeaPonUI = CWeapon::MissileGatling;
        m_bChooseWeaPon = true;
    }
    if (m_pGameInstance->Get_DIKeyDown(DIK_F4))
    {
        static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Choose_Weapon(CWeapon::HeavyCrossbow);
        m_WeaPonUI = CWeapon::HeavyCrossbow;
        m_bChooseWeaPon = true;
    }

    if (m_pGameInstance->Get_DIKeyDown(DIK_C))
    {
        m_change = true;

        if (true == m_bRander)
        {
            if (true == m_change)
            {
                _uint WeaPonUI{};
                WeaPonUI = m_ScecondWeaPonUI;
                m_ScecondWeaPonUI = m_WeaPonUI;
                m_WeaPonUI = WeaPonUI;
                m_change = false;
                m_bChooseWeaPon = true;
            }
        }
    }
    else
    {
        m_change = false;
    }

    if (m_bChooseWeaPon)
    {
        switch (m_WeaPonUI)
        {
        case Client::CWeapon::HendGun:
            static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Choose_Weapon(CWeapon::HendGun);
            break;
        case Client::CWeapon::AssaultRifle:
            static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Choose_Weapon(CWeapon::AssaultRifle);
            break;
        case Client::CWeapon::MissileGatling:
            static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Choose_Weapon(CWeapon::MissileGatling);
            break;
        case Client::CWeapon::HeavyCrossbow:
            static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Choose_Weapon(CWeapon::HeavyCrossbow);
            break;
        default: break;
        }

        m_bChooseWeaPon = false;
    }
}

void CWeaponUI::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
        return;
}

HRESULT CWeaponUI::Render()
{
   // if (m_bPrUpdate)
        Set_UI_Pos(&m_Desc[4]);
    if (false == m_IsShaking)
    {
        m_pTransformCom->Set_Scaling(m_Desc[4].fSizeX, m_Desc[4].fSizeY, 1.f);
        m_pTransformCom->Set_TRANSFORM(
            CTransform::T_POSITION,
            XMVectorSet(m_Desc[4].fX - ViewportDesc.Width * 0.5f, -m_Desc[4].fY + ViewportDesc.Height * 0.5f, 0.5f, 1.f));


    }

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    ;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

  //  if (m_bPrUpdate)
        Set_UI_Pos(&m_ScecondDesc[4]);
    if (false == m_IsShaking)
    {
        m_pTransformCom->Set_Scaling(m_ScecondDesc[4].fSizeX, m_ScecondDesc[4].fSizeY, 1.f);
        m_pTransformCom->Set_TRANSFORM(
            CTransform::T_POSITION,
            XMVectorSet(m_ScecondDesc[4].fX - ViewportDesc.Width * 0.5f, -m_ScecondDesc[4].fY + ViewportDesc.Height * 0.5f, 0.5f, 1.f));
   
   
    }
   
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    ;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
   
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
   
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
        return E_FAIL;
   
    m_pShaderCom->Begin(0);
   
    m_pVIBufferCom->Bind_Buffers();
   
    m_pVIBufferCom->Render();

    for (_uint i = 0; i < 4; i++)
    {//if(m_bPrUpdate)
        Set_UI_Pos(&m_Desc[i]);

        if (m_WeaPonUI != i)
            continue;

        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;
        ;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pVIBufferCom->Bind_Buffers();

        m_pVIBufferCom->Render();
    }

    for (_uint i = 4; i < 6; i++)
    {
        //if (m_bPrUpdate)
        Set_UI_Pos(&m_Desc[i]);

        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;
        ;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pVIBufferCom->Bind_Buffers();

        m_pVIBufferCom->Render();
    }

    if (m_bRander)
    {
        for (_uint i = 0; i < 4; i++)
        {
           // if (m_bPrUpdate)
            Set_UI_Pos(&m_ScecondDesc[i]);

            if (m_ScecondWeaPonUI != i)
                continue;


            if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
                return E_FAIL;
            ;
            if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
                return E_FAIL;

            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i)))
                return E_FAIL;

            m_pShaderCom->Begin(0);

            m_pVIBufferCom->Bind_Buffers();

            m_pVIBufferCom->Render();
        }
    }


    //최대 탄창

    if(m_iMaxBullet>10)
    m_pGameInstance->Render_Text(TEXT("Robo"), m_wMaxBullte, _float2(m_Desc[5].fX - 3.f, m_Desc[5].fY - 13.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.4f);
    else
    m_pGameInstance->Render_Text(TEXT("Robo"), m_wMaxBullte, _float2(m_Desc[5].fX + 3.f, m_Desc[5].fY - 13.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.4f);

    m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("/"), _float2(m_Desc[5].fX-13.f, m_Desc[5].fY-23.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.4f);

    //현재 탄

    if(m_iCurrentBullet >=10)
    m_pGameInstance->Render_Text(TEXT("Robo"), m_wCurrentBullte, _float2(m_Desc[5].fX - 58.f, m_Desc[5].fY - 48.f), m_vColar, 0.6f);
    else 
        m_pGameInstance->Render_Text(TEXT("Robo"), m_wCurrentBullte, _float2(m_Desc[5].fX - 38.f, m_Desc[5].fY - 48.f), m_vColar, 0.6f);
    return S_OK;
}

HRESULT CWeaponUI::Set_WeaponUI_Pos()
{
    /*hendGun*/
    m_Desc[0].UID = UIID_PlayerWeaPon;
    m_Desc[0].fX = m_fXPos;
    m_Desc[0].fY = 650.f;
    m_Desc[0].fZ = 0.4f;
    m_Desc[0].fSizeX = 220.f;
    m_Desc[0].fSizeY = 75.f;
    m_Desc[0].Update = true;

    m_Desc[0].fSpeedPerSec = 0.f;

    /*AssaultRifle*/
    m_Desc[1].UID = UIID_PlayerWeaPon;
    m_Desc[1].fX = m_fXPos;
    m_Desc[1].fY = 650.f;
    m_Desc[1].fZ = 0.4f;
    m_Desc[1].fSizeX = 200.f;
    m_Desc[1].fSizeY = 75.f;

    m_Desc[1].Update = true;

    m_Desc[1].fSpeedPerSec = 0.f;

    /*MissileGatling*/
    m_Desc[2].UID = UIID_PlayerWeaPon;
    m_Desc[2].fX = m_fXPos;
    m_Desc[2].fY = 650.f;
    m_Desc[2].fZ = 0.4f;
    m_Desc[2].fSizeX = 200.f;
    m_Desc[2].fSizeY = 70.f;

    m_Desc[2].Update = true;

    m_Desc[2].fSpeedPerSec = 0.f;

    /*HeavyCrossbow*/
    m_Desc[3].UID = UIID_PlayerWeaPon;
    m_Desc[3].fX = m_fXPos;
    m_Desc[3].fY = 650.f;
    m_Desc[3].fZ = 0.4f;
    m_Desc[3].fSizeX = 200.f;
    m_Desc[3].fSizeY = 70.f;

    m_Desc[3].Update = true;

    m_Desc[3].fSpeedPerSec = 0.f;

    /*Fream*/
    m_Desc[4].UID = UIID_PlayerWeaPon;
    m_Desc[4].fX = m_fXPos;
    m_Desc[4].fY = 650.f;
    m_Desc[4].fZ = 0.5f;
    m_Desc[4].fSizeX = 200.f;
    m_Desc[4].fSizeY = 95.f;

    m_Desc[4].Update = true;

    m_Desc[4].fSpeedPerSec = 0.f;

    /*Tan*/
    m_Desc[5].UID = UIID_PlayerWeaPon;
    m_Desc[5].fX = 1000.f;
    m_Desc[5].fY = 670.f;
    m_Desc[5].fZ = 0.5f;
    m_Desc[5].fSizeX = 70.f;
    m_Desc[5].fSizeY = 55.f;
    m_Desc[5].Update = true;
    m_Desc[5].fSpeedPerSec = 0.f;

    for (_int i = 0; i < 6; i++)
    {
        m_fPrXPos[i] = m_Desc[i].fX;
        m_fPrYPos[i] = m_Desc[i].fY;
    }

    return S_OK;
}

HRESULT CWeaponUI::Set_WeaponUI_Pos2()
{
    /*hendGun*/
    m_ScecondDesc[0].UID = UIID_PlayerWeaPon;
    m_ScecondDesc[0].fX = m_fXPos + 40.f;
    m_ScecondDesc[0].fY = 580.f;
    m_ScecondDesc[0].fZ = 0.4f;
    m_ScecondDesc[0].fSizeX = 110.f;
    m_ScecondDesc[0].fSizeY = 35.f;

    m_ScecondDesc[0].Update = true;

    m_ScecondDesc[0].fSpeedPerSec = 0.f;

    /*AssaultRifle*/
    m_ScecondDesc[1].UID = UIID_PlayerWeaPon;
    m_ScecondDesc[1].fX = m_fXPos + 40.f;
    m_ScecondDesc[1].fY = 580.f;
    m_ScecondDesc[1].fZ = 0.4f;
    m_ScecondDesc[1].fSizeX = 110.f;
    m_ScecondDesc[1].fSizeY = 35.f;

    m_ScecondDesc[1].Update = true;

    m_ScecondDesc[1].fSpeedPerSec = 0.f;

    /*MissileGatling*/
    m_ScecondDesc[2].UID = UIID_PlayerWeaPon;
    m_ScecondDesc[2].fX = m_fXPos + 40.f;
    m_ScecondDesc[2].fY = 580.f;
    m_ScecondDesc[2].fZ = 0.4f;
    m_ScecondDesc[2].fSizeX = 110.f;
    m_ScecondDesc[2].fSizeY = 35.f;
    m_ScecondDesc[2].Update = true;
    m_ScecondDesc[2].fSpeedPerSec = 0.f;

    /*HeavyCrossbow*/
    m_ScecondDesc[3].UID = UIID_PlayerWeaPon;
    m_ScecondDesc[3].fX = m_fXPos + 40.f;
    m_ScecondDesc[3].fY = 580.f;
    m_ScecondDesc[3].fZ = 0.4f;
    m_ScecondDesc[3].fSizeX = 120.f;
    m_ScecondDesc[3].fSizeY = 45.f;
    m_ScecondDesc[3].Update = true;
    m_ScecondDesc[3].fSpeedPerSec = 0.f;

    /*Fream*/
    m_ScecondDesc[4].UID = UIID_PlayerWeaPon;
    m_ScecondDesc[4].fX = m_fXPos + 40.f;
    m_ScecondDesc[4].fY = 580.f;
    m_ScecondDesc[4].fZ = 0.5f;
    m_ScecondDesc[4].fSizeX = 120.f;
    m_ScecondDesc[4].fSizeY = 45.f;
    m_ScecondDesc[4].Update = true;
    m_ScecondDesc[4].fSpeedPerSec = 0.f;

    for (_int i = 0; i < 5; i++)
    {
        m_fPrXPos2[i] = m_ScecondDesc[i].fX;
        m_fPrYPos2[i] = m_ScecondDesc[i].fY;
    }

    return S_OK;
}

HRESULT CWeaponUI::Add_Components()
{
    /* For.Com_Texture */

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_WeaPonTEX"),
                                      TEXT("Com_Texture_WeaPonTEX"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
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

CWeaponUI* CWeaponUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeaponUI* pInstance = new CWeaponUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWeaponUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeaponUI::Clone(void* pArg)
{
    CWeaponUI* pInstance = new CWeaponUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CWeaponUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeaponUI::Free()
{

    __super::Free();

    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pTextureCom);

    Safe_Release(m_pShaderCom);
}
