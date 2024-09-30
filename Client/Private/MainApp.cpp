#include "stdafx.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	/* 내 게임에 필요한 필수 기능들에 대한 초기화과정을 수행한다. */
	
	ENGINE_DESC			EngineDesc{};
	EngineDesc.hInstance = g_hInst;
	EngineDesc.hWnd       = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iWinSizeX  = g_iWinSizeX;
	EngineDesc.iWinSizeY  = g_iWinSizeY;
	EngineDesc.iNumLevels = LEVEL_END;

	/*게임 인스턴스 생성*/
	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;


	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;



	/*최초 씬 설정*/
	if(FAILED(Open_Level(LEVEL_MENU)))
		return E_FAIL;
   
	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update(fTimeDelta);
#ifndef _DEBUG
	m_pGameInstance->Get_FPS(TEXT("Timer_60"), g_hWnd);
#endif
}

void CMainApp::Render()
{
	if (FAILED(m_pGameInstance->Render_Begin(_float4(0.f, 0.f, 1.f, 1.f))))
		 return;

	 m_pGameInstance->Draw(); 

	if (FAILED(m_pGameInstance->Render_End()))
		 return;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	/* 내 정점의 구성 정보를 상세히 묘사한다. */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;


		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
			return E_FAIL;
	/* For.Prototype_Component_Player_HpBar */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_HpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerUI/PlayerHPBar%d.png"),3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Cusor */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cusor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/T_MouseCursor.dds")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Canyon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Canyon%d.png"), 6))))
		return E_FAIL;
	

	// 로딩용 그 버스랑 배경 지나가는거

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

