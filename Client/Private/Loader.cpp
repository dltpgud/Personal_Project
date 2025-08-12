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
#include "Body_BoomBot.h"
#include "BoomBot.h"
#include "InteractiveUI.h"
#include "Body_HealthBot.h"
#include "HealthBot.h"
#include "MonsterHP.h"
#include "Bullet.h"
#include "Body_MecanoBot.h"
#include "MecanoBot.h"
#include "Body_BillyBoom.h"
#include "BillyBoom.h"
#include "SceneCamera.h"
#include "BossIntroBG.h"
#include "BossHP.h"
#include "ShootEffect.h"
#include "PlayerEffectUI.h"
#include "ShootingUI.h"
#include "PlayerBullet.h"
#include "BossBullet_Berrle.h"
#include "ShockWave.h"
#include "Shock.h"
#include "BossBullet_Laser.h"
#include "Particle_Explosion.h"
#include "Fade.h"
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

    if (LEVEL_MENU == m_eNextLevelID && m_pGameInstance->Get_Player() == nullptr)
    {
          Loading_For_ProtoObject();
          Loading_For_Sound(); 
          Loading_For_Static_ComPonent(); 
          Loading_For_Static_Texture();
    }

	InitializeCriticalSection(&m_CriticalSection);

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
	case LEVEL_BOSS:
		hr = Loading_For_BossLevel();
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

HRESULT CLoader::Loading_For_ProtoObject()
{
#pragma region Map

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Proto GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext)); });

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_NonAniObj"),CNonAni::Create(m_pDevice, m_pContext)); });

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Proto GameObject Door_aniObj"), CDOOR::Create(m_pDevice, m_pContext));});
       
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Proto GameObject Chest_aniObj"),CCHEST::Create(m_pDevice, m_pContext));});

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_WALL"),CWall::Create(m_pDevice, m_pContext));});
	
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),CSky::Create(m_pDevice, m_pContext));});

#pragma endregion

#pragma region Camera

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),CCamera_Free::Create(m_pDevice, m_pContext));});
	
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SceneCamera"),CSceneCamera::Create(m_pDevice, m_pContext));});
  
#pragma endregion
	
#pragma region BoomBot

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BoomBot"),CBoomBot::Create(m_pDevice, m_pContext));});
	
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_BoomBot"),CBody_BoomBot::Create(m_pDevice, m_pContext));});

#pragma endregion

#pragma region MecanoBot

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MecanoBot"),CMecanoBot::Create(m_pDevice, m_pContext));});
	
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_MecanoBot"),CBody_MecanoBot::Create(m_pDevice, m_pContext));});

#pragma endregion

#pragma region JetFly

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_JetFly"), CJetFly::Create(m_pDevice, m_pContext));});
	
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_JetFly"),CBody_JetFly::Create(m_pDevice, m_pContext));});
   
#pragma endregion

#pragma region GunPawn

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunPawn"),CGunPawn::Create(m_pDevice, m_pContext));});
	
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_GunPawn"),CBody_GunPawn::Create(m_pDevice, m_pContext));});
 
#pragma endregion

#pragma region HealthBot

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HealthBot"),CHealthBot::Create(m_pDevice, m_pContext));});
	
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_HealthBot"),CBody_HealthBot::Create(m_pDevice, m_pContext));});
  
#pragma endregion

#pragma region BillyBoom

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BillyBoom"),CBillyBoom::Create(m_pDevice, m_pContext));});
	
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_BillyBoom"),CBody_BillyBoom::Create(m_pDevice, m_pContext));});

#pragma endregion

#pragma region Player 

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),CPlayer::Create(m_pDevice, m_pContext));});
	
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),CBody_Player::Create(m_pDevice, m_pContext));});

#pragma endregion

#pragma region Weapon

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_WeaPonIcon"),CWeaPonIcon::Create(m_pDevice, m_pContext));});
	
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),CWeapon::Create(m_pDevice, m_pContext));});

#pragma endregion

#pragma region Skill

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShockWave"),CShockWave::Create(m_pDevice, m_pContext));});
 
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shock"),CShock::Create(m_pDevice, m_pContext));});
  
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_BossBullet_Berrle"),CBossBullet_Berrle::Create(m_pDevice, m_pContext));});

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_BossBullet_Laser"),CBossBullet_Laser::Create(m_pDevice, m_pContext));});

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_PlayerBullet"),CPlayerBullet::Create(m_pDevice, m_pContext));});

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_Bullet"),CBullet::Create(m_pDevice, m_pContext));});
  
#pragma endregion
  
#pragma region Effect

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype GameObject_ShootEffect"),CShootEffect::Create(m_pDevice, m_pContext));});
   
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Explosion"),CParticle_Explosion::Create(m_pDevice, m_pContext));});

#pragma endregion

#pragma region UI
   if (FAILED(m_pGameInstance->Add_UI_To_Proto(L"Prototype_Gameobject_Pade", CFade::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype_GameObject_Menu"),CMenu::Create(m_pDevice, m_pContext))))
       return E_FAIL;
	 
   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype_GameObject_Cursor"),CCursor::Create(m_pDevice, m_pContext))))
       return E_FAIL;
   				 
   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype_GameObject_PlayerHP_UI"),CPlayerUI::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype_GameObject_WeaponUI_UI"),CWeaponUI::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype_GameObject_Player_Aim"),CAim::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype_GameObject_InteractiveUI"),CInteractiveUI::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterHP"),CMonsterHP::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype_GameObject_Boss_InstroUI"),CBossIntroBG::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype GameObject_ShootingUI"),CShootingUI::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype GameObject_PlayerEffectUI"),CPlayerEffectUI::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype_GameObject_Loading"),CLoading::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype_GameObject_SpriteTexture"),CSpriteTexture::Create(m_pDevice, m_pContext))))
       return E_FAIL;

   if (FAILED(m_pGameInstance->Add_UI_To_Proto(TEXT("Prototype_GameObject_Boss_HP"),CBossHPUI::Create(m_pDevice, m_pContext))))
       return E_FAIL;
#pragma endregion 
  
    return S_OK;
}

HRESULT CLoader::Loading_For_Sound()
{
    /*LEVEL*/
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Ambient_Canyon.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Ambient_Special.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Ambient_EnergyCenter.ogg"); });
    /*Menu */
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("Menu.mp3"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Button_Hover_In.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Button_Click.ogg"); });
    /*LODING*/
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Music_Credits.wav"); });
    /*interect*/
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Door_Act1_Open.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Door_Act1_Close.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Door_Boss_Act1_Open.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Door_Special_Open.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Door_Special_Airlock_Open.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Healbot_Use_09_15_2023.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Chest_Open.ogg"); });
    /*Player*/
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Player_Stun_Loop.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Player_Footstep_Scuff_Sand.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Player_Footstep_Scuff_Sand_05.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Player_Switch_T01.ogg"); });

    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Player_Jump.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Player_DoubleJump.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Player_Burn_Loop.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Player_Burn_Trigger.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Player_Burn_Stop.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Player_Flash_Stop.ogg"); });
    /*WeaPon*/
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Handgun_Reload.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Handgun_PF_Shoot.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_AssaultRifle_Reload.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_AssaultRifle_PF_Shoot.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_HeavyCrossbow_Reload.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_HeavyCrossbow_SF_Shoot_A.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_MissileGatling_Reload.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_MissileGatling_PF_Shoot.ogg"); });
    /*Boss*/
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_BillyBoom_Outro.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_BillyBoom_Hand_Blast_Shoot.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_BillyBoom_Shockwave_Shoot.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Enemy_Laser_Loop.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Enemy_Laser_Loop_Start.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Enemy_Laser_Loop_Stop.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_BillyBoom_FootStep.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_BillyBoom_From_Two_Blaster_To_One.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_DiggyMole_Shockwave.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_BillyBoom_Cast_Hand_Blast.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_BillyBoom_Presence.ogg"); });
    /*Enemy*/
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Enemy_Step_Medium.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_MortarPod_Shoot.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Enemy_Rocket_Shoot.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_FlashFly_Shoot_A.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Enemy_Move_Roll.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Enemy_Move_Roll2.ogg"); });
    m_pGameInstance->Add_Job([this]() { return m_pGameInstance->LoadSoundFile("ST_Enemy_Move_Fly_Loop.ogg");});
    return S_OK;
}

HRESULT CLoader::Loading_For_Static_ComPonent()
{
#pragma region Shader

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
                          CShader::Create(m_pDevice, m_pContext,TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"),VTXPOSTEX::Elements, VTXPOSTEX::iNumElements));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_Inverse"),
                          CShader::Create(m_pDevice, m_pContext,TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_Inverse.hlsl"),VTXPOSTEX::Elements, VTXPOSTEX::iNumElements));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
                          CShader::Create(m_pDevice, m_pContext,TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"),VTXMESH::Elements, VTXMESH::iNumElements));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshIst"),
                          CShader::Create(m_pDevice, m_pContext,TEXT("../Bin/ShaderFiles/Shader_VtxMeshInstance.hlsl"),VTXMATRIX_INSTANCE::Elements, VTXMATRIX_INSTANCE::iNumElement));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
                          CShader::Create(m_pDevice, m_pContext,TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"),VTXNORTEX::Elements, VTXNORTEX::iNumElements));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
                          CShader::Create(m_pDevice, m_pContext,TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"),VTXANIMMESH::Elements, VTXANIMMESH::iNumElements));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
                          CShader::Create(m_pDevice, m_pContext,TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"),VTXCUBE::Elements, VTXCUBE::iNumElements));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Particle_Rect"),
                          CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxParticleRect.hlsl"), VTXPARTICLE_RECT::Elements, VTXPARTICLE_RECT::iNumElements));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Shader_Particle_Point"),
                          CShader::Create(m_pDevice, m_pContext,TEXT("../Bin/ShaderFiles/Shader_VtxParticlePoint.hlsl"),VTXPARTICLE_POINT::Elements, VTXPARTICLE_POINT::iNumElements));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Point"),
                          CShader::Create(m_pDevice, m_pContext,TEXT("../Bin/ShaderFiles/Shader_VtxPoint.hlsl"),VTXPOSTEX::Elements, VTXPOSTEX::iNumElements));});
#pragma endregion

#pragma region Buffer

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),CVIBuffer_Rect::Create(m_pDevice, m_pContext));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBufferPoint"),CVIBuffer_Point::Create(m_pDevice, m_pContext));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),CVIBuffer_Cube::Create(m_pDevice, m_pContext));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),CVIBuffer_Terrain::Create(m_pDevice, m_pContext));});
   
#pragma endregion

#pragma region Collider

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB));});
   
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE));});

#pragma endregion
  
#pragma region Player
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/Player.dat"),XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY((XMConvertToRadians(180.f)))));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HendGun"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/HendGun.dat"),XMMatrixIdentity()));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_AssaultRifle"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/AssaultRifle.dat"),XMMatrixIdentity()));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MissileGatling"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/MissileGatling.dat"),XMMatrixIdentity()));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HeavyCrossbow"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/HeavyCrossbow.dat"),XMMatrixIdentity()));});
#pragma endregion 

#pragma region Weapon
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_AssaultRifle_NonAni"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                     TEXT("../Bin/Data/NonAni/AssaultRifle_NonAni.dat"),XMMatrixRotationY((XMConvertToRadians(45.f))) * XMMatrixRotationZ((XMConvertToRadians(25.f)))));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_MissileGatling_NonAni"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                     TEXT("../Bin/Data/NonAni/MissileGatling_NonAni.dat"),XMMatrixRotationY((XMConvertToRadians(45.f))) * XMMatrixRotationZ((XMConvertToRadians(25.f)))));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_HeavyCrossbow_NonAni"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                     TEXT("../Bin/Data/NonAni/HeavyCrossbow_NonAni.dat"),XMMatrixRotationY((XMConvertToRadians(45.f))) * XMMatrixRotationZ((XMConvertToRadians(25.f)))));});
#pragma endregion 

#pragma region Monster
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto_Component_GunPawn_Model"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/GunPawn.dat"),XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f))));});
   
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto_Component_JetFly_Model"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/JetFly.dat"),XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f))));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto_Component_BoomBot_Model"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/SKT_BoomBot.dat"),XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f))));});
   
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_ComPonent_MecanoBot"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/SKT_MecanoBot.dat"),XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f))));});
  
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_ComPonent_BillyBoom"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/SKT_BillyBoom.dat"),XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f))));});
    //*Prototype_Component_NPC*//
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Proto Component HealthBot_Model"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                     TEXT("../Bin/Data/Ani/HealthBot.dat"),XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f))));});
#pragma endregion 

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext));});

#pragma region Partcle
  CVIBuffer_Instancing::INSTANCING_DESC ParticleExploDesc{};
  ParticleExploDesc.iNumInstance = 90;
  ParticleExploDesc.vCenter = _float3(0.f, 0.f, 0.f);
  ParticleExploDesc.vRange = _float3(8.f, 8.f, 7.f);
  ParticleExploDesc.vSize = _float2(2.f, 2.f);
  ParticleExploDesc.vSpeed = _float2(0.5f, 0.6f);
  ParticleExploDesc.vLifeTime = _float2(10.f, 0.5f);
  ParticleExploDesc.vPivot = _float3(0.f, 9.f, 0.f);
  ParticleExploDesc.isLoop = false;
  
  if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &ParticleExploDesc))))
      return E_FAIL; 
#pragma endregion

    return S_OK;
}

HRESULT CLoader::Loading_For_Static_Texture()
{
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_HpBar"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerUI/PlayerHPBar%d.png"),3));});
		
	m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_WeaPonTEX"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerUI/WeaPon%d.dds"),6));});

	m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_KeyBoard_F"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/InteractiveUI/T_Keyboard_Black_F.dds")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_Aim"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerUI/Aim0.png")));});
	
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pade"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Pade.png")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Canyon"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Canyon%d.png"), 6));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky.dds")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_BusSmokeBack"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bus/Back%d.png"), 20));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_BusSmokefront"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Bus/Front%d.png"), 20));});

	m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Loading"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Loading.png")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterHP"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MonsterHP/MonsterHP%d.dds"),2));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShootEffect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_VFX_Sprite-impact-spark_Gen2.dds")));});

	m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sprint"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_SprintLayer_3C.dds")));});
	
	m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UIEffect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/UIEffect.png")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PrrrEffect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Gen2_Rifle_PRRR_ga.dds")));});

	m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ClackEffect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Gen2_Rifle_Clack.dds")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TAAEffect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Gen2_Rifle_TAA.dds")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_tAkAEffect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_JunkRifle_TAKA.dds")));});
     
    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Clack2Effect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Gen1_Clack.dds")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FlinkEffect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Gen2_Flink.dds")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TRAEffect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Gen2_Rifle_TRA2.dds")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_spackEffect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Shooting/spack%d.png"),5));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_PlayerBullet"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/PlayerBullet0.png")));});

	m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_PlayerBulletBoom"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/PlayerBullet1.png")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterBullet"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/MonsterBullet.png")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShockWaveEffect"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/ShokWave_Ground_01_output.dds")));});

    m_pGameInstance->Add_Job([this](){return m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
                    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Mask/T_Noise_Liquid.dds")));});

    return S_OK;
}

HRESULT CLoader::Loading_For_MenuLevel()
{
    m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_MENU, TEXT("Prototype_Component_Texture_Cusor"),
            CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/T_MouseCursor.dds")))))
        return E_FAIL;

	/* For.Prototype_Component_Texture_Menu */
 	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_MENU, TEXT("Prototype_Component_Texture_Menu"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MainMenu/T_MainMenu_Background%d.png"),3))))
		return E_FAIL;
	
	m_strLoadingText = TEXT("로딩 완료되었습니다.");

	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1Level()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Texture1_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.dds")))))
		return E_FAIL;
	

		m_strLoadingText = TEXT("네비게이션 로딩중입니다.");
		CComponent* pComponent =  m_pGameInstance->Find_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"));

		static_cast<CNavigation*>(pComponent)->Load(L"../Bin/Data/Navigation/Navigation_Stage1.dat");


m_strLoadingText = TEXT("모델 로딩중입니다.");
        _matrix PreTransformMatrix = XMMatrixIdentity();

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Bus Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Bus.dat"), PreTransformMatrix))))
		    return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component RockWallLarge01 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/RockWallLarge.dat"), PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component RockWallLarge02 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/RockWallLarge02.dat"), PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component RockWallLarge03 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/RockWallLarge03.dat"), PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component RockWallsmall Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/RockWallsmall.dat"), PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component RockBase Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/RockBase.dat"),PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Desertic Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
            TEXT("../Bin/Data/NonAni/Desertic.dat"),PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Rock.dat"),PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Rock2.dat"),PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock3 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Rock3.dat"),PreTransformMatrix))))
            return E_FAIL;
        
        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock4 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Rock4.dat"),PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock5 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Rock5.dat"),PreTransformMatrix))))
            return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component struck Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/struck.dat"), PreTransformMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component struck2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
            TEXT("../Bin/Data/NonAni/struck2.dat"), PreTransformMatrix))))
			return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock6 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Rock6.dat"), PreTransformMatrix))))
		    return E_FAIL;
	    
        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock7 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Rock7.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock8 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
            TEXT("../Bin/Data/NonAni/RocK8.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Arch3 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
            TEXT("../Bin/Data/NonAni/Arch3.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Arch2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Arch2.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Rock small wall1 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
            TEXT("../Bin/Data/NonAni/Rock_small_wall1.dat"), PreTransformMatrix))))
		    return E_FAIL;

    	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Wall Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
            TEXT("../Bin/Data/NonAni/Wall.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Wall2 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
            TEXT("../Bin/Data/NonAni/Wall2.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Wall3 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Wall3.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component DoorRock Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/DoorRock.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Cactus Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Cactus.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Cattail2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Cattail2.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Vine Falling1 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Vine_Falling1.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Vine Falling2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
            TEXT("../Bin/Data/NonAni/Vine_Falling2.dat"), PreTransformMatrix))))
	   	    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component vine1 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/vine1.dat"), PreTransformMatrix))))
	     	return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component vine2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Vine2.dat"), PreTransformMatrix))))
	     	return E_FAIL;
       
	   if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Platkit Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
            TEXT("../Bin/Data/NonAni/Platkit.dat"), PreTransformMatrix))))
	   	    return E_FAIL;

	   if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component Platkit2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
            TEXT("../Bin/Data/NonAni/Platkit2.dat"), PreTransformMatrix))))
	     	return E_FAIL;

		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) ;
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component ChestWeapon Model_aniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
            TEXT("../Bin/Data/Ani/ChestWeapon.dat"), PreTransformMatrix))))
			return E_FAIL;  

		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component StageDoor Model_aniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
            TEXT("../Bin/Data/Ani/StageDoor.dat"), PreTransformMatrix))))
			return E_FAIL;
		
		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE1, TEXT("Proto Component ItemDoor Model_aniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
            TEXT("../Bin/Data/Ani/WPDoor.dat"), PreTransformMatrix))))
			return E_FAIL;
	
	m_strLoadingText = TEXT("로딩 완료되었습니다.");

	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Stage2Level()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Texture1_Terrain"),
            CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.dds")))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Fire1_Terrain"),
            CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/T_Fire_Pattern_02.dds")))))
        return E_FAIL;
	m_strLoadingText = TEXT("네비게이션 로딩중입니다.");


	m_pGameInstance->Get_Player()->Set_onCell(false);
	m_pGameInstance->Get_Player()->Clear_CNavigation(L"../Bin/Data/Navigation/Navigation_Stage2.dat");
	CComponent* pComponent = m_pGameInstance->Find_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"));
	static_cast<CNavigation*>(pComponent)->Delete_ALLCell();
	static_cast<CNavigation*>(pComponent)->Load(L"../Bin/Data/Navigation/Navigation_Stage2.dat");


m_strLoadingText = TEXT("모델 로딩중입니다.");
     _matrix PreTransformMatrix = XMMatrixIdentity();
   
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component RockWallLarge01 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/RockWallLarge.dat"),PreTransformMatrix))))
        return E_FAIL;
   
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component RockWallLarge02 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/RockWallLarge02.dat"),PreTransformMatrix))))
        return E_FAIL;
   
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component RockWallLarge03 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/RockWallLarge03.dat"),PreTransformMatrix))))
        return E_FAIL;
   
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component RockWallsmall Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/RockWallsmall.dat"),PreTransformMatrix))))
        return E_FAIL;
   
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component RockBase Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/RockBase.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Desertic Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Desertic.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Rock.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Rock2.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock3 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Rock3.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock4 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Rock4.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock5 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Rock5.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock6 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Rock6.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock7 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Rock7.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock8 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/RocK8.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Arch3 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Arch3.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Arch2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Arch2.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Rock small wall1 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Rock_small_wall1.dat"),PreTransformMatrix))))
        return E_FAIL;
     
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Cactus Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Cactus.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Cattail2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Cattail2.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Vine Falling1 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
              TEXT("../Bin/Data/NonAni/Vine_Falling1.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Vine Falling2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Vine_Falling2.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component vine1 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/vine1.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component vine2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Vine2.dat"), PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Platkit Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Platkit.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Platkit2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Platkit2.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Pylon Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Pylon.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Wall Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Wall.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Wall2 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Wall2.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component Wall3 Model_Wall"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/Wall3.dat"),PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component DoorRock Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
              TEXT("../Bin/Data/NonAni/DoorRock.dat"),PreTransformMatrix))))
        return E_FAIL;

    PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component ChestWeapon Model_aniObj"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
              TEXT("../Bin/Data/Ani/ChestWeapon.dat"), PreTransformMatrix))))
        return E_FAIL;

    PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component StageDoor Model_aniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
              TEXT("../Bin/Data/Ani/StageDoor.dat"), PreTransformMatrix))))
	    return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component BossDoor Model_aniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, 
              TEXT("../Bin/Data/Ani/BossDoor.dat"), PreTransformMatrix))))
		return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STAGE2, TEXT("Proto Component ItemDoor Model_aniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
              TEXT("../Bin/Data/Ani/WPDoor.dat"), PreTransformMatrix))))
		return E_FAIL;


	m_strLoadingText = TEXT("로딩 완료되었습니다.");

	m_bFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_BossLevel()
{
	m_strLoadingText = TEXT("텍스쳐 로딩중입니다.");
    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Intro"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Intro/T_Intro_Billy%d.Png"), 3))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_BossHP"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MonsterHP/T_BossHealthBar.png")))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Fire1_Terrain"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/T_Fire_Pattern_02.dds")))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Fire_Explostion"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_FireBallAtlas_02.dds")))))
        return E_FAIL;
		
	m_strLoadingText = TEXT("네비게이션 로딩중입니다.");

	m_pGameInstance->Get_Player()->Set_onCell(false);
	m_pGameInstance->Get_Player()->Clear_CNavigation(L"../Bin/Data/Navigation/Navigation_Boss.dat");
	CComponent* pComponent = m_pGameInstance->Find_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"));
	static_cast<CNavigation*>(pComponent)->Delete_ALLCell();
	static_cast<CNavigation*>(pComponent)->Load(L"../Bin/Data/Navigation/Navigation_Boss.dat");


m_strLoadingText = TEXT("모델 로딩중입니다.");
        _matrix PreTransformMatrix = XMMatrixIdentity();
         if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Bus Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/Bus.dat"), PreTransformMatrix))))
             return E_FAIL;
        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component RockWallLarge02 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/RockWallLarge02.dat"), PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component RockWallLarge03 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/RockWallLarge03.dat"), PreTransformMatrix))))
            return E_FAIL;

       	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component RockWallsmall Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/RockWallsmall.dat"), PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Rock small wall1 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/Rock_small_wall1.dat"), PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Rock8 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/RocK8.dat"), PreTransformMatrix))))
            return E_FAIL;		
        
        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Arch3 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/Arch3.dat"),PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Arch2 Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/Arch2.dat"),PreTransformMatrix))))
            return E_FAIL;

        if(FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Pylon Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/Pylon.dat"),PreTransformMatrix))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Wall Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
                   TEXT("../Bin/Data/NonAni/Wall.dat"), PreTransformMatrix))))
		    return E_FAIL;	

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component Wall2 Model_Wall"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/Wall2.dat"), PreTransformMatrix))))
		    return E_FAIL;

	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component BossFloor Model_nonaniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/BossFloor.dat"), PreTransformMatrix))))
	    	return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_Beam"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
                   TEXT("../Bin/Data/NonAni/Beam.dat"), PreTransformMatrix))))
		    return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_FireBallSP"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, 
                   TEXT("../Bin/Data/NonAni/Sphere.dat"), PreTransformMatrix))))
		    return E_FAIL;

        PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto Component BossDoor Model_aniObj"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
                   TEXT("../Bin/Data/Ani/BossDoor.dat"),PreTransformMatrix))))
            return E_FAIL;

        
         PreTransformMatrix = XMMatrixIdentity();
	    if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto_Component_ShockWave"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/ShockWave.dat"), PreTransformMatrix))))
		    return E_FAIL;

        if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_BOSS, TEXT("Proto_Component_Shock"),CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
                   TEXT("../Bin/Data/NonAni/Shock.dat"), PreTransformMatrix))))
		    return E_FAIL;
		 
	m_strLoadingText = TEXT("로딩 완료되었습니다.");

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
