#include "stdafx.h"
#include "Level_StageBoss.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "PlayerUI.h"
#include "Camera_Free.h"
#include "SceneCamera.h"


CLevel_StageBoss::CLevel_StageBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_StageBoss::Initialize()
{

	m_pGameInstance->Get_Player()->Set_onCell(false);
	m_pGameInstance->Get_Player()->Clear_CNavigation(L"../Bin/Data/Navigation/Navigation_Boss.dat");
	CComponent* pComponent = m_pGameInstance->Find_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"));
	static_cast<CNavigation*>(pComponent)->Delete_ALLCell();
	static_cast<CNavigation*>(pComponent)->Load(L"../Bin/Data/Navigation/Navigation_Boss.dat");

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

	if (FAILED(Ready_Layer_UI(CGameObject::UI)))
		return E_FAIL;

	if (FAILED(Ready_Find_cell()))
		return E_FAIL;


	return S_OK;
}

void CLevel_StageBoss::Update(_float fTimeDelta)
{

	__super::Update(fTimeDelta);
}

HRESULT CLevel_StageBoss::Render()
{
	__super::Render();

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("스테이지 보스 레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_StageBoss::Ready_Layer_Monster(const _uint& pLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, pLayerTag, L"Prototype_GameObject_BillyBoom",
		L"../Bin/Data/Monster/BossStage_Monster.dat", CGameObject::DATA_MONSTER, nullptr, CActor::TYPE_BOSS)))
		return   E_FAIL;

	return S_OK;
}

HRESULT CLevel_StageBoss::Ready_Layer_Camera(const _uint& pLayerTag)
{
	static_cast<CCamera_Free*>(m_pGameInstance->Find_CloneGameObject(LEVEL_STATIC, CGameObject::CAMERA, CGameObject::DATA_CAMERA))->Set_Update(false);

	CGameObject* pMonster = m_pGameInstance->Find_CloneGameObject(LEVEL_BOSS, CGameObject::ACTOR, CGameObject::DATA_MONSTER);

	CSceneCamera::CSceneCamera_DESC			Desc{};
	_float4 fEye = { 50.f, 5.f, 16.f, 1.f };
	
 	_float4 fCamPos{};
 	XMStoreFloat4(&fCamPos, pMonster->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION));
	
	_float4 At = { fCamPos.x, 5.f, fCamPos.z, fCamPos.w };

    Desc.vEye = fEye;
	Desc.vAt = At;
	Desc.fFovy = XMConvertToRadians(45.0f);
	Desc.fNearZ = 0.1f;
	Desc.fFarZ = 500.f;
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fSpeedPerSec = 10.f;
	Desc.vStopPos = XMVectorSet(50.f, 5, fCamPos.z -20.f, fCamPos.w);
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, pLayerTag,
		TEXT("Prototype_GameObject_SceneCamera"), nullptr, 0, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StageBoss::Ready_Layer_UI(const _uint& pLayerTag)
{
	// 보스 인트로 떄문에 다꺼놔
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerHP, false)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerWeaPon, false)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerWeaPon_Aim, false)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_Cursor, false)))
		return S_OK;

	return S_OK;
}

HRESULT CLevel_StageBoss::Ready_Layer_Map(const _uint& pLayerTag)
{
 	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, pLayerTag, L"Proto GameObject_Terrain", L"../Bin/Data/Map/SetMap_Boss_terrain.dat", CGameObject::DATA_TERRAIN)))
		return   E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, pLayerTag, L"Prototype GameObject_NonAniObj", L"../Bin/Data/Map/SetMap_Boss_Nonani.dat", CGameObject::DATA_NONANIMAPOBJ)))
 		return   E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, pLayerTag, L"Prototype GameObject_WALL", L"../Bin/Data/Map/SetMap_Boss_Wall.dat", CGameObject::DATA_WALL)))
		return   E_FAIL;
	
 	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, pLayerTag, L"Prototype GameObject_DOOR", L"../Bin/Data/Map/SetMap_Boss_ani.dat", CGameObject::DATA_DOOR)))
		return   E_FAIL;

	return S_OK;
}


HRESULT CLevel_StageBoss::Ready_Find_cell()
{
	  m_pGameInstance->Find_Cell();
	return S_OK;
}

HRESULT CLevel_StageBoss::Ready_Light()
{
	m_pGameInstance->Light_Clear();
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

HRESULT CLevel_StageBoss::Ready_Layer_Player(const _uint& pLayerTag)
{
	m_pGameInstance->Get_Player()->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(50.f, 0.f, 15.f, 1.f));
	return S_OK;
}

CLevel_StageBoss* CLevel_StageBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_StageBoss* pInstance = new CLevel_StageBoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_StageBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_StageBoss::Free()
{
	__super::Free();

}
