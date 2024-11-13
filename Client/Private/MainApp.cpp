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
	/*Font*/
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Robo"), TEXT("../Bin/Resources/Fonts/Cart.spritefont"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_Component_Shader*/
	/* 내 정점의 구성 정보를 상세히 묘사한다. */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_Inverse"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_Inverse.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
			return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
			return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
			return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Particle_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxParticleRect.hlsl"), VTXPARTICLE_RECT::Elements, VTXPARTICLE_RECT::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Shader_Particle_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxParticlePoint.hlsl"), VTXPARTICLE_POINT::Elements, VTXPARTICLE_POINT::iNumElements))))
		return E_FAIL;



	/* For.Prototype_Component_Navigation*/
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext))))
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
	/* For.Prototype_Component_Player_WeaPonTEX */

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_WeaPonTEX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerUI/WeaPon%d.dds"),6))))
		return E_FAIL;


	/* For.Prototype_Component_Player_Aim */

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_Aim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerUI/Aim0.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky.dds")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_BusSmokeBack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bus/Back%d.png"), 20))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_BusSmokefront"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bus/Front%d.png"), 20))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Loading.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_KeyBoard_F"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/InteractiveUI/T_Keyboard_Black_F.dds")))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterHP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MonsterHP/MonsterHP%d.dds"),2))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Intro"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Intro/T_Intro_Billy%d.Png"),3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_BossHP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MonsterHP/T_BossHealthBar.png")))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Mask/T_Noise_Liquid.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_ OBB */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;


	///*Prototype_Component_Wall*/
	_matrix PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component RockWallLarge01 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component RockWallLarge02 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge02.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component RockWallLarge03 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge03.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component RockWallsmall Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallsmall.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component RockBase Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockBase.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component RockWallLarge01 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component RockWallLarge02 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge02.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component RockWallLarge03 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge03.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component RockWallsmall Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallsmall.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component RockBase Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockBase.dat"), PreTransformMatrix))))
		return E_FAIL;

			
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component RockWallLarge02 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge02.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component RockWallLarge03 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallLarge03.dat"), PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Desertic Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Desertic.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock3 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock3.dat"), PreTransformMatrix))))
		return E_FAIL;
	;	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock4 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock4.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock5 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock5.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Desertic Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Desertic.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock3 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock3.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock4 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock4.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock5 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock5.dat"), PreTransformMatrix))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock6 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock6.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock7 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock7.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock8 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RocK8.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Arch3 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Arch3.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Arch2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Arch2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock small wall1 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock_small_wall1.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component RockWallsmall Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RockWallsmall.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component	(LEVEL_BOSS, TEXT("Proto Component Rock small wall1 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock_small_wall1.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Rock8 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RocK8.dat"), PreTransformMatrix))))
		return E_FAIL;					

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Pylon Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Pylon.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Wall Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Wall2 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Wall3 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall3.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component DoorRock Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/DoorRock.dat"), PreTransformMatrix))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Wall Model_Wall"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall.dat"), PreTransformMatrix))))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Arch3 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Arch3.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Arch2 Model_nonaniObj"),
	 	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Arch2.dat"), PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock6 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock6.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock7 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock7.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock8 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/RocK8.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Arch3 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Arch3.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Arch2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Arch2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock small wall1 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Rock_small_wall1.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Pylon Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Pylon.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Wall Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Wall2 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Wall3 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall3.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component DoorRock Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/DoorRock.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Wall Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall.dat"), PreTransformMatrix))))
		return E_FAIL;		
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Wall2 Model_Wall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Wall2.dat"), PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component BossFloor Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/BossFloor.dat"), PreTransformMatrix))))
		return E_FAIL;

	///*Prototype_Component_Ve*/
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Cactus Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Cactus.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Cattail2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Cattail2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Vine Falling1 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Vine_Falling1.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Vine Falling2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Vine_Falling2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component vine1 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/vine1.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component vine2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Vine2.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Platkit Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Platkit.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Platkit2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Platkit2.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Cactus Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Cactus.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Cattail2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Cattail2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Vine Falling1 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Vine_Falling1.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Vine Falling2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Vine_Falling2.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component vine1 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/vine1.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component vine2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Vine2.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Platkit Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Platkit.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Platkit2 Model_nonaniObj"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Data/NonAni/Platkit2.dat"), PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY((XMConvertToRadians(180.f)));
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/Player.dat"), PreTransformMatrix))))
		return E_FAIL;

	/*Prototype_Component_Model_Weapon */
	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HendGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/HendGun.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_AssaultRifle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/AssaultRifle.dat"), PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MissileGatling"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/MissileGatling.dat"), PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HeavyCrossbow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Data/Ani/HeavyCrossbow.dat"), PreTransformMatrix))))
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

