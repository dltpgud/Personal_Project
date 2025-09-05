#include "stdafx.h"
#include "Level_Stage2.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Player_HpUI.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Level_StageBoss.h"
#include "Fade.h"
#include "Terrain.h"
#include "Trigger.h"
CLevel_Stage2::CLevel_Stage2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Stage2::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

    if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
		return E_FAIL;

	if (FAILED(Ready_UI()))
        return E_FAIL;

	if (FAILED(Ready_Find_cell()))
		return E_FAIL;
     
	return S_OK;
}

void CLevel_Stage2::Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsOpenStage() && m_pFade->FinishPade())
	{
		m_pGameInstance->Set_Open_Bool(false);
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_BOSS));
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
    CTerrain::CTerrain_DESC TDESC{};
    CTrigger::CTrigger_DESC TriggerDesc{};
    if (FAILED(Load_to_Next_Map_terrain(LEVEL_STAGE2, pLayerTag, L"Proto GameObject_Terrain",
                                        L"../Bin/Data/Map/SetMap_Stage2_terrain.dat", &TDESC)))
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

   if (FAILED(Load_to_Next_Map_Trigger(LEVEL_STAGE2, pLayerTag, L"Prototype GameObject_Trigger",
                                       L"../Bin/Data/Map/Stage2_Trigger.dat", &TriggerDesc)))
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

    
   _float4x4 ViewMatrix, ProjMatrix;
        _vector m_Eye = XMVectorSet(41, 812, 211, 0.f);
        _vector m_Dire = XMVectorSet(138.6, 17, 2, 0.f);

        XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_Eye, m_Dire, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
        XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f),
                                                              (_float)g_iWinSizeX / g_iWinSizeY, 0.1, 1000));

        m_pGameInstance->Set_ShadowTransformMatrix(CPipeLine::TRANSFORM_STATE::D3DTS_VIEW, XMLoadFloat4x4(&ViewMatrix));

        m_pGameInstance->Set_ShadowTransformMatrix(CPipeLine::TRANSFORM_STATE::D3DTS_PROJ, XMLoadFloat4x4(&ProjMatrix));


	return S_OK;
}

HRESULT CLevel_Stage2::Ready_UI()
{
    m_pFade = static_cast<CFade*>(m_pGameInstance->Find_Clone_UIObj(L"Fade"));
    Safe_AddRef(m_pFade);

    m_pGameInstance->PlayBGM(&m_pChannel, L"ST_Ambient_Special.ogg", 0.5f);
	m_pGameInstance->Set_OpenUI(true,TEXT("PlayerHP"));
    m_pGameInstance->Set_OpenUI(true, TEXT("WeaPon_Aim"));
    m_pGameInstance->Set_OpenUI(true,TEXT("WeaponUI"));
    return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_Player()
{
    static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::FLAG_UPDATE, true);

	m_pGameInstance->Get_Player()->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(5.f, 0.f, 6.f, 1.f));

	return S_OK;
}

HRESULT CLevel_Stage2::Load_to_Next_Map_terrain(const _uint& iLevelIndex, const _wstring& strLayerTag,
                                         const _wstring& strProto, const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    _matrix World = XMMatrixIdentity();
    _uint Type{0};
    _uint TileX{0};
    _uint TileY{0};
    _bool bMainTile = true;
    _uint iLava{0};
    _float FireOffset{0.f};
    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(World), sizeof(_matrix), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(Type), sizeof(_uint), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(TileX), sizeof(_uint), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(TileY), sizeof(_uint), &dwByte, nullptr);

        // wstring 문자열 길이
        DWORD strLength;
        bFile = ReadFile(hFile, &strLength, sizeof(DWORD), &dwByte, NULL);

        // wstring 데이터 읽기
        wchar_t* pModel = new wchar_t[strLength + 1]; // NULL 종단 추가
        bFile = ReadFile(hFile, pModel, strLength * sizeof(wchar_t), &dwByte, NULL);
        pModel[strLength] = L'\0';

        DWORD Length;
        bFile = ReadFile(hFile, &Length, sizeof(DWORD), &dwByte, nullptr);
        wchar_t* pPoroto = new wchar_t[Length + 1];
        bFile = ReadFile(hFile, pPoroto, Length, &dwByte, nullptr);

        bFile = ReadFile(hFile, &(iLava), sizeof(_uint), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(FireOffset), sizeof(_float), &dwByte, nullptr);

        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
            break;
        }

        CTerrain::CTerrain_DESC* pDesc = static_cast<CTerrain::CTerrain_DESC*>(Arg);
        pDesc->WorldMatrix = World;
        if (true == bMainTile)
        {
            pDesc->IsMain = true;
        }
        else
            pDesc->IsMain = false;

        pDesc->ProtoName = pModel;
        pDesc->CuriLevelIndex = iLevelIndex;
        pDesc->Buffer[0] = TileX;
        pDesc->Buffer[1] = TileY;
        pDesc->ilava = iLava;
        pDesc->flavaOffset = FireOffset;

        CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(strProto, pDesc);
        bMainTile = false;
        m_pGameInstance->Add_Clon_to_Layers(iLevelIndex, strLayerTag, pGameObject);

        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_Stage2::Load_to_Next_Map_Trigger(const _uint& iLevelIndex, const _wstring& strLayerTag,
                                                const _wstring& strProto, const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);

    _matrix WorldMat = XMMatrixIdentity();
    _uint Type = {0};
    _int TriggerType = {0};
    _int ColType = {0};
    _tchar* pPoroto = {};
    _int ID{};
    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(WorldMat), sizeof(_matrix), &dwByte, nullptr);

        bFile = ReadFile(hFile, &(Type), sizeof(_uint), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(ColType), sizeof(_int), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(TriggerType), sizeof(_int), &dwByte, nullptr);

        DWORD Length;
        bFile = ReadFile(hFile, &Length, sizeof(DWORD), &dwByte, NULL);

        pPoroto = new _tchar[Length + 1];
        bFile = ReadFile(hFile, pPoroto, Length, &dwByte, NULL);
        pPoroto[Length] = L'\0';

        if (0 == dwByte)
        {

            Safe_Delete_Array(pPoroto);
            break;
        }

        CTrigger::CTrigger_DESC* pDesc = static_cast<CTrigger::CTrigger_DESC*>(Arg);
        pDesc->ProtoName = pPoroto;
        pDesc->Object_Type = ID++;
        pDesc->WorldMatrix = WorldMat;
        pDesc->iColType = ColType;
        pDesc->iTriggerType = TriggerType;
        pDesc->CuriLevelIndex = iLevelIndex;
        CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(strProto, pDesc);
        m_pGameInstance->Add_Clon_to_Layers(iLevelIndex, strLayerTag, pGameObject);

        Safe_Delete_Array(pPoroto);
    }

    CloseHandle(hFile);

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
    m_pGameInstance->StopSound(&m_pChannel);
    Safe_Release(m_pFade);
}
