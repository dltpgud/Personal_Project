#include "stdafx.h"
#include "Level_Stage1.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "PlayerUI.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Actor.h"

CLevel_Stage1::CLevel_Stage1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Stage1::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;


	if (FAILED(Ready_Layer_Player(CGameObject::ACTOR)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(CGameObject::ACTOR)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(CGameObject::CAMERA)))
		return E_FAIL;

 	if (FAILED(Ready_Layer_Map(CGameObject::MAP)))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(CGameObject::NPC)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(CGameObject::UI)))
		return E_FAIL;

	if (FAILED(Ready_Find_cell()))
		return E_FAIL;


	return S_OK;
}

void CLevel_Stage1::Update(_float fTimeDelta)
{

	if (m_pGameInstance->Get_DIKeyDown(DIK_U))
	{

		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE2, GORGE));
	}

	if (m_pGameInstance->IsOpenStage())
	{
		m_pGameInstance->Set_Open_Bool(false);
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE2, GORGE));	


	}

	__super::Update(fTimeDelta);


}

HRESULT CLevel_Stage1::Render()
{
	
	__super::Render();

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("스테이지 1 레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Monster(const _uint& pLayerTag)
{

  if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype_GameObject_GunPawn",
	  L"../Bin/Data/Monster/Stage1_Monster.dat", CGameObject::DATA_MONSTER, nullptr, CActor::TYPE_GUN_PAWN)))
  	return   E_FAIL;
  
  if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype_GameObject_JetFly",
	  L"../Bin/Data/Monster/Stage1_Monster.dat" , CGameObject::DATA_MONSTER, nullptr, CActor::TYPE_JET_FLY)))
  	return   E_FAIL;
  
  if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype_GameObject_BoomBot", 
	  L"../Bin/Data/Monster/Stage1_Monster.dat", CGameObject::DATA_MONSTER, nullptr, CActor::TYPE_BOOM_BOT)))
	return   E_FAIL;

  if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype_GameObject_MecanoBot",
	  L"../Bin/Data/Monster/Stage1_Monster.dat", CGameObject::DATA_MONSTER, nullptr, CActor::TYPE_MECANOBOT)))
	  return   E_FAIL;
	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Camera(const _uint& pLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			Desc{};

  const _float4x4* fcamBone = 	static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_CameraBone();

 _vector vEye = {fcamBone->_41, fcamBone->_42, fcamBone->_43, fcamBone->_44};

 _vector Eye = XMVector3TransformCoord(vEye, m_pGameInstance->Get_Player()->Get_Transform()->Get_WorldMatrix());

 _float4 fEye{};
 XMStoreFloat4(&fEye, Eye);

  Desc.vEye = fEye; 

  _float4 At{};
  XMStoreFloat4(&At, Eye + +m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_LOOK));
	 
	Desc.vAt = At;
	Desc.fFovy = XMConvertToRadians(45.0f);
	Desc.fNearZ = 0.1f;
	Desc.fFarZ = 500.f;
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, pLayerTag,
		TEXT("Prototype_GameObject_Camera_Free"),nullptr,0, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_UI(const _uint& pLayerTag)
{

	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerHP, true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerWeaPon, true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerWeaPon_Aim, true)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_Cursor, false)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_InteractiveUI, false)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Map(const _uint& pLayerTag)
{

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Proto GameObject_Terrain", 
		L"../Bin/Data/Map/SetMap_Stage1_terrain.dat", CGameObject::DATA_TERRAIN)))
		return   E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype GameObject_NonAniObj",
		L"../Bin/Data/Map/SetMap_Stage1_Nonani.dat", CGameObject::DATA_NONANIMAPOBJ)))
 		return   E_FAIL;


	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag,L"Prototype GameObject_CHEST",
		L"../Bin/Data/Map/SetMap_Stage1_ani.dat", CGameObject::DATA_CHEST)))
		return   E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype GameObject_WALL",
		L"../Bin/Data/Map/SetMap_Stage1_Wall.dat", CGameObject::DATA_WALL)))
		return   E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype GameObject_DOOR",
		L"../Bin/Data/Map/SetMap_Stage1_ani.dat", CGameObject::DATA_DOOR)))
		return   E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, pLayerTag,
		TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_NPC(const _uint& pLayerTag)
{

	
   if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype_GameObject_HealthBot", 
	   L"../Bin/Data/NPC/Stage1_NPC.dat", CGameObject::DATA_NPC, nullptr, CActor::TYPE_HEALTH_BOT)))
		return   E_FAIL;
		

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Find_cell()
{
    m_pGameInstance->Find_Cell();
	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.5f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 0.5f);



	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Player(const _uint& pLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, pLayerTag,
		TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;
	m_pGameInstance->Set_Player(m_pGameInstance->Recent_GameObject(LEVEL_STATIC, pLayerTag));

	m_pGameInstance->Get_Player()->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(25.f, 2.f, -12.f, 1.f));
	m_pGameInstance->Get_Player()->Get_Transform()->Rotation(0.f, 0.4f, 0.f);
	return S_OK;
}

CLevel_Stage1* CLevel_Stage1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage1* pInstance = new CLevel_Stage1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Stage1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage1::Free()
{
	__super::Free();

}
