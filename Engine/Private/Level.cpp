#include "Level.h"
#include "GameInstance.h"
#include "Actor.h"
CLevel::CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Update(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

HRESULT CLevel::Load_to_Next_Map_NonaniObj(const _uint& iLevelIndex, const _wstring& strLayerTag,
                                           const _wstring& strProto,
                                            const _tchar* strProtoMapPath, void* Arg)
{
    map<const _wstring, vector<_matrix>> modelNameCount;

    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    _matrix World = XMMatrixIdentity();
    _uint Type{0};

    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(World), sizeof(_matrix), &dwByte, nullptr);
     
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
        modelNameCount[pModel].emplace_back(World);

        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);

    CGameObject::GAMEOBJ_DESC* pDesc = static_cast<CGameObject::GAMEOBJ_DESC*>(Arg);

    for (const auto& pair : modelNameCount)
    {
        const _wstring& modelName = pair.first;
        const vector<_matrix>& transforms = pair.second;

        CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(strProto, pDesc);

        if (pGameObject != nullptr)
        {
            pGameObject->Set_Model(modelName, iLevelIndex); // 모델을 설정한다.
            pGameObject->Set_InstaceBuffer(transforms);     // 인스턴싱 버퍼를 설정한다.
            m_pGameInstance->Add_Clon_to_Layers(iLevelIndex, strLayerTag,
                                                pGameObject); // 해당 레벨에 레이어를 추가한다.
        }
    }

    return S_OK;
}

HRESULT CLevel::Load_to_Next_Map_Wall(const _uint& iLevelIndex, const _wstring& strLayerTag, const _wstring& strProto,
                                              const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    _matrix World = XMMatrixIdentity();
    _uint Type{0};

    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(World), sizeof(_matrix), &dwByte, nullptr);

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

        CGameObject::GAMEOBJ_DESC* pDesc = static_cast<CGameObject::GAMEOBJ_DESC*>(Arg);
        pDesc->WorldMatrix = World;
        pDesc->ProtoName = pModel;
        pDesc->CuriLevelIndex = iLevelIndex;
   
        m_pGameInstance->Add_GameObject_To_Layer(iLevelIndex, strLayerTag, strProto, pDesc);

        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel::Load_to_Next_Map_AniOBj(const _uint& iLevelIndex, const _wstring& strLayerTag, const _wstring& strProto,
                                       const _tchar* strProtoMapPath,
                                                void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    _matrix World = XMMatrixIdentity();
    _uint Type = {0};
    _uint WType = {};
    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(World), sizeof(_matrix), &dwByte, nullptr);
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
        CGameObject::GAMEOBJ_DESC* pDesc = static_cast<CGameObject::GAMEOBJ_DESC*>(Arg);
        pDesc->WorldMatrix = World;

        if (pPoroto == strProto)
        {
            pDesc->ProtoName = pModel;
            pDesc->CuriLevelIndex = iLevelIndex;
            pDesc->Object_Type = WType;
            m_pGameInstance->Add_GameObject_To_Layer(iLevelIndex, strLayerTag, strProto, pDesc);
        }
        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel::Load_to_Next_Map_Monster(const _uint& iLevelIndex, const _wstring& strLayerTag,
                                         const _wstring& strProto,
                                         const _wstring& strProtoComponet,
                                                 const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    _matrix World = XMMatrixIdentity();
    _uint Type = {0};

    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(World), sizeof(_matrix), &dwByte, nullptr);
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
 
        CActor::Actor_DESC* Desc = static_cast<CActor::Actor_DESC*>(Arg);
        Desc->WorldMatrix = World;
     
        if (pModel== strProtoComponet)
        {
            m_pGameInstance->Add_GameObject_To_Layer(iLevelIndex, strLayerTag, strProto, Desc);
        }  
        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);
    return S_OK;
}

HRESULT CLevel::Load_to_Next_Map_NPC(const _uint& iLevelIndex, const _wstring& strLayerTag, const _wstring& strProto,
                                   const _tchar* strProtoMapPath, void* Arg)
{
    HANDLE hFile = CreateFile(strProtoMapPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return E_FAIL;
    }

    DWORD dwByte(0);
    _matrix World = XMMatrixIdentity();
    _uint Type = {0};

    while (true)
    {
        _bool bFile(false);

        bFile = ReadFile(hFile, &(World), sizeof(_matrix), &dwByte, nullptr);
    
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
       
        CActor::Actor_DESC* Desc = static_cast<CActor::Actor_DESC*>(Arg);
        Desc->WorldMatrix = World;
        m_pGameInstance->Add_GameObject_To_Layer(iLevelIndex, strLayerTag, strProto, Desc);
     
        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);

    return S_OK;
}
void CLevel::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
