#include "stdafx.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Menu.h"
#include "Loading.h"
#include "Terrain.h"
#include "PlayerUI.h"
#include "WeaponUI.h"
#include "Camera_Free.h"
#include "Cursor.h"
#include "NonAniObj.h"
#include "Model.h"
#include "DOOR.h"
#include "CHEST.h"
#include "Player.h"
#include "Body_Player.h"
#include "Weapon.h"
#include "Wall.h"
#include "WeaPonIcon.h"
#include "Navigation.h"
#include "GunPawn.h"
#include "Body_GunPawn.h"
#include "Sky.h"
#include "SpriteTexture.h"
#include "Aim.h"
#include "JetFly.h"
#include "Body_JetFly.h"
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
	case LEVEL_STAGE2:
		hr = Loading_For_Stage2Level();
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

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpriteTexture"),
		CSpriteTexture::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerHP_UI"),
		CPlayerUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WeaponUI_UI"),
		CWeaponUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cursor"),
		CCursor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Player_Aim */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Aim"),
		CAim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");
	/* For.Prototype_Component_Texture_Menu */
 	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_MENU, TEXT("Prototype_Component_Texture_Menu"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/T_MainMenu_Background%d.png"),3))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델 로딩중입니다.");

	_matrix PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Bus Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Bus.dat"), PreTransformMatrix))))
		return E_FAIL;




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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.dds")))))
		return E_FAIL;
	
		m_strLoadingText = TEXT("네비게이션 로딩중입니다.");
		/* For.Prototype_Component_Navigation*/
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Prototype_Component_Navigation"),
			CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Data/Navigation/Navigation_Stage1.dat"))))
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


		/*Prototype_Component_ChestWeapon*/
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) ;
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component ChestWeapon Model_aniObj"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/ChestWeapon.dat"), PreTransformMatrix))))
			return E_FAIL;  
		PreTransformMatrix = XMMatrixIdentity(); // XMMatrixScaling(1.f, 1.45f, 1.45f);
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component ChestWeapon Model_Nonani"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/NonAnichest.dat"), PreTransformMatrix))))
			return E_FAIL;



		/*Prototype_Component_WeaponIcon*/
		PreTransformMatrix = XMMatrixRotationY((XMConvertToRadians(45.f))) * XMMatrixRotationZ((XMConvertToRadians(25.f)));
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_AssaultRifle_NonAni"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/AssaultRifle_NonAni.dat"), PreTransformMatrix))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_MissileGatling_NonAni"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/MissileGatling_NonAni.dat"), PreTransformMatrix))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_HeavyCrossbow_NonAni"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/HeavyCrossbow_NonAni.dat"), PreTransformMatrix))))
			return E_FAIL;


		///*Prototype_Component_Door*/
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component StageDoor Model_aniObj"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/StageDoor.dat"), PreTransformMatrix))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component BossDoor Model_aniObj"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/BossDoor.dat"), PreTransformMatrix))))
			return E_FAIL;



		//*Prototype_Component_Monster*//
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)*XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto_Component_GunPawn_Model"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/GunPawn.dat"), PreTransformMatrix))))
			return E_FAIL;
			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto_Component_JetFly_Model"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/JetFly.dat"), PreTransformMatrix))))
				return E_FAIL;

	m_strLoadingText = TEXT("셰이더 로딩중입니다.");




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

	/*Prototype_GameObject_Item*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_WeaPonIcon"),
		CWeaPonIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*Prototype_GameObject_AniObj*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_DOOR"),
		CDOOR::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_CHEST"),
	 	CCHEST::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*Prototype_GameObject_Wall*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_WALL"),
		CWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Prototype_GameObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Monster_GunPawn */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunPawn"),
		CGunPawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Prototype_GameObject_Monster_Body_GunPawn */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_GunPawn"),
		CBody_GunPawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_JetFly"),
		CJetFly::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Prototype_GameObject_Monster_Body_GunPawn */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_JetFly"),
		CBody_JetFly::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	m_strLoadingText = TEXT("사운드 로딩중입니다.");

	m_strLoadingText = TEXT("로딩 완료되었습니다.");

	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Stage2Level()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");
	m_strLoadingText = TEXT("네비게이션 로딩중입니다.");
	/* For.Prototype_Component_Navigation*/
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Prototype_Component_Navigation2"),
		CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Data/Navigation/Navigation_Stage2.dat"))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델 로딩중입니다.");

	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	m_strLoadingText = TEXT("객체원형 로딩중입니다.");

	m_strLoadingText = TEXT("사운드 로딩중입니다.");

	m_strLoadingText = TEXT("로딩 완료되었습니다.");

	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_BossLevel()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");
	m_strLoadingText = TEXT("네비게이션 로딩중입니다.");
	/* For.Prototype_Component_Navigation*/
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Prototype_Component_Navigation3"),
		CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Data/Navigation/Navigation_Boss.dat"))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델 로딩중입니다.");

	m_strLoadingText = TEXT("셰이더 로딩중입니다.");

	m_strLoadingText = TEXT("객체원형 로딩중입니다.");

	m_strLoadingText = TEXT("사운드 로딩중입니다.");

	m_strLoadingText = TEXT("로딩 완료되었습니다.");

	m_bFinished = true;
	return E_NOTIMPL;
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
