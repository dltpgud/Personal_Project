#include "stdafx.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Menu.h"
#include "Loading.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "PlayerUI.h"
#include "Cursor.h"
#include "NonAniObj.h"
#include "Model.h"
#include "DOOR.h"
#include "CHEST.h"
_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0); // 컴객체를 한 번 초기화 해준다.

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice}
	, m_pContext{pContext}
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	

	InitializeCriticalSection(&m_CriticalSection);

	/*서브 스레드 생성*/
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT hr = { 0 };

	switch (m_eNextLevelID)
	{
	case LEVEL_MENU:
		hr = Loading_For_MenuLevel();
		break;
	case LEVEL_STAGE1:
		hr = Loading_For_Stage1Level();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG

void CLoader::Output_LoadingState()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());
}

#endif

HRESULT CLoader::Loading_For_MenuLevel()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading"),
		CLoading::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerHP_UI"),
		CPlayerUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cursor"),
		CCursor::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");
	/* For.Prototype_Component_Texture_Menu */
 	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_MENU, TEXT("Prototype_Component_Texture_Menu"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/T_MainMenu_Background%d.png"),3))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델 로딩중입니다.");

	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	m_strLoadingText = TEXT("객체원형 로딩중입니다.");
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Menu"),
		CMenu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("사운드 로딩중입니다.");
	//m_pGameInstance->LoadSoundFile("ST.ogg");


	m_strLoadingText = TEXT("로딩 완료되었습니다.");

	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1Level()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Texture1_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.jpg")))))
		return E_FAIL;


	m_strLoadingText = TEXT("모델 로딩중입니다.");
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

		/*Prototype_Component_Struck*/
		_matrix			PreTransformMatrix = XMMatrixIdentity();
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component struck Model_nonaniObj"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/struck.dat"), PreTransformMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component struck2 Model_nonaniObj"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/struck2.dat"), PreTransformMatrix))))
			return E_FAIL;

		PreTransformMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationY((XMConvertToRadians(180.f)));
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component ChestWeapon Model_aniObj"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/ChestWeapon.dat"), PreTransformMatrix))))
			return E_FAIL;  


		///*Prototype_Component_Door*/
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component StageDoor Model_aniObj"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/StageDoor.dat"), PreTransformMatrix))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component BossDoor Model_aniObj"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/BossDoor.dat"), PreTransformMatrix))))
			return E_FAIL;

	m_strLoadingText = TEXT("셰이더 로딩중입니다.");
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
			return E_FAIL;
	m_strLoadingText = TEXT("객체원형 로딩중입니다.");
	/* Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Proto GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*Prototype_GameObject_Camera_Free*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*Prototype_GameObject_NonAniObj*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_NonAniObj"),
		CNonAni::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*Prototype_GameObject_AniObj*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_DOOR"),
		CDOOR::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_CHEST"),
	 	CCHEST::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("사운드 로딩중입니다.");

	m_strLoadingText = TEXT("로딩 완료되었습니다.");

	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_EDIT()
{
	m_bFinished = true;
	return S_OK;
}


CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
	 	MSG_BOX("Failed to Created : CLoader");
	 	Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
