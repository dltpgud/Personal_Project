#include "stdafx.h"
#include "Level_Loading.h"
#include "Loader.h"
#include "Loading.h"
#include "LEVEL_MENU.h"
#include "Level_Stage1.h"
#include "GameInstance.h"

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
    if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerHP, false)))
        return S_OK;
    if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerWeaPon, false)))
        return S_OK;
    if (m_eLodingType == LOADINGID_END)
        return S_OK;

    if (FAILED(Ready_Clone_Layer(CGameObject::UI)))
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{

    __super::Update(fTimeDelta);

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
            if (GetKeyState(VK_SPACE) & 0x8000)
                hr = m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Stage1::Create(m_pDevice, m_pContext));
            break;
        }

        if (FAILED(hr))
            return;
    }
}

HRESULT CLevel_Loading::Render()
{

    __super::Render();

#ifdef _DEBUG
    m_pLoader->Output_LoadingState();
#endif

    return S_OK;
}

HRESULT CLevel_Loading::Ready_Clone_Layer(const _uint& pLayerTag)
{

    CLoading::CLoading_DESC Load{};
    Load.LoadingID = m_eLodingType;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, pLayerTag, L"Prototype_GameObject_Loading",
                                                        nullptr, 0, &Load)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_Cursor, false)))
        return S_OK;

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
