#include "stdafx.h"
#include "Level_Stage1.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "PlayerUI.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Actor.h"
#include "Level_Stage2.h"
#include "Pade.h"
#include "HealthBot.h"

CLevel_Stage1::CLevel_Stage1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Stage1::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

 	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
		return E_FAIL;

	if (FAILED(Ready_Find_cell()))
		return E_FAIL;

	m_pGameInstance->PlayBGM(CSound::SOUND_LEVEL, L"ST_Ambient_Canyon.ogg", 0.1f);
	return S_OK;
}

void CLevel_Stage1::Update(_float fTimeDelta)
{
	
	if (m_bPade == false) {
        static_cast<CPade*>(m_pGameInstance->Find_Clone_UIObj(L"Fade"))->Set_Pade(false);
		m_pGameInstance->Set_OpenUI(CUI::UIID_Pade, true);
		m_bPade = true;
	}


	if (m_pGameInstance->IsOpenStage())
	{
		m_pGameInstance->Set_Open_Bool(false);
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_STAGE2));
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

HRESULT CLevel_Stage1::Ready_Layer_Monster(const _wstring& pLayerTag)
{
    CActor::Actor_DESC Desc{};
    if (FAILED(Load_to_Next_Map_Monster(LEVEL_STAGE1, pLayerTag, L"Prototype_GameObject_GunPawn",
                                        L"Proto Component GunPawn_Monster",
                                        L"../Bin/Data/Monster/Stage1_Monster.dat", &Desc)))
  	  return E_FAIL;

  if (FAILED(Load_to_Next_Map_Monster(LEVEL_STAGE1, pLayerTag, L"Prototype_GameObject_JetFly",
                                        L"Proto Component JetFly_Monster",
                                        L"../Bin/Data/Monster/Stage1_Monster.dat", &Desc)))
      return E_FAIL;

  if (FAILED(Load_to_Next_Map_Monster(LEVEL_STAGE1, pLayerTag, L"Prototype_GameObject_BoomBot",
                                      L"Proto Component BoomBot_Monster",
                                      L"../Bin/Data/Monster/Stage1_Monster.dat", &Desc)))
      return E_FAIL;

  if (FAILED(Load_to_Next_Map_Monster(LEVEL_STAGE1, pLayerTag, L"Prototype_GameObject_MecanoBot",
                                      L"Proto Component MecanoBot_Monster", L"../Bin/Data/Monster/Stage1_Monster.dat",
                                      &Desc)))
      return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Camera(const _wstring& pLayerTag)
{
  CCamera_Free::CAMERA_FREE_DESC			Desc{};

  const _float4x4* fcamBone = 	static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_CameraBone();

 _vector vEyePos = {fcamBone->_41, fcamBone->_42, fcamBone->_43, fcamBone->_44};

 _vector vEye = XMVector3TransformCoord(vEyePos, m_pGameInstance->Get_Player()->Get_Transform()->Get_WorldMatrix());

 _float4 fEye{};
 XMStoreFloat4(&fEye, vEye);

 Desc.vEye = fEye; 

 _float4 At{};
 XMStoreFloat4(&At, vEye + m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_LOOK));
	 
 Desc.vAt = At;
 Desc.fFovy = XMConvertToRadians(45.0f);
 Desc.fNearZ = 0.1f;
 Desc.fFarZ = 500.f;
 Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
 if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &Desc)))
	return E_FAIL;

   _float4x4			ViewMatrix, ProjMatrix;
 _vector m_Eye = XMVectorSet(143, 394, 432, 0.f);
 _vector m_Dire = XMVectorSet(139, 17, 2, 0.f);

   XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_Eye,  m_Dire, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
   XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), (_float)g_iWinSizeX / g_iWinSizeY,
                                                         Desc.fNearZ, 1000));

	m_pGameInstance->Set_ShadowTransformMatrix(CPipeLine::TRANSFORM_STATE::D3DTS_VIEW, XMLoadFloat4x4(&ViewMatrix));

	m_pGameInstance->Set_ShadowTransformMatrix(CPipeLine::TRANSFORM_STATE::D3DTS_PROJ, XMLoadFloat4x4(&ProjMatrix));

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Map(const _wstring& pLayerTag)
{
    CGameObject::GAMEOBJ_DESC Desc{};
    if (FAILED(Load_to_Next_Map_terrain(LEVEL_STAGE1, pLayerTag, L"Proto GameObject_Terrain",
                                        L"../Bin/Data/Map/SetMap_Stage1_terrain.dat", &Desc)))
		return E_FAIL;

	if (FAILED(Load_to_Next_Map_NonaniObj(LEVEL_STAGE1, pLayerTag, L"Prototype GameObject_NonAniObj",
                                          L"../Bin/Data/Map/SetMap_Stage1_Nonani.dat",&Desc)))
 		return E_FAIL;

    if (FAILED(Load_to_Next_Map_AniOBj(LEVEL_STAGE1, pLayerTag, L"Proto GameObject Door_aniObj",
                                            L"../Bin/Data/Map/SetMap_Stage1_ani.dat", &Desc)))
        return E_FAIL;
                                      
	if (FAILED(Load_to_Next_Map_AniOBj(LEVEL_STAGE1, pLayerTag, L"Proto GameObject Chest_aniObj",
                                            L"../Bin/Data/Map/SetMap_Stage1_ani.dat", &Desc)))
		return E_FAIL;

	if (FAILED(Load_to_Next_Map_Wall(LEVEL_STAGE1, pLayerTag, L"Prototype GameObject_WALL",
                                       L"../Bin/Data/Map/SetMap_Stage1_Wall.dat", &Desc)))
		return E_FAIL;

	 if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, pLayerTag,TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_NPC(const _wstring& pLayerTag)
{   
	 CActor::Actor_DESC HealthBotDesc{};
    if (FAILED(Load_to_Next_Map_NPC(LEVEL_STAGE1, pLayerTag, L"Prototype_GameObject_HealthBot",
                                    L"../Bin/Data/NPC/Stage1_NPC.dat", &HealthBotDesc)))
	return E_FAIL;

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
	LightDesc.vDirection = _float4(0.f, -1.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 0.5f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Player()
{
    m_pGameInstance->Set_Player(TEXT("Prototype_GameObject_Player"));
	m_pGameInstance->Get_Player()->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(25.f, 2.f, -12.f, 1.f));
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
