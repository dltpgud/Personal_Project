#include "stdafx.h"
#include "Level_Loading.h"
#include "Loader.h"
#include "Loading.h"
#include "LEVEL_MENU.h"
#include "Level_Stage1.h"
#include "Level_Stage2.h"
#include "Level_StageBoss.h"
#include "GameInstance.h"
#include "SpriteTexture.h"
#include "Fade.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel{pDevice, pContext}
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID, _bool Page )
{
    m_eNextLevelID = eNextLevelID;

    m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    if (FAILED(Ready_UI()))
        return E_FAIL;

    if (true== Page)
    {
      if (FAILED(Ready_Clone_Layer()))
         return E_FAIL;
    }

    return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
    m_fTimeSum += fTimeDelta;
    if (m_fTimeSum >= 1.f  )
        m_fTimeSum = 0;

    if (m_pGameInstance->AllJobCompleted() && true == m_pLoader->Finished())
    {
        switch (m_eNextLevelID)
        {
        case LEVEL_MENU:
            m_pGameInstance->Open_Level(m_eNextLevelID, CLEVEL_MENU::Create(m_pDevice, m_pContext));
            break;
        case LEVEL_STAGE1:
            if (m_pGameInstance->Get_DIAnyKey())
            {
                if (m_pFade)
                    m_pFade->Set_Fade(false);
                m_pGameInstance->StopSound(&m_pChannel);
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Stage1::Create(m_pDevice, m_pContext));
            }
            break;
        case LEVEL_STAGE2:
            if (m_pFade)
            m_pFade->Set_Fade(false); 
            m_pGameInstance->StopSound(&m_pChannel);
            m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Stage2::Create(m_pDevice, m_pContext));
            break;
        case LEVEL_BOSS:
            m_pGameInstance->StopSound(&m_pChannel);
            m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_StageBoss::Create(m_pDevice, m_pContext));
            break;
        }
    } 
}

HRESULT CLevel_Loading::Render()
{
#ifdef _DEBUG
    m_pLoader->Output_LoadingState();
#endif

    if (LEVEL_MENU == m_eNextLevelID || LEVEL_STAGE2 == m_eNextLevelID || LEVEL_BOSS == m_eNextLevelID)
        return S_OK;

    if (false == m_pLoader->Finished())
    {
      if (m_fTimeSum <= 0.25f)
      {
        m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("불러 오는 중 "), _float2(955.f, 635.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.8f);
      }
      else if (m_fTimeSum <= 0.5f)
      {
        m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("불러 오는 중 ."), _float2(955.f, 635.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.8f);
      }
      else if (m_fTimeSum <= 0.75f)
      {
        m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("불러 오는 중 .."), _float2(955.f, 635.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.8f);
      }
      else if (m_fTimeSum <= 1.f)
      {
        m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("불러 오는 중 ..."), _float2(955.f, 635.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.8f);
      }
    }       

    if (true == m_pLoader->Finished())
    {
      m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("계속 하려면 아무키나 누르세요 "), _float2(760.f, 635.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.7f);
    }

    m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("협곡"), _float2(40.f, 50.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 1.f);

    return S_OK;
}

HRESULT CLevel_Loading::Ready_UI()
{
    m_pGameInstance->Play_Sound(L"ST_Music_Credits.wav", &m_pChannel, 0.5f);

    if (m_pGameInstance->Find_Clone_UIObj(L"Fade") != nullptr)
    {
        m_pFade = static_cast<CFade*>(m_pGameInstance->Find_Clone_UIObj(L"Fade"));
        Safe_AddRef(m_pFade);
    }

    m_pGameInstance->Set_OpenUI(false, TEXT("Cursor"));
    m_pGameInstance->Set_OpenUI(false, TEXT("Menu"));
    m_pGameInstance->Set_OpenUI(false, TEXT("PlayerHP"));
    m_pGameInstance->Set_OpenUI(false, TEXT("WeaPon_Aim"));
    m_pGameInstance->Set_OpenUI(false, TEXT("WeaponUI"));

    return S_OK;
}

HRESULT CLevel_Loading::Ready_Clone_Layer()
{
    CLoading::CLoading_DESC Load{};
    Load.UID = UIID_Loading;
    Load.fX = 3500.f;
    Load.fY = 120.f;
    Load.fZ = 0.6f;
    Load.fSizeX = 8000.f;
    Load.fSizeY = 1340.f;
    Load.Update = true;
    Load.fSpeedPerSec = 100.f;
    Load.TexIndex = 5;
    Load.iDeleteLevel = LEVEL_LOADING;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"bg1", L"Prototype_GameObject_Loading", &Load)))
        return E_FAIL;
  
    CLoading::CLoading_DESC Load1{};
    Load1.UID = UIID_Loading;
    Load1.fX = 1900.f;
    Load1.fY = 360.f;
    Load1.fZ = 0.5f;
    Load1.fSizeX = 10480.f;;
    Load1.fSizeY = 720.f;
    Load1.Update = true;
    Load1.fSpeedPerSec = 250.f;
    Load1.TexIndex = 4;
    Load1.iDeleteLevel = LEVEL_LOADING;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"bg2",L"Prototype_GameObject_Loading",&Load1)))
        return E_FAIL;
  
    CLoading::CLoading_DESC Load2{};
    Load2.UID = UIID_Loading;
    Load2.fX = 2150.f;
    Load2.fY = 360.f;
    Load2.fZ = 0.4f;
    Load2.fSizeX = 2280.f;;
    Load2.fSizeY = 720.f;
    Load2.Update = true;
    Load2.fSpeedPerSec = 300.f;
    Load2.TexIndex = 3;
    Load2.iDeleteLevel = LEVEL_LOADING;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"bg3",L"Prototype_GameObject_Loading",&Load2)))
        return E_FAIL;
  
    CLoading::CLoading_DESC Load3{};
    /*바닥*/
    Load3.UID = UIID_Loading;
    Load3.fX = 10040.f;
    Load3.fY = 720.f;
    Load3.fZ = 0.3f;
    Load3.fSizeX = 20040.f;
    Load3.fSizeY = 200.f;
    Load3.Update = true;
    Load3.fSpeedPerSec = 350.f;
    Load3.TexIndex = 2;
    Load3.iDeleteLevel = LEVEL_LOADING;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"Floor",L"Prototype_GameObject_Loading",&Load3)))
        return E_FAIL;
  
    /*bus*/
    CLoading::CLoading_DESC Load4{};
    Load4.UID = UIID_Loading;
    Load4.fX = 231.f;
    Load4.fY = 509.f;
    Load4.fZ = 0.2f;
    Load4.fSizeX = 550.f;
    Load4.fSizeY = 280.f;
    Load4.Update = true;
    Load4.fSpeedPerSec = 0.f;
    Load4.TexIndex = 1;
    Load4.iDeleteLevel = LEVEL_LOADING;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"bus",L"Prototype_GameObject_Loading",&Load4)))
        return E_FAIL;

    /* 버스 연기 뒤*/
    CSpriteTexture::CSpriteTexture_DESC Smoke1{};
    Smoke1.UID = UIID_Loading;
    Smoke1.fX = 101.f;
    Smoke1.fY = 619.f;
    Smoke1.fZ = 0.19f;
    Smoke1.fSizeX = 100.f;
    Smoke1.fSizeY = 100.f;
    Smoke1.Update = true;
    Smoke1.fSpeedPerSec = 0.f;
    Smoke1.ProtoName = L"Prototype_Component_BusSmokeBack";
    Smoke1.TexIndex = 20;
    Smoke1.interver = 0.2f;
    Smoke1.iDeleteLevel = LEVEL_LOADING;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"Smoke1",L"Prototype_GameObject_SpriteTexture",&Smoke1)))
        return E_FAIL;
  
    /* 버스 연기 앞*/
    CSpriteTexture::CSpriteTexture_DESC Smoke2{};
    Smoke2.UID = UIID_Loading;
    Smoke2.fX = 271.f;
    Smoke2.fY = 615.f;
    Smoke2.fZ = 0.19f;
    Smoke2.fSizeX = 100.f;
    Smoke2.fSizeY = 90.f;
    Smoke2.Update = true;
    Smoke2.fSpeedPerSec = 0.f;
    Smoke2.ProtoName = L"Prototype_Component_BusSmokefront";
    Smoke2.TexIndex = 20;
    Smoke2.interver = 0.2f;
    Smoke2.iDeleteLevel = LEVEL_LOADING;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"Smoke2",L"Prototype_GameObject_SpriteTexture",&Smoke2)))
        return E_FAIL;

    /*배경 1 선인장*/
    CLoading::CLoading_DESC Load5{};
    Load5.UID = UIID_Loading;
    Load5.fX = 4080.f;
    Load5.fY = 360.f;
    Load5.fZ = 0.1f;
    Load5.fSizeX = 4080.f;
    Load5.fSizeY = 720.f;
    Load5.Update = true;
    Load5.fSpeedPerSec = 1000.f;
    Load5.TexIndex = 0;
    Load5.iDeleteLevel = LEVEL_LOADING;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"cactus",L"Prototype_GameObject_Loading",&Load5)))
        return E_FAIL;

    CSpriteTexture::CSpriteTexture_DESC LoadingBar{};
    LoadingBar.UID = UIID_Loading;
    LoadingBar.fX = 640.f;
    LoadingBar.fY = 685.f;
    LoadingBar.fZ = 0.08f;
    LoadingBar.fSizeX = 1280.f;
    LoadingBar.fSizeY = 90.f;
    LoadingBar.Update = true;
    LoadingBar.fSpeedPerSec = 0.f;
    LoadingBar.ProtoName = L"Prototype_Component_Loading";
    LoadingBar.TexIndex = 1;
    LoadingBar.interver = 0.f;
    LoadingBar.iDeleteLevel = LEVEL_LOADING;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"LoadingBar",L"Prototype_GameObject_SpriteTexture",&LoadingBar)))
        return E_FAIL;

    return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID, _bool Page)
{
    CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

    if (FAILED(pInstance->Initialize(eNextLevelID,Page)))
    {
        MSG_BOX("Failed to Created : CLevel_Loading");
        Safe_Release(pInstance);
    }

    return pInstance;
    }

void CLevel_Loading::Free()
{
    __super::Free();
    m_pGameInstance->StopSound(&m_pChannel);
    Safe_Release(m_pLoader);
    Safe_Release(m_pFade);
}
