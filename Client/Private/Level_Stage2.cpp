#include "stdafx.h"
#include "Level_Stage2.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "PlayerUI.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Level_StageBoss.h"
#include "Pade.h"
CLevel_Stage2::CLevel_Stage2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Stage2::Initialize()
{


	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
		return E_FAIL;

	

	if (FAILED(Ready_Find_cell()))
		return E_FAIL;
	m_pGameInstance->PlayBGM(CSound::SOUND_LEVEL, L"ST_Ambient_Special.ogg", 0.1f);
	return S_OK;
}

void CLevel_Stage2::Update(_float fTimeDelta)
{

	if (m_bPade == false) {
            static_cast<CPade*>(m_pGameInstance->Find_Clone_UIObj(L"Fade"))->Set_Pade(false);
		m_pGameInstance->Set_OpenUI(CUI::UIID_Pade, true);
		m_bPade = true;
	}

	if (m_pGameInstance->IsOpenStage())
	{
		m_pGameInstance->Set_Open_Bool(false);

//		m_pGameInstance->Open_Level(LEVEL_STAGE2, CLevel_Stage2::Create(m_pDevice, m_pContext));
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_BOSS));
	//	m_pGameInstance->Open_Level(LEVEL_BOSS,CLevel_StageBoss::Create(m_pDevice,m_pContext));

	}
	__super::Update(fTimeDelta);
}

HRESULT CLevel_Stage2::Render()
{
	
	__super::Render();

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("스테이지 2 레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Monster(const _wstring& pLayerTag)
{
    CActor::Actor_DESC Desc{};
    if (FAILED(Load_to_Next_Map_Monster(LEVEL_STAGE2, pLayerTag, L"Prototype_GameObject_GunPawn",
                                        L"Proto Component GunPawn_Monster",
                                        L"../Bin/Data/Monster/Stage2_Monster.dat", &Desc)))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_Monster(LEVEL_STAGE2, pLayerTag, L"Prototype_GameObject_JetFly",
                                        L"Proto Component JetFly_Monster",
                                        L"../Bin/Data/Monster/Stage2_Monster.dat", &Desc)))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_Monster(LEVEL_STAGE2, pLayerTag, L"Prototype_GameObject_BoomBot",
                                        L"Proto Component BoomBot_Monster",
                                        L"../Bin/Data/Monster/Stage2_Monster.dat", &Desc)))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_Monster(LEVEL_STAGE2, pLayerTag, L"Prototype_GameObject_MecanoBot",
                                        L"Proto Component MecanoBot_Monster", L"../Bin/Data/Monster/Stage2_Monster.dat",
                                        &Desc)))
        return E_FAIL;
	return S_OK;
}



HRESULT CLevel_Stage2::Ready_Layer_Map(const _wstring& pLayerTag)
{
    CGameObject::GAMEOBJ_DESC Desc{};
    if (FAILED(Load_to_Next_Map_terrain(LEVEL_STAGE2, pLayerTag, L"Proto GameObject_Terrain",
                                        L"../Bin/Data/Map/SetMap_Stage2_terrain.dat", &Desc)))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_NonaniObj(LEVEL_STAGE2, pLayerTag, L"Prototype GameObject_NonAniObj",
                                          L"../Bin/Data/Map/SetMap_Stage2_Nonani.dat", &Desc)))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_AniOBj(LEVEL_STAGE2, pLayerTag, L"Proto GameObject Door_aniObj", 
                                       L"../Bin/Data/Map/SetMap_Stage2_ani.dat", &Desc)))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_AniOBj(LEVEL_STAGE2, pLayerTag, L"Proto GameObject Chest_aniObj", 
                                       L"../Bin/Data/Map/SetMap_Stage2_ani.dat", &Desc)))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_Wall(LEVEL_STAGE2, pLayerTag, L"Prototype GameObject_WALL",
                                     L"../Bin/Data/Map/SetMap_Stage2_Wall.dat", &Desc)))
        return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_NPC(const _wstring& pLayerTag)
{
    CActor::Actor_DESC HealthBotDesc{};
    if (FAILED(Load_to_Next_Map_NPC(LEVEL_STAGE2, pLayerTag, L"Prototype_GameObject_HealthBot",
                                    L"../Bin/Data/NPC/Stage2_NPC.dat", &HealthBotDesc)))
        return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Find_cell()
{
   m_pGameInstance->Find_Cell();
	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Light()
{
	m_pGameInstance->Light_Clear();
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

HRESULT CLevel_Stage2::Ready_Layer_Player(const _wstring& pLayerTag)
{

	m_pGameInstance->Get_Player()->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(5.f, 0.f, 5.f, 1.f));

	return S_OK;
}

CLevel_Stage2* CLevel_Stage2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage2* pInstance = new CLevel_Stage2(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Stage2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage2::Free()
{
	__super::Free();

}
