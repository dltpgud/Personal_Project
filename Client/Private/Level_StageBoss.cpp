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
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	 	return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	if (FAILED(Ready_Find_cell()))
		return E_FAIL;

	m_pGameInstance->PlayBGM(CSound::SOUND_LEVEL, L"ST_Ambient_EnergyCenter.ogg", 0.1f);
	return S_OK;
}

void CLevel_StageBoss::Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsOpenStage())
	{
		m_pGameInstance->Set_Open_Bool(false);
	//	m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_RETURN_MENU, STAGE));

	}


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

HRESULT CLevel_StageBoss::Ready_Layer_Monster(const _wstring& pLayerTag)
{
    CActor::Actor_DESC Desc{};
    if (FAILED(Load_to_Next_Map_Monster(LEVEL_BOSS, pLayerTag, L"Prototype_GameObject_BillyBoom",
                                        L"Proto Component Boss_Monster", L"../Bin/Data/Monster/BossStage_Monster.dat",
                                              &Desc)))
        return E_FAIL;
	return S_OK;
}

HRESULT CLevel_StageBoss::Ready_Layer_Camera(const _wstring& pLayerTag)
{
	static_cast<CCamera_Free*>(m_pGameInstance->Find_CloneGameObject(LEVEL_STATIC,pLayerTag, CGameObject::GAMEOBJ_TYPE::CAMERA))->Set_Update(false);

	CGameObject* pMonster =
            m_pGameInstance->Find_CloneGameObject(LEVEL_BOSS, TEXT("Layer_Monster"), CGameObject::ACTOR);

	CSceneCamera::CSceneCamera_DESC			Desc{};
	_float4 fEye = { 50.f, 5.f, 16.f, 1.f };
	
 	_float4 fCamPos{};
 	XMStoreFloat4(&fCamPos, pMonster->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION));
	
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
		TEXT("Prototype_GameObject_SceneCamera"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StageBoss::Ready_UI()
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

HRESULT CLevel_StageBoss::Ready_Layer_Map(const _wstring& pLayerTag)
{
    CGameObject::GAMEOBJ_DESC Desc{};

    if (FAILED(Load_to_Next_Map_terrain(LEVEL_BOSS, pLayerTag, L"Proto GameObject_Terrain",
                                        L"../Bin/Data/Map/SetMap_Boss_terrain.dat", &Desc)))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_NonaniObj(LEVEL_BOSS, pLayerTag, L"Prototype GameObject_NonAniObj",
                                          L"../Bin/Data/Map/SetMap_Boss_Nonani.dat")))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_AniOBj(LEVEL_BOSS, pLayerTag, L"Proto GameObject Door_aniObj",
                                       L"../Bin/Data/Map/SetMap_Boss_ani.dat", &Desc)))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_Wall(LEVEL_BOSS, pLayerTag, L"Prototype GameObject_WALL",
                                     L"../Bin/Data/Map/SetMap_Boss_Wall.dat", &Desc)))
        return E_FAIL;

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
	LightDesc.vDirection = _float4(0.f, -1.f, 2.f, 0.f);
	LightDesc.vDiffuse = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StageBoss::Ready_Layer_Player(const _wstring& pLayerTag)
{
	m_pGameInstance->Get_Player()->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(50.f, 0.f, 15.f, 1.f));
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
