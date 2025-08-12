#include "stdafx.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Fade.h"
#include "Menu.h"
#include "Loading.h"
#include "Cursor.h"
#include "PlayerUI.h"
#include"WeaponUI.h"
#include "Aim.h"
#include "InteractiveUI.h"
CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};
	EngineDesc.hInstance = g_hInst;
	EngineDesc.hWnd       = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iWinSizeX  = g_iWinSizeX; 
	EngineDesc.iWinSizeY  = g_iWinSizeY;
	EngineDesc.iNumLevels = LEVEL_END;

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if(FAILED(Open_Level(LEVEL_MENU)))
		return E_FAIL;
   
	   if (FAILED(m_pGameInstance->Add_Font(TEXT("Robo"), TEXT("../Bin/Resources/Fonts/Cart.spritefont"))))
            return E_FAIL;
	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update(fTimeDelta);

#ifdef _DEBUG
        m_fTimeAcc += fTimeDelta;
#endif
}

void CMainApp::Render()
{
	if (FAILED(m_pGameInstance->Render_Begin(_float4(0.f, 0.f, 0.f, 1.f))))
		 return;

    m_pGameInstance->Draw(); 

#ifdef _DEBUG
    ++m_iNumRender;

    if (m_fTimeAcc >= 1.f)
    {
        wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumRender);
        m_fTimeAcc = 0.f;
        m_iNumRender = 0;
    }
    m_pGameInstance->Render_Text(TEXT("Robo"), m_szFPS, _float2(1150.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f);

#endif

	if (FAILED(m_pGameInstance->Render_End()))
		 return;
}

void CMainApp::Delete()
{
    m_pGameInstance->Delete(); 
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}


CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}

