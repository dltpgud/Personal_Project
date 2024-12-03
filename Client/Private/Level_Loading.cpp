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

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel{pDevice, pContext}
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID, LOADINGID eLodingType)
{
    m_eNextLevelID = eNextLevelID;

    m_eLodingType = eLodingType;
    /* 로딩화면을 보여주기위한 객체들을 생성한다. */

    /* 로딩 작업을 직접 수행할 하청업체를 선정한다. */
    m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    if (m_eLodingType == LOADINGID_END)
        return S_OK;

     if (FAILED(Ready_Clone_Layer(CGameObject::UI)))
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
    m_fTimeSum += fTimeDelta;
    m_fFinishSum += fTimeDelta;

    if (m_fTimeSum >= 1.75f  )
        m_fTimeSum = 0;
    /* 로더가 다음레벨에 대한 자원 생성을 끝냈다라면 */
    if (true == m_pLoader->Finished())
    {
       
        HRESULT hr = {};

        /* 다음레벨 아이디에 맞는 실제 레벨을 할당해준다. */

        switch (m_eNextLevelID)
        {
        case LEVEL_MENU:
            hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLEVEL_MENU::Create(m_pDevice, m_pContext));
            break;
        case LEVEL_STAGE1:
            if (m_pGameInstance->Get_DIAnyKey())
            {
                m_pGameInstance->StopSound(CSound::SOUND_MENU);
                m_pGameInstance->StopSound(CSound::SOUND_LODING);
                hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Stage1::Create(m_pDevice, m_pContext));
            }
            break;
        case LEVEL_STAGE2:
            m_pGameInstance->StopSound(CSound::SOUND_LEVEL);
                hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Stage2::Create(m_pDevice, m_pContext));
            break;
        case LEVEL_BOSS:
            m_pGameInstance->StopSound(CSound::SOUND_LEVEL);
                hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_StageBoss::Create(m_pDevice, m_pContext));
            break;
        case LEVEL_RETURN_MENU:
            m_pGameInstance->StopSound(CSound::SOUND_LEVEL);
                hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLEVEL_MENU::Create(m_pDevice, m_pContext));
            break;
        }

        if (FAILED(hr))
            return;
    } 

}

HRESULT CLevel_Loading::Render()
{
#ifdef _DEBUG
    m_pLoader->Output_LoadingState();
#endif

    if (m_eLodingType == LOADINGID_END)
        return S_OK;

    if (m_eLodingType == LOADINGID::GORGE) {
        if (m_fFinishSum < 3.f) {
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
            else if (m_fTimeSum <= 1.25f)
            {
                m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("불러 오는 중 ...."), _float2(955.f, 635.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.8f);
            }
            else if (m_fTimeSum <= 1.5f)
            {
                m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("불러 오는 중 ....."), _float2(955.f, 635.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.8f);
            }
            else if (m_fTimeSum <= 1.75f)
            {
                m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("불러 오는 중 ......"), _float2(955.f, 635.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.8f);
            }
        }

        if (true == m_pLoader->Finished() && m_fFinishSum > 3.f) {

            // if (m_fTimeSum <= 0.25f)
             //{
            m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("계속 하려면 아무키나 누르세요 "), _float2(760.f, 635.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.7f);
            //   }

        }

        m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("협곡"), _float2(40.f, 50.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 1.f);
    }

    return S_OK;
}

HRESULT CLevel_Loading::Initialize_GORGE(const _uint& pLayerTag)
{
    CLoading::CLoading_DESC Load{};
    Load.LoadingID = m_eLodingType;
    Load.UID = CUI::UIID_Loading;
    Load.fX = 3500.f;
    Load.fY = 120.f;
    Load.fZ = 0.6f;
    Load.fSizeX = 8000.f;
    Load.fSizeY = 1340.f;
    Load.PrUpdate = true;
    Load.Update = true;
    Load.LateUpdate = true;
    Load.fSpeedPerSec = 100.f;
    Load.TexIndex = 5;
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_Loading",
        nullptr, 0, &Load)))
        return E_FAIL;

    CLoading::CLoading_DESC Load1{};
    Load1.UID = CUI::UIID_Loading;
    Load1.fX = 1900.f;
    Load1.fY = 360.f;
    Load1.fZ = 0.5f;
    Load1.fSizeX = 10480.f;;
    Load1.fSizeY = 720.f;
    Load1.PrUpdate = true;
    Load1.Update = true;
    Load1.LateUpdate = true;
    Load1.fSpeedPerSec = 250.f;
    Load1.TexIndex = 4;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_Loading",
        nullptr, 0, &Load1)))
        return E_FAIL;

    CLoading::CLoading_DESC Load2{};
    /*배경 2 중간에 나오는 배경*/
    Load2.UID = CUI::UIID_Loading;
    Load2.fX = 2150.f;
    Load2.fY = 360.f;
    Load2.fZ = 0.4f;
    Load2.fSizeX = 2280.f;;
    Load2.fSizeY = 720.f;
    Load2.PrUpdate = true;
    Load2.Update = true;
    Load2.LateUpdate = true;
    Load2.fSpeedPerSec = 300.f;
    Load2.TexIndex = 3;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_Loading",
        nullptr, 0, &Load2)))
        return E_FAIL;

    CLoading::CLoading_DESC Load3{};
    /*바닥*/
    Load3.UID = CUI::UIID_Loading;
    Load3.fX = 10040.f;
    Load3.fY = 720.f;
    Load3.fZ = 0.3f;
    Load3.fSizeX = 20040.f;
    Load3.fSizeY = 200.f;
    Load3.PrUpdate = true;
    Load3.Update = true;
    Load3.LateUpdate = true;
    Load3.fSpeedPerSec = 350.f;
    Load3.TexIndex = 2;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_Loading",
        nullptr, 0, &Load3)))
        return E_FAIL;

    /*bus*/
    CLoading::CLoading_DESC Load4{};
    Load4.UID = CUI::UIID_Loading;
    Load4.fX = 231.f;
    Load4.fY = 509.f;
    Load4.fZ = 0.2f;
    Load4.fSizeX = 550.f;
    Load4.fSizeY = 280.f;
    Load4.PrUpdate = true;
    Load4.Update = true;
    Load4.LateUpdate = true;
    Load4.fSpeedPerSec = 0.f;
    Load4.TexIndex = 1;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_Loading",
        nullptr, 0, &Load4)))
        return E_FAIL;


    /* 버스 연기 뒤*/
    CSpriteTexture::CSpriteTexture_DESC Smoke1{};
    Smoke1.UID = CUI::UIID_Loading;
    Smoke1.fX = 101.f;
    Smoke1.fY = 619.f;
    Smoke1.fZ = 0.19f;
    Smoke1.fSizeX = 100.f;
    Smoke1.fSizeY = 100.f;
    Smoke1.PrUpdate = true;
    Smoke1.Update = true;
    Smoke1.LateUpdate = true;
    Smoke1.fSpeedPerSec = 0.f;
    Smoke1.FilePath = L"Prototype_Component_BusSmokeBack";
    Smoke1.TexIndex = 20;
    Smoke1.interver = 0.2f;
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_SpriteTexture",
        nullptr, 0, &Smoke1)))
        return E_FAIL;

    /* 버스 연기 앞*/
    CSpriteTexture::CSpriteTexture_DESC Smoke2{};
    Smoke2.UID = CUI::UIID_Loading;
    Smoke2.fX = 271.f;
    Smoke2.fY = 615.f;
    Smoke2.fZ = 0.19f;
    Smoke2.fSizeX = 100.f;
    Smoke2.fSizeY = 90.f;
    Smoke2.PrUpdate = true;
    Smoke2.Update = true;
    Smoke2.LateUpdate = true;
    Smoke2.fSpeedPerSec = 0.f;
    Smoke2.FilePath = L"Prototype_Component_BusSmokefront";
    Smoke2.TexIndex = 20;
    Smoke2.interver = 0.2f;
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_SpriteTexture",
        nullptr, 0, &Smoke2)))
        return E_FAIL;

    /*배경 1 선인장*/
    CLoading::CLoading_DESC Load5{};
    Load5.UID = CUI::UIID_Loading;
    Load5.fX = 4080.f;
    Load5.fY = 360.f;
    Load5.fZ = 0.1f;
    Load5.fSizeX = 4080.f;
    Load5.fSizeY = 720.f;
    Load5.PrUpdate = true;
    Load5.Update = true;
    Load5.LateUpdate = true;
    Load5.fSpeedPerSec = 1000.f;
    Load5.TexIndex = 0;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_Loading",
        nullptr, 0, &Load5)))
        return E_FAIL;


    CSpriteTexture::CSpriteTexture_DESC LoadingBar{};
    LoadingBar.UID = CUI::UIID_Loading;
    LoadingBar.fX = 640.f;
    LoadingBar.fY = 685.f;
    LoadingBar.fZ = 0.09f;
    LoadingBar.fSizeX = 1280.f;
    LoadingBar.fSizeY = 90.f;
    LoadingBar.PrUpdate = true;
    LoadingBar.Update = true;
    LoadingBar.LateUpdate = true;
    LoadingBar.fSpeedPerSec = 0.f;
    LoadingBar.FilePath = L"Prototype_Component_Loading";
    LoadingBar.TexIndex = 1;
    LoadingBar.interver = 0.f;
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_SpriteTexture",
        nullptr, 0, &LoadingBar)))
        return E_FAIL;

    m_pGameInstance->Play_Sound( L"ST_Music_Credits.wav", CSound::SOUND_LODING, 0.5f);
    return S_OK;
}

HRESULT CLevel_Loading::Initialize_STAGE(const _uint& pLayerTag)
{
    CSpriteTexture::CSpriteTexture_DESC Loading{};
    Loading.UID = CUI::UIID_Loading;
    Loading.fX = g_iWinSizeX*0.5f;
    Loading.fY = g_iWinSizeY * 0.5f;
    Loading.fZ = 0.0f;
    Loading.fSizeX = g_iWinSizeX ;
    Loading.fSizeY = g_iWinSizeY;
    Loading.PrUpdate = true;
    Loading.Update = true;
    Loading.LateUpdate = true;
    Loading.fSpeedPerSec = 0.f;
    Loading.FilePath = L"Prototype_Component_Texture_Pade";
    Loading.TexIndex = 1;
    Loading.interver = 0.f;
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_SpriteTexture",
        nullptr, 0, &Loading)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Loading::Ready_Clone_Layer(const _uint& pLayerTag)
{

    if (m_eLodingType == GORGE)
    {
        if (FAILED(Initialize_GORGE(pLayerTag)))
            return E_FAIL;
    }

    if (m_eLodingType == STAGE)
    {
        if (FAILED(Initialize_STAGE(pLayerTag)))
            return E_FAIL;
    }



    if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_Cursor, false)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerHP, false)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerWeaPon, false)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerWeaPon_Aim, false)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, false)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerState, false)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_InteractiveUI, false)))
        return E_FAIL;
    return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID,
                                       LOADINGID eLodingType)
{
    CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

    if (FAILED(pInstance->Initialize(eNextLevelID, eLodingType)))
    {
        MSG_BOX("Failed to Created : CLevel_Loading");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Loading::Free()
{
    __super::Free();

    Safe_Release(m_pLoader);
}
