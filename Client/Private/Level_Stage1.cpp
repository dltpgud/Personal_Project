#include "stdafx.h"
#include "Level_Stage1.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Player_HpUI.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Actor.h"
#include "Level_Stage2.h"
#include "Fade.h"
#include "HealthBot.h"
#include "Terrain.h"
#include "Trigger.h"
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

	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
		return E_FAIL;

    if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
        return E_FAIL;


    if (FAILED(Ready_UI()))
        return E_FAIL;

	if (FAILED(Ready_Find_cell()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Stage1::Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsOpenStage() && m_pFade->FinishPade())
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
   Desc.fFarZ = 1000.f;
   Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
   if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &Desc)))
	   return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Map(const _wstring& pLayerTag)
{
    CGameObject::GAMEOBJ_DESC Desc{};
    CTerrain::CTerrain_DESC TDESC{};
    CGameObject::GAMEOBJ_DESC SkyDesc{};
    CTrigger::CTrigger_DESC TriggerDesc;
    if (FAILED(Load_to_Next_Map_terrain(LEVEL_STAGE1, pLayerTag, L"Proto GameObject_Terrain",
                                        L"../Bin/Data/Map/SetMap_Stage1_terrain.dat", &TDESC)))
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
   
    if (FAILED(Load_to_Next_Map_Trigger(LEVEL_STAGE1, pLayerTag, L"Prototype GameObject_Trigger",
                                            L"../Bin/Data/Map/Stage1_Trigger.dat", &TriggerDesc)))
		return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, pLayerTag,TEXT("Prototype_GameObject_Sky"),&SkyDesc)))
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
	LightDesc.vDirection = _float4(-0.2f, -1.f, -0.5f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 0.5f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

// 맵 바운딩 박스
        _float3 minBound = _float3(0.f, 0.f, 0.f);
        _float3 maxBound = _float3(600.f, 100.f, 600.f); // 높이 100은 예시

        // 라이트 방향
        _vector vLightDir = XMVector3Normalize(XMLoadFloat4(&LightDesc.vDirection));

        // 맵 중심
        _vector vCenter = 0.5f * (XMLoadFloat3(&minBound) + XMLoadFloat3(&maxBound));

        // 라이트 위치 = 맵 중심 - 라이트 방향 * 거리
        _vector vLightPos = vCenter - vLightDir * 800.f;

        // Up 벡터 보정
        _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        if (fabsf(XMVectorGetX(XMVector3Dot(vUp, vLightDir))) > 0.9f) // 거의 평행일 경우
        {
            vUp = XMVectorSet(0.f, 0.f, 1.f, 0.f); // 다른 축으로 교체
        }

        // View 행렬
        _matrix LightView = XMMatrixLookAtLH(vLightPos, vCenter, vUp);

        // ---- AABB 8점 뽑기 ----
        _float3 corners[8] = {
            {minBound.x, minBound.y, minBound.z}, {maxBound.x, minBound.y, minBound.z},
            {minBound.x, maxBound.y, minBound.z}, {maxBound.x, maxBound.y, minBound.z},
            {minBound.x, minBound.y, maxBound.z}, {maxBound.x, minBound.y, maxBound.z},
            {minBound.x, maxBound.y, maxBound.z}, {maxBound.x, maxBound.y, maxBound.z},
        };

        float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
        float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

        for (int i = 0; i < 8; i++)
        {
            XMVECTOR corner = XMLoadFloat3(&corners[i]);
            XMVECTOR cornerVS = XMVector3TransformCoord(corner, LightView);

            minX = min(minX, XMVectorGetX(cornerVS));
            minY = min(minY, XMVectorGetY(cornerVS));
            minZ = min(minZ, XMVectorGetZ(cornerVS));

            maxX = max(maxX, XMVectorGetX(cornerVS));
            maxY = max(maxY, XMVectorGetY(cornerVS));
            maxZ = max(maxZ, XMVectorGetZ(cornerVS));
        }

        // Ortho 크기 세팅 (타이트하게)
        _float nearZ = minZ - 5.f;
        _float farZ = maxZ + 20.f;

        _matrix LightProj = XMMatrixOrthographicOffCenterLH(minX, maxX, minY, maxY, nearZ, farZ);

        // 게임 인스턴스에 세팅
        m_pGameInstance->Set_ShadowTransformMatrix(CPipeLine::D3DTS_VIEW, LightView);
        m_pGameInstance->Set_ShadowTransformMatrix(CPipeLine::D3DTS_PROJ, LightProj);


	return S_OK;
}

HRESULT CLevel_Stage1::Ready_UI()
{
    m_pFade = static_cast<CFade*>(m_pGameInstance->Find_Clone_UIObj(L"Fade"));
    Safe_AddRef(m_pFade);

    m_pGameInstance->PlayBGM(&m_pChannel, L"ST_Ambient_Canyon.ogg", 0.5f);

    m_pGameInstance->Set_OpenUI(true,TEXT("PlayerHP"));
    m_pGameInstance->Set_OpenUI(true, TEXT("WeaPon_Aim"));
    m_pGameInstance->Set_OpenUI(true, TEXT("WeaponUI"));

    m_pGameInstance->Set_OpenUI(false, TEXT("bg1"));
    m_pGameInstance->Set_OpenUI(false, TEXT("bg2"));
    m_pGameInstance->Set_OpenUI(false, TEXT("bg3"));
    m_pGameInstance->Set_OpenUI(false, TEXT("Floor"));
    m_pGameInstance->Set_OpenUI(false, TEXT("bus"));
    m_pGameInstance->Set_OpenUI(false, TEXT("Smoke1"));
    m_pGameInstance->Set_OpenUI(false, TEXT("Smoke2"));
    m_pGameInstance->Set_OpenUI(false, TEXT("cactus"));
    m_pGameInstance->Set_OpenUI(false, TEXT("LoadingBar"));
	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Player()
{
    m_pGameInstance->Set_Player(TEXT("Prototype_GameObject_Player"));
	m_pGameInstance->Get_Player()->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(25.f, 2.f, -12.f, 1.f));
	m_pGameInstance->Get_Player()->Get_Transform()->Rotation(0.f, 0.4f, 0.f);
    m_pGameInstance->MouseFix(true);
	return S_OK;
}
HRESULT CLevel_Stage1::Load_to_Next_Map_terrain(const _uint& iLevelIndex, const _wstring& strLayerTag,
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
HRESULT CLevel_Stage1::Load_to_Next_Map_Trigger(const _uint& iLevelIndex, const _wstring& strLayerTag, const _wstring& strProto,
                                                const _tchar* strProtoMapPath, void* Arg)
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
    m_pGameInstance->StopAll();
    Safe_Release(m_pFade);
}
