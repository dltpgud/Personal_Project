#include "Level_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Level.h"
#include "Actor.h"
CLevel_Manager::CLevel_Manager() : m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Open_Level(_uint iCurrentLevelID, CLevel* pNewLevel)
{
    if (nullptr == pNewLevel)
        return E_FAIL;

    /* 최초레벨할당할때 지우면 큰일난다. m_iCurrentLevelID : 의도치 않은 값으로 초기화되어있기때문에 의도치않은
     * 레벨자원을 지우잖어. */
    if (nullptr != m_pCurrentLevel)
         m_pGameInstance->Clear(m_iCurrentLevelID);

    Safe_Release(m_pCurrentLevel);

    m_pCurrentLevel = pNewLevel;

    m_iCurrentLevelID = iCurrentLevelID;

    return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
    m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
    return m_pCurrentLevel->Render();
}

HRESULT CLevel_Manager::Load_to_Next_Map_terrain(const _uint& iLevelIndex, const _uint& strLayerTag, CGameObject* ProtoObj,
                                                 const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    DWORD dwStrByte(0);
    _vector Right = {0.f, 0.f, 0.f, 0.f};
    _vector UP = {0.f, 0.f, 0.f, 0.f};
    _vector LOOK = {0.f, 0.f, 0.f, 0.f};
    _vector POSITION = {0.f, 0.f, 0.f, 0.f};
    _uint Type{0};
    _uint TileX{0};
    _uint TileY{0};
    _bool bMainTile = true;
    _uint bFire{ 0 };
    _float FireOffset{ 0.f };
    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(Right), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(UP), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(LOOK), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(POSITION), sizeof(_vector), &dwByte, nullptr);
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



        bFile = ReadFile(hFile, &(bFire), sizeof(_uint), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(FireOffset), sizeof(_float), &dwByte, nullptr);

        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
            break;
        }

        CGameObject* pGameObject{};

        if (nullptr == Arg)
        {
            CGameObject::GAMEOBJ_DESC DESC{};

            if (true == bMainTile)
            {
                DESC.DATA_TYPE = 0;
            }else
                DESC.DATA_TYPE = 1;
            pGameObject = ProtoObj->Clone(&DESC);
        }
        else
        pGameObject = ProtoObj->Clone(Arg);

        pGameObject->Set_Scalra_uint(bFire);
        pGameObject->Set_Model(pModel, iLevelIndex);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);
        pGameObject->Set_Scalra_float(FireOffset);


        pGameObject->Set_Buffer(TileX, TileY);
        bMainTile = false;
        
        m_pGameInstance->Add_Clon_to_Layers(iLevelIndex, strLayerTag, pGameObject);

         Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_Manager::Load_to_Next_Map_NonaniObj(const _uint& iLevelIndex, const _uint& strLayerTag, CGameObject* ProtoObj,
                                                   const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    DWORD dwStrByte(0);
    _vector Right = {0.f, 0.f, 0.f, 0.f};
    _vector UP = {0.f, 0.f, 0.f, 0.f};
    _vector LOOK = {0.f, 0.f, 0.f, 0.f};
    _vector POSITION = {0.f, 0.f, 0.f, 0.f};
    _uint Type{0};

    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(Right), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(UP), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(LOOK), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(POSITION), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(Type), sizeof(_uint), &dwByte, nullptr);

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
        pPoroto[Length] = L'\0';
        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
            break;
        }

        CGameObject* pGameObject = ProtoObj->Clone(Arg);

        pGameObject->Set_Model(pModel, iLevelIndex);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);
        m_pGameInstance->Add_Clon_to_Layers(iLevelIndex, strLayerTag, pGameObject);

        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_Manager::Load_to_Next_Map_Wall(const _uint& iLevelIndex, const _uint& strLayerTag, CGameObject* ProtoObj,
                                              const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    DWORD dwStrByte(0);
    _vector Right = {0.f, 0.f, 0.f, 0.f};
    _vector UP = {0.f, 0.f, 0.f, 0.f};
    _vector LOOK = {0.f, 0.f, 0.f, 0.f};
    _vector POSITION = {0.f, 0.f, 0.f, 0.f};
    _uint Type{0};

    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(Right), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(UP), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(LOOK), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(POSITION), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(Type), sizeof(_uint), &dwByte, nullptr);

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
        pPoroto[Length] = L'\0';
        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
            break;
        }

        CGameObject* pGameObject = ProtoObj->Clone(Arg);

     pGameObject->Set_Model(pModel, iLevelIndex);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);

        m_pGameInstance->Add_Clon_to_Layers(iLevelIndex, strLayerTag, pGameObject);

        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_Manager::Load_to_Next_Map_AniOBj(const _uint& iLevelIndex, const _uint& strLayerTag, CGameObject* ProtoObj, const _uint& type, const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    DWORD dwStrByte(0);
    _vector Right = { 0.f, 0.f, 0.f, 0.f };
    _vector UP = { 0.f, 0.f, 0.f, 0.f };
    _vector LOOK = { 0.f, 0.f, 0.f, 0.f };
    _vector POSITION = { 0.f, 0.f, 0.f, 0.f };
    _uint Type = { 0 };
    _uint WType = {};
    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(Right), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(UP), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(LOOK), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(POSITION), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(Type), sizeof(_uint), &dwByte, nullptr);

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
        pPoroto[Length] = L'\0';

        bFile = ReadFile(hFile, &(WType), sizeof(_uint), &dwByte, nullptr);

        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
            break;
        }

        if (type == Type) {
            CGameObject* pGameObject = ProtoObj->Clone(Arg);

            pGameObject->Set_Model(pModel, iLevelIndex);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);
            pGameObject->Set_Buffer(0, WType);
           
            m_pGameInstance->Add_Clon_to_Layers(iLevelIndex, strLayerTag, pGameObject);
        }
        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_Manager::Load_to_Next_Map_Monster(const _uint& iLevelIndex, const _uint& strLayerTag, CGameObject* ProtoObj, const _uint& ProtoTag, const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    DWORD dwStrByte(0);
    _vector Right = { 0.f, 0.f, 0.f, 0.f };
    _vector UP = { 0.f, 0.f, 0.f, 0.f };
    _vector LOOK = { 0.f, 0.f, 0.f, 0.f };
    _vector POSITION = { 0.f, 0.f, 0.f, 0.f };
    _uint Type = { 0 };

    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(Right), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(UP), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(LOOK), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(POSITION), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(Type), sizeof(_uint), &dwByte, nullptr);

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
        pPoroto[Length] = L'\0';

  

        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
            break;
        }
        CGameObject* pGameObject{}; 

        switch (ProtoTag)
        {
        case CActor::TYPE_BOOM_BOT:
            if (false == lstrcmpW(pModel, L"Proto Component BoomBot_Monster")) {
                pGameObject = ProtoObj->Clone(Arg);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);
            }
            break;

        case CActor::TYPE_GUN_PAWN:
            if (false == lstrcmpW(pModel, L"Proto Component GunPawn_Monster")) {
                pGameObject = ProtoObj->Clone(Arg);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);
            }
            break;


        case CActor::TYPE_JET_FLY:
            if (false == lstrcmpW(pModel, L"Proto Component JetFly_Monster")) {
                pGameObject = ProtoObj->Clone(Arg);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);
            }
            break;

        case CActor::TYPE_MECANOBOT:
            if (false == lstrcmpW(pModel, L"Proto Component MecanoBot_Monster")) {
                pGameObject = ProtoObj->Clone(Arg);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);
            }
            break;

        case CActor::TYPE_BOSS:
            if (false == lstrcmpW(pModel, L"Proto Component Boss_Monster")) {
                pGameObject = ProtoObj->Clone(Arg);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);
            }
            break;
        }

        m_pGameInstance->Add_Clon_to_Layers(iLevelIndex, strLayerTag, pGameObject);
        m_pGameInstance->Add_Monster(iLevelIndex, m_pGameInstance->Recent_GameObject(iLevelIndex, strLayerTag));
       
      
        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);
    return S_OK;
}

HRESULT CLevel_Manager::Load_to_Next_Map_NPC(const _uint& iLevelIndex, const _uint& strLayerTag, CGameObject* ProtoObj, const _uint& ProtoTag, const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    DWORD dwStrByte(0);
    _vector Right = { 0.f, 0.f, 0.f, 0.f };
    _vector UP = { 0.f, 0.f, 0.f, 0.f };
    _vector LOOK = { 0.f, 0.f, 0.f, 0.f };
    _vector POSITION = { 0.f, 0.f, 0.f, 0.f };
    _uint Type = { 0 };

    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(Right), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(UP), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(LOOK), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(POSITION), sizeof(_vector), &dwByte, nullptr);
        bFile = ReadFile(hFile, &(Type), sizeof(_uint), &dwByte, nullptr);

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
        pPoroto[Length] = L'\0';



        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
            break;
        }
        CGameObject* pGameObject{};

        switch (ProtoTag)
        {
        case CActor::TYPE_HEALTH_BOT:
            if (false == lstrcmpW(pModel, L"Proto Component HealthBot_NPC")) {
                pGameObject = ProtoObj->Clone(Arg);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
                pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);
            }
            break;

        }

        m_pGameInstance->Add_Clon_to_Layers(iLevelIndex, strLayerTag, pGameObject);
       


        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);


    return S_OK;
}

_uint CLevel_Manager::Get_iCurrentLevel()
{
    return m_iCurrentLevelID;
}

void CLevel_Manager::Set_Open_Bool(_bool NextStage)
{
    OpenLevel = NextStage;
}

CLevel_Manager* CLevel_Manager::Create()
{
    return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
    __super::Free();
    Safe_Release(m_pCurrentLevel);
    Safe_Release(m_pGameInstance);
}
