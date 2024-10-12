#include "stdafx.h"
#include "Level_Stage1.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "PlayerUI.h"
#include "Camera_Free.h"
#include "Player.h"

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


	if (FAILED(Ready_Layer_UI(CGameObject::UI)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(CGameObject::CAMERA)))
		return E_FAIL;



	if (FAILED(Ready_Layer_Map(CGameObject::MAP)))
		return E_FAIL;





	return S_OK;
}

void CLevel_Stage1::Update(_float fTimeDelta)
{
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
	CContainerObject::CONTAINEROBJECT_DESC Desc{};
	Desc.POSITION = XMVectorSet( - 13.f, 0.f, 90.f, 1.f );

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag,
		TEXT("Prototype_GameObject_GunPawn"),nullptr,0 , &Desc)))
		return E_FAIL;
	


	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Camera(const _uint& pLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC			Desc{};

	_vector vEye = {static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_CameraBone()->_41,
		    	 static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_CameraBone()->_42,
		         static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_CameraBone()->_43,
		         static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_CameraBone()->_44};

	XMStoreFloat4(&Desc.vEye , XMVector3TransformCoord(vEye, m_pGameInstance->Get_Player()->Get_Transform()->Get_WorldMatrix()));

	_float4 At;
	XMStoreFloat4(&At, m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)
		+ m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_LOOK));
	Desc.vAt = At;
	Desc.fFovy = XMConvertToRadians(30.0f);
	Desc.fNearZ = 0.1f;
	Desc.fFarZ = 500.f;
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fSpeedPerSec = m_pGameInstance->Get_Player()->Get_Transform()->Get_MoveSpeed();
	Desc.fRotationPerSec = m_pGameInstance->Get_Player()->Get_Transform()->Get_RotSpeed(); 
	Desc.fMouseSensor = 0.05f;
	Desc.JumpPower = m_pGameInstance->Get_Player()->Get_Transform()->Get_JumpPower();
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag,
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
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_Cursor, false)))
		return S_OK;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Map(const _uint& pLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Proto GameObject_Terrain", L"../Bin/Data/Map/SetMap_Stage1_terrain.dat", CGameObject::DATA_TERRAIN)))
		return   E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype GameObject_NonAniObj", L"../Bin/Data/Map/SetMap_Stage1_Nonani.dat", CGameObject::DATA_NONANIMAPOBJ)))
 		return   E_FAIL;


	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag,L"Prototype GameObject_CHEST", L"../Bin/Data/Map/SetMap_Stage1_ani.dat", CGameObject::DATA_CHEST)))
		return   E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype GameObject_WALL", L"../Bin/Data/Map/SetMap_Stage1_Wall.dat", CGameObject::DATA_WALL)))
		return   E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STAGE1, pLayerTag, L"Prototype GameObject_DOOR", L"../Bin/Data/Map/SetMap_Stage1_ani.dat", CGameObject::DATA_DOOR)))
		return   E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Player(const _uint& pLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, pLayerTag,
		TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;
	m_pGameInstance->Set_Player(m_pGameInstance->Recent_GameObject(pLayerTag));

	m_pGameInstance->Get_Player()->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(25.f, 2.f, -12.f, 1.f));


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
