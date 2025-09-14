#include "stdafx.h"
#include "Level_StageBoss.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Player_HpUI.h"
#include "Camera_Free.h"
#include "SceneCamera.h"
#include "Player.h"
#include "Fade.h"
#include "WeaponUI.h"
#include "Terrain.h"
CLevel_StageBoss::CLevel_StageBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_StageBoss::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	 	return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Find_cell()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
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

HRESULT CLevel_StageBoss::Ready_Layer_Monster(const _wstring& pLayerTag)
{
    CActor::Actor_DESC Desc{};
    if (FAILED(Load_to_Next_Map_Monster(LEVEL_BOSS, pLayerTag, L"Prototype_GameObject_BillyBoom",
                                        L"Proto Component Boss_Monster", L"../Bin/Data/Monster/BossStage_Monster.dat",&Desc)))
        return E_FAIL;
	return S_OK;
}

HRESULT CLevel_StageBoss::Ready_Layer_Camera(const _wstring& pLayerTag)
{
	static_cast<CCamera_Free*>(m_pGameInstance->Find_CloneGameObject(LEVEL_STATIC,pLayerTag, GAMEOBJ_TYPE::CAMERA))->Set_Update(false);

	CGameObject* pMonster = m_pGameInstance->Find_CloneGameObject(LEVEL_BOSS, TEXT("Layer_Monster"), GAMEOBJ_TYPE::ACTOR);

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

HRESULT CLevel_StageBoss::Ready_Layer_Map(const _wstring& pLayerTag)
{
    CGameObject::GAMEOBJ_DESC Desc{};
    CTerrain::CTerrain_DESC TDESC{};
    if (FAILED(Load_to_Next_Map_terrain(LEVEL_BOSS, pLayerTag, L"Proto GameObject_Terrain",
                                        L"../Bin/Data/Map/SetMap_Boss_terrain.dat", &TDESC)))
        return E_FAIL;

    if (FAILED(Load_to_Next_Map_NonaniObj(LEVEL_BOSS, pLayerTag, L"Prototype GameObject_NonAniObj",
                                          L"../Bin/Data/Map/SetMap_Boss_Nonani.dat", &Desc)))
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
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;
        _float3 minBound = _float3(-100.f, 0.f, 0.f);
        _float3 maxBound = _float3(200.f, 100.f, 250.f); // 높이 100은 예시

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
            _vector corner = XMLoadFloat3(&corners[i]);
            _vector cornerVS = XMVector3TransformCoord(corner, LightView);

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

HRESULT CLevel_StageBoss::Ready_UI()
{
    m_pGameInstance->PlayBGM(&m_pChannel, L"ST_Ambient_EnergyCenter.ogg", 0.5f);
    return S_OK;
}

HRESULT CLevel_StageBoss::Ready_Layer_Player()
{
	m_pGameInstance->Get_Player()->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(50.f, 0.f, 15.f, 1.f));
	return S_OK;
}

HRESULT CLevel_StageBoss::Load_to_Next_Map_terrain(const _uint& iLevelIndex, const _wstring& strLayerTag,
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
    m_pGameInstance->StopAll();
}
