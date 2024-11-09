#include "stdafx.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Edit.h"
#include "Camera_Tool.h"
#include "Terrain.h"
#include "Model.h"
#include "fabric.h"
#include "Door.h"
#include "Chest.h"
#include "Wall.h"
#include "Monster.h"
#include "NPC.h"
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


	if (FAILED(Ready_Prototype_For_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_For_GameObject()))
		return E_FAIL;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);




	/*최초 씬 설정*/
	if(FAILED(Open_Level(LEVEL_EDIT)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	m_pGameInstance->Update(fTimeDelta);

}

void CMainApp::Render()
{
	if (FAILED(m_pGameInstance->Render_Begin(_float4(0.f, 0.f, 1.f, 1.f))))
		 return;

	 m_pGameInstance->Draw(); 

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (FAILED(m_pGameInstance->Render_End()))
		 return;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	m_pGameInstance->Open_Level(LEVEL_EDIT, CLevel_Edit::Create(m_pDevice, m_pContext));
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_For_Component()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	/* 내 정점의 구성 정보를 상세히 묘사한다. */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


	/*Prototype_Component_Texture_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Texture1_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Tile0.dds")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Fire1_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/T_Fire_Pattern_02.dds")))))
		return E_FAIL;

	///*Prototype_Component_Struck*/
	_matrix			PreTransformMatrix = XMMatrixIdentity();

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component struck Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/struck.dat"), PreTransformMatrix))))
	return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component struck2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/struck2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Desertic Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Desertic.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Rock Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Rock2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Rock3 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock3.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Rock4 Model_nonaniObj"),
	    CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock4.dat"), PreTransformMatrix))))
	    return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Rock5 Model_nonaniObj"),
	    CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock5.dat"), PreTransformMatrix))))
	    return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Rock6 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock6.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Rock7 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock7.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Rock8 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RocK8.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Arch3 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Arch3.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Arch2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Arch2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Rock small wall1 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock_small_wall1.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component BossFloor Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/BossFloor.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Test Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Test.dat"), PreTransformMatrix))))
		return E_FAIL;

if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Pylon Model_nonaniObj"),
	    CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Pylon.dat"), PreTransformMatrix))))
	    return E_FAIL;
if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Bus Model_nonaniObj"),
	    CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Bus.dat"), PreTransformMatrix))))
	    return E_FAIL;
if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Platkit Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Platkit.dat"), PreTransformMatrix))))
		return E_FAIL;
if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Platkit2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Platkit2.dat"), PreTransformMatrix))))
		return E_FAIL;


		///*Prototype_Component_Ve*/
if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Cactus Model_nonaniObj"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Cactus.dat"), PreTransformMatrix))))
	return E_FAIL;
if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Cattail2 Model_nonaniObj"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Cattail2.dat"), PreTransformMatrix))))
	return E_FAIL;
if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Vine Falling1 Model_nonaniObj"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Vine_Falling1.dat"), PreTransformMatrix))))
	return E_FAIL;

if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Vine Falling2 Model_nonaniObj"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Vine_Falling2.dat"), PreTransformMatrix))))
	return E_FAIL;
if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component vine1 Model_nonaniObj"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/vine1.dat"), PreTransformMatrix))))
	return E_FAIL;
if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component vine2 Model_nonaniObj"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Vine2.dat"), PreTransformMatrix))))
	return E_FAIL;


		///*Prototype_Component_Wall*/
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component RockWallLarge01 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component RockWallLarge02 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge02.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component RockWallLarge03 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge03.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component RockWallsmall Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallsmall.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component RockBase Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockBase.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Wall Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Wall2 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Wall3 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall3.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component DoorRock Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/DoorRock.dat"), PreTransformMatrix))))
		return E_FAIL;



	/* For.Prototype_Component_Navigation*/
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///*Prototype_Component_Door*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component StageDoor Model_aniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/StageDoor.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component BossDoor Model_aniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/BossDoor.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component ItemDoor Model_aniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/WPDoor.dat"), PreTransformMatrix))))
		return E_FAIL;
	///*Prototype_Component_Chest*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) *XMMatrixRotationY((XMConvertToRadians(180.f)));
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_aniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/ChestWeapon.dat"), PreTransformMatrix))))
		return E_FAIL;


	
	///*Prototype_Component_Monster*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component GunPawn_Monster"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/GunPawn.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component JetFly_Monster"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/JetFly.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component BoomBot_Monster"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/SKT_BoomBot.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component MecanoBot_Monster"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/SKT_MecanoBot.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component Boss_Monster"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/SKT_BillyBoom.dat"), PreTransformMatrix))))
		return E_FAIL;
	///*Prototype_Component_NPC*/
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component HealthBot_NPC"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/HealthBot.dat"), PreTransformMatrix))))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_For_GameObject()
{
	/* Prototype_GameObject_Camera_Tool */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("ProtoGameObject_CameraTool"),
		CCamera_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if(FAILED(m_pGameInstance->Add_Prototype(TEXT("Proto GameObject_Terrain"),CTerrain::Create(m_pDevice,m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_nonaniObj*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Proto GameObject fabric_nonaniObj"), Cfabric::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Proto GameObject Rock_Wall"),  CWALL::Create(m_pDevice, m_pContext))))
		return E_FAIL;

    /* For.Prototype_GameObject_aniObj*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Proto GameObject Door_aniObj"), CDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Proto GameObject Chest_aniObj"), CChest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*Proto GameObject Monster_Monster*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Proto GameObject Monster_Monster"),  CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/*Proto GameObject NPC_NPC*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Proto GameObject NPC_NPC"), CNPC::Create(m_pDevice, m_pContext))))
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

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}

