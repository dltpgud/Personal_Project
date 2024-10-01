#include "stdafx.h"
#include "Level_Edit.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera_Tool.h"
#include "Terrain.h"
#include "VIBuffer_Terrain.h"
CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel{pDevice, pContext}
{
    ZeroMemory(&m_iBufferCount, sizeof(_int) * 2);
    ZeroMemory(&m_fposition, sizeof(_float) * 3);
    ZeroMemory(&m_fscale, sizeof(_float) * 3);
    ZeroMemory(&m_iItem_selected_idx, sizeof(_int) * 4);
    ZeroMemory(&m_iIcomtem_selected_idx, sizeof(_int) * 4);
}

HRESULT CLevel_Edit::Initialize()
{
    m_iMapObj_Type = 0;

    m_bshow_win_Terrian = true;
    m_fspped = 7.f;
    m_fRotfspped = 1.f;
    m_tFPath[0] = L"../Bin/Data/Map/SetMap_Stage1_terrain.dat";
    m_tFPath[1] = L"../Bin/Data/Map/SetMap_Stage1_Wall.dat";
    m_tFPath[2] = L"../Bin/Data/Map/SetMap_Stage1_Nonani.dat";
    m_tFPath[3] = L"../Bin/Data/Map/SetMap_Stage1_ani.dat";
    for (_uint i = 0; i < 3; i++) { m_fscale[i] = 1; }

    if (FAILED(Ready_ToolCamera(CGameObject::CAMERA)))
        return E_FAIL;

    if (FAILED(Ready_Light()))
        return E_FAIL;

    Push_Proto_vec();
    Push_ProtoCom_vec();

    return S_OK;
}

void CLevel_Edit::Update(_float fTimeDelta)
{
    if (false == m_pGameInstance->IsGameObject(LEVEL_EDIT, CGameObject::CAMERA))
    {
        if (FAILED(Ready_ToolCamera(CGameObject::CAMERA)))
            return;
    }

    Tool();

    Msg_collection();

    Key_input(fTimeDelta);

    __super::Update(fTimeDelta);
}

HRESULT CLevel_Edit::Render()
{
    SetWindowText(g_hWnd, TEXT("편집 레벨입니다."));
    __super::Render();

    return S_OK;
}

void CLevel_Edit::Tool()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);      // ui위치
    ImGui::SetNextWindowSize(ImVec2(215, 117), ImGuiCond_Always); // ui 크기
    ImGui::Begin("My Tool");

    ImGui::Text("Secne");
    ImGui::SameLine(0.f, 5.f);
    _bool Scene_Type = false;
    const char* Scene[] = {"LEVEL_STAGE1", "LEVEL_STAGE2", "LEVEL_BOSS"}; // 콤보 상자
    Scene_Type = ImGui::Combo("##0", &m_iScene_Type, Scene, IM_ARRAYSIZE(Scene));

    if (Scene_Type)
    {
        if (m_iScene_Type == 0)
        {
            m_iScene = 3;
            m_tFPath[0] = L"../Bin/Data/Map/SetMap_Stage1_terrain.dat";
            m_tFPath[1] = L"../Bin/Data/Map/SetMap_Stage1_Wall.dat";
            m_tFPath[2] = L"../Bin/Data/Map/SetMap_Stage1_Nonani.dat";
            m_tFPath[3] = L"../Bin/Data/Map/SetMap_Stage1_ani.dat";
        }

        if (m_iScene_Type == 1)
        {
            m_iScene = 4;
            m_tFPath[0] = L"../Bin/Data/Map/SetMap_Stage2_terrain.dat";
            m_tFPath[1] = L"../Bin/Data/Map/SetMap_Stage2_Wall.dat";
            m_tFPath[2] = L"../Bin/Data/Map/SetMap_Stage2_Nonani.dat";
            m_tFPath[3] = L"../Bin/Data/Map/SetMap_Stage2_ani.dat";
        }
        if (m_iScene_Type == 2)
        {
            m_iScene = 5;
            m_tFPath[0] = L"../Bin/Data/Map/SetMap_Boss_terrain.dat";
            m_tFPath[1] = L"../Bin/Data/Map/SetMap_Boss_Wall.dat";
            m_tFPath[2] = L"../Bin/Data/Map/SetMap_Boss_Nonani.dat";
            m_tFPath[3] = L"../Bin/Data/Map/SetMap_Boss_ani.dat";
        }
    }

    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Checkbox("Map tool", &m_bshow_Map_window);
    ImGui::Checkbox("Another", &m_bshow_another_window);
    ImGui::End();

    if (m_bshow_Map_window == true)
    {
        Map();
    }

    if (m_bshow_another_window == true)
    {
        Another();
    }
}

void CLevel_Edit::Map()
{
    ImGui::Begin("Map Tool", &m_bshow_Map_window, ImGuiWindowFlags_MenuBar);
    Menu_File();
    ImGui::Separator();
    ComboType();
    if (true == m_bshow_win_Terrian)
    {
        Terrain_ListBox();
    }
    if (true == m_bshow_win_Wall)
    {
        Wall_ListBox();
    }
    if (true == m_bshow_win_NONaniObj)
    {
        MapNONANIObj_ListBox();
    }
    if (true == m_bshow_win_aniObj)
    {
        MapANIObj_ListBox();
    }
    ImGui::Separator();
    Set_pos();
    ImGui::Separator();
    Set_scale();
    ImGui::Separator();
    Set_Speed();
    ImGui::Separator();

    ImGui::End();
}

void CLevel_Edit::Another()
{
    ImGui::Begin("Another Tool");

    ImGui::End();
}

void CLevel_Edit::Menu_File()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {

            if (ImGui::MenuItem("Save"))
            {
                m_bshow_Save_MessageBox = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Load"))
            {
                m_bshow_Load_MessageBox = true;
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void CLevel_Edit::ComboType()
{
    ImGui::Text("MapObj_Type");
    ImGui::SameLine(50.f, 50.f);
    _bool MapObj_Type = false;
    const char* Obj_Type[] = {"Terrein", "Wall", "NonAniObj", "AniObj"}; // 콤보 상자
    MapObj_Type = ImGui::Combo("##0", &m_iMapObj_Type, Obj_Type, IM_ARRAYSIZE(Obj_Type));

    if (MapObj_Type)
    {
        if (m_iMapObj_Type == 0)
        {
            m_bshow_win_Terrian = true;
            m_bshow_win_Wall = false;
            m_bshow_win_NONaniObj = false;
            m_bshow_win_aniObj = false;
        }

        if (m_iMapObj_Type == 1)
        {
            m_bshow_win_Terrian = false;
            m_bshow_win_Wall = true;
            m_bshow_win_NONaniObj = false;
            m_bshow_win_aniObj = false;
        }
        if (m_iMapObj_Type == 2)
        {
            m_bshow_win_Terrian = false;
            m_bshow_win_Wall = false;
            m_bshow_win_NONaniObj = true;
            m_bshow_win_aniObj = false;
        }
        if (m_iMapObj_Type == 3)
        {
            m_bshow_win_Terrian = false;
            m_bshow_win_Wall = false;
            m_bshow_win_NONaniObj = false;
            m_bshow_win_aniObj = true;
        }
    }
}

void CLevel_Edit::MapNONANIObj_ListBox()
{
    if (ImGui::BeginListBox("##listobj"))
    {
        for (_int i = 0; i < m_protokey[NONANIMAPOBJ].size(); i++)
        {
            const _bool is_selected = (m_iItem_selected_idx[2] == i);
            if (ImGui::Selectable(m_protokey[NONANIMAPOBJ][i], is_selected))
            {
                m_iItem_selected_idx[2] = i;
            }
        }
        ImGui::EndListBox();
    }

    if (ImGui::BeginListBox("##listobjcom"))
    {
        for (_int i = 0; i < m_protoComkey[NONANIMAPOBJ].size(); i++)
        {
            const _bool is_selected = (m_iIcomtem_selected_idx[2] == i);
            if (ImGui::Selectable(m_protoComkey[NONANIMAPOBJ][i], is_selected))
            {
                m_iIcomtem_selected_idx[2] = i;
            }
        }
        ImGui::EndListBox();
    }

    Create_Leyer_Botton(NONANIMAPOBJ, m_iItem_selected_idx[2], m_iIcomtem_selected_idx[2]);
}

void CLevel_Edit::MapANIObj_ListBox()
{
    if (ImGui::BeginListBox("##listaobj"))
    {
        for (_int i = 0; i < m_protokey[ANIMAPOBJ].size(); i++)
        {
            const _bool is_selected = (m_iItem_selected_idx[3] == i);
            if (ImGui::Selectable(m_protokey[ANIMAPOBJ][i], is_selected))
            {
                m_iItem_selected_idx[3] = i;
            }
        }
        ImGui::EndListBox();
    }

    if (ImGui::BeginListBox("##listobjacom"))
    {
        for (_int i = 0; i < m_protoComkey[ANIMAPOBJ].size(); i++)
        {
            const _bool is_selected = (m_iIcomtem_selected_idx[3] == i);
            if (ImGui::Selectable(m_protoComkey[ANIMAPOBJ][i], is_selected))
            {
                m_iIcomtem_selected_idx[3] = i;
            }
        }
        ImGui::EndListBox();
    }

    Create_Leyer_Botton(ANIMAPOBJ, m_iItem_selected_idx[3], m_iIcomtem_selected_idx[3]);
}
void CLevel_Edit::Terrain_ListBox()
{
    if (ImGui::BeginListBox("##listTerrain"))
    {
        for (_int i = 0; i < m_protokey[TERRAIN].size(); i++)
        {
            const _bool is_selected = (m_iItem_selected_idx[0] == i);
            if (ImGui::Selectable(m_protokey[TERRAIN][i], is_selected))
            {
                m_iItem_selected_idx[0] = i;
            }
        }
        ImGui::EndListBox();
    }

    if (ImGui::BeginListBox("##listTerraincom"))
    {
        for (_int i = 0; i < m_protoComkey[TERRAIN].size(); i++)
        {
            const _bool is_selected = (m_iIcomtem_selected_idx[0] == i);
            if (ImGui::Selectable(m_protoComkey[TERRAIN][i], is_selected))
            {
                m_iIcomtem_selected_idx[0] = i;
            }
        }
        ImGui::EndListBox();
    }

    Set_Tile();

    Create_Leyer_Botton(TERRAIN, m_iItem_selected_idx[0], m_iIcomtem_selected_idx[0]);
}

void CLevel_Edit::Wall_ListBox()
{
    if (ImGui::BeginListBox("##listWall"))
    {
        for (_int i = 0; i < m_protokey[WALL].size(); i++)
        {
            const _bool is_selected = (m_iItem_selected_idx[1] == i);
            if (ImGui::Selectable(m_protokey[WALL][i], is_selected))
            {
                m_iItem_selected_idx[1] = i;
            }
        }
        ImGui::EndListBox();
    }

    if (ImGui::BeginListBox("##listWallcom"))
    {
        for (_int i = 0; i < m_protoComkey[WALL].size(); i++)
        {
            const _bool is_selected = (m_iIcomtem_selected_idx[1] == i);
            if (ImGui::Selectable(m_protoComkey[WALL][i], is_selected))
            {
                m_iIcomtem_selected_idx[1] = i;
            }
        }
        ImGui::EndListBox();
    }

    Create_Leyer_Botton(WALL, m_iItem_selected_idx[1], m_iIcomtem_selected_idx[1]);
}

HRESULT CLevel_Edit::Create_Layer_Obj(POROTO_TYPE type, const _uint& pLayerTag, _int Iindex, _uint Comindex)
{
    if (false == m_pGameInstance->IsGameObject(LEVEL_EDIT, CGameObject::CAMERA))
    {
        if (FAILED(Ready_ToolCamera(CGameObject::CAMERA)))
            return E_FAIL;
    }
    _tchar* protKey{}; // wstring을 _char로 변형하고 보관할 변수

    _char* key = m_protokey[type][Iindex];

    _int iLength = (_int)(strlen(key) + 1);

    protKey = new _tchar[iLength];
    ZeroMemory(protKey, sizeof(_tchar) * iLength);

    MultiByteToWideChar(CP_UTF8, 0, key, iLength, protKey, iLength);

    _tchar* protcomKey{}; // wstring을 _char로 변형하고 보관할 변수

    _char* comkey = m_protoComkey[type][Comindex];

    _int icomLength = (_int)(strlen(comkey) + 1);

    protcomKey = new _tchar[icomLength];
    ZeroMemory(protcomKey, sizeof(_tchar) * icomLength);

    MultiByteToWideChar(CP_UTF8, 0, comkey, icomLength, protcomKey, icomLength);

    m_pType = type;
    CGameObject::GAMEOBJ_DESC pDec;
    pDec.fSpeedPerSec = m_fspped;
    pDec.fRotationPerSec = m_fRotfspped;
    pDec.ProtoName = protKey;

    switch (m_pType)
    {
    case Tool::CLevel_Edit::TERRAIN:
    {
        if (1 > m_iBufferCount[0] || 1 > m_iBufferCount[1])
            return E_FAIL;

        pDec.DATA_TYPE = CGameObject::DATA_TERRAIN;
        CGameObject* pTerrain = m_pGameInstance->Clone_GameObject(protKey, &pDec);

        pTerrain->Set_Model(protcomKey);

        pTerrain->Set_Buffer(m_iBufferCount[0], m_iBufferCount[1]);

        m_pTerrainVT = static_cast<CTerrain*>(pTerrain)->Get_buffer();

        m_pGameInstance->Add_Clon_to_Layers(LEVEL_EDIT, pLayerTag, pTerrain);

        m_pTerrainTR = m_pGameInstance->Recent_GameObject(CGameObject::MAP)->Get_Transform();
        Safe_Delete(protKey);

        m_TerrainVT.push_back(m_pTerrainTR);
    }
    break;
    case Tool::CLevel_Edit::WALL:
    {
        pDec.DATA_TYPE = CGameObject::DATA_WALL;
        CGameObject* pObj = m_pGameInstance->Clone_GameObject(protKey, &pDec);
        pObj->Set_Model(protcomKey);
        m_pGameInstance->Add_Clon_to_Layers(LEVEL_EDIT, pLayerTag, pObj);
    }
    break;
    case Tool::CLevel_Edit::NONANIMAPOBJ:
    {
        pDec.DATA_TYPE = CGameObject::DATA_NONANIMAPOBJ;
        CGameObject* nObj = m_pGameInstance->Clone_GameObject(protKey, &pDec);
        nObj->Set_Model(protcomKey);
        m_pGameInstance->Add_Clon_to_Layers(LEVEL_EDIT, pLayerTag, nObj);
    }
    break;
    case Tool::CLevel_Edit::ANIMAPOBJ:
    {
        if (false == lstrcmpW(protKey, L"Proto GameObject Door_aniObj"))
        {
            pDec.DATA_TYPE = CGameObject::DATA_DOOR;
            CGameObject* aObj = m_pGameInstance->Clone_GameObject(protKey, &pDec);
            aObj->Set_Model(protcomKey);
            m_pGameInstance->Add_Clon_to_Layers(LEVEL_EDIT, pLayerTag, aObj);
        }
        if (false == lstrcmpW(protKey, L"Proto GameObject Chest_aniObj"))
        {
            pDec.DATA_TYPE = CGameObject::DATA_CHEST;
            CGameObject* aObj = m_pGameInstance->Clone_GameObject(protKey, &pDec);
            aObj->Set_Model(protcomKey);
            m_pGameInstance->Add_Clon_to_Layers(LEVEL_EDIT, pLayerTag, aObj);
        }
    }
    break;
    }

    m_pObjTransform = m_pGameInstance->Recent_GameObject(CGameObject::MAP)->Get_Transform();
    Safe_Delete_Array(protKey);
    Safe_Delete_Array(protcomKey);
    return S_OK;
}

void CLevel_Edit::Push_Proto_vec()
{
    for (auto iter : m_pGameInstance->Get_ProtoObject_map())
    {
        const _wstring key = iter.first;

        _int iLength = (_int)(key.length() + 1);

        _char* protKey = new _char[iLength];
        ZeroMemory(protKey, sizeof(_char) * iLength);

        WideCharToMultiByte(CP_ACP, 0, key.c_str(), iLength, protKey, iLength, nullptr, nullptr);

        _char* restore_key = new _char[256]; // 잘라 둔키 복구
        // 문자열 뒷자리 찾기
        _char* token = NULL;
        _char* next_token = NULL;

        token = strtok_s(protKey, "_", &next_token);

        if (token != NULL)
        {
            token = strtok_s(NULL, "_", &next_token);
        }

        if (false == lstrcmpA(token, "CameraTool"))
        {
            Safe_Delete(protKey);
            Safe_Delete(restore_key);
            continue;
        }
        if (false == lstrcmpA(token, "Terrain"))
        {
            strcpy_s(restore_key, 256, protKey);
            strcat_s(restore_key, 256, "_");
            strcat_s(restore_key, 256, token);

            m_protokey[TERRAIN].push_back(restore_key);
        }
        if (false == lstrcmpA(token, "Wall"))
        {
            strcpy_s(restore_key, 256, protKey);
            strcat_s(restore_key, 256, "_");
            strcat_s(restore_key, 256, token);

            m_protokey[WALL].push_back(restore_key);
        }
        else if (false == lstrcmpA(token, "nonaniObj"))
        {
            strcpy_s(restore_key, 256, protKey);
            strcat_s(restore_key, 256, "_");
            strcat_s(restore_key, 256, token);

            m_protokey[NONANIMAPOBJ].push_back(restore_key);
        }
        else if (false == lstrcmpA(token, "aniObj"))
        {
            strcpy_s(restore_key, 256, protKey);
            strcat_s(restore_key, 256, "_");
            strcat_s(restore_key, 256, token);

            m_protokey[ANIMAPOBJ].push_back(restore_key);
        }

        Safe_Delete(protKey);
    }
}

void CLevel_Edit::Push_ProtoCom_vec()
{
    for (auto iter : m_pGameInstance->Get_Com_proto_vec(LEVEL_STATIC))
    {
        const _wstring key = iter.first;

        _int iLength = (_int)(key.length() + 1);

        _char* protKey = new _char[iLength];
        ZeroMemory(protKey, sizeof(_char) * iLength);

        WideCharToMultiByte(CP_ACP, 0, key.c_str(), iLength, protKey, iLength, nullptr, nullptr);

        _char* restore_key = new _char[256]; // 잘라 둔키 복구
        // 문자열 뒷자리 찾기
        _char* token = NULL;
        _char* next_token = NULL;

        token = strtok_s(protKey, "_", &next_token);

        if (token != NULL)
        {
            token = strtok_s(NULL, "_", &next_token);
        }

        if (false == lstrcmpA(token, "Terrain"))
        {

            strcpy_s(restore_key, 256, protKey);
            strcat_s(restore_key, 256, "_");
            strcat_s(restore_key, 256, token);

            m_protoComkey[TERRAIN].push_back(restore_key);
        }
        else if (false == lstrcmpA(token, "Wall"))
        {

            strcpy_s(restore_key, 256, protKey);
            strcat_s(restore_key, 256, "_");
            strcat_s(restore_key, 256, token);

            m_protoComkey[WALL].push_back(restore_key);
        }
        else if (false == lstrcmpA(token, "nonaniObj"))
        {
            strcpy_s(restore_key, 256, protKey);
            strcat_s(restore_key, 256, "_");
            strcat_s(restore_key, 256, token);

            m_protoComkey[NONANIMAPOBJ].push_back(restore_key);
        }
        else if (false == lstrcmpA(token, "aniObj"))
        {
            strcpy_s(restore_key, 256, protKey);
            strcat_s(restore_key, 256, "_");
            strcat_s(restore_key, 256, token);

            m_protoComkey[ANIMAPOBJ].push_back(restore_key);
        }
        else
        {
            Safe_Delete(restore_key);
        }
        Safe_Delete(protKey);
    }
}

void CLevel_Edit::Create_Leyer_Botton(POROTO_TYPE type, _uint Objnum, _uint Comnum)
{
    _bool ret;
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.f, 0.5f, 0.f));       // 기본
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(1.f, 0.f, 0.f)); // 마웃스 올린색
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(1.f, 0.f, 1.f));  // 클릭
    ret = ImGui::Button("Create_MapObj");
    ImGui::PopStyleColor(3);

    if (ret)
    {
        if (type == TERRAIN)
        {
            if (1 >= m_iBufferCount[0] || 1 >= m_iBufferCount[1])
            {
                MSG_BOX("다 골랐어? 확실해?");
                return;
            }
        }

        m_CopyiItem_selected_idx = Objnum;
        m_CopyiIcomtem_selected_idx = Comnum;

        re_setting();

        Create_Layer_Obj(type, CGameObject::MAP, Objnum, Comnum);
    }
}

HRESULT CLevel_Edit::Ready_ToolCamera(const _uint& pLayerTag)
{
    CCamera_Tool::CAMERA_Tool_DESC Desc{};

    Desc.vEye = _float4(0.f, 10.f, -8.f, 1.f);
    Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.fFovy = XMConvertToRadians(60.0f);
    Desc.fNearZ = 0.1f;
    Desc.fFarZ = 500.f;
    Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    Desc.fSpeedPerSec = 20.f;
    Desc.fRotationPerSec = XMConvertToRadians(60.0f);
    Desc.fMouseSensor = 0.1f;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_EDIT, pLayerTag, TEXT("ProtoGameObject_CameraTool"),
                                                        nullptr, 0, &Desc)))
        return E_FAIL;

    return S_OK;
}

void CLevel_Edit::Set_Tile()
{
    _bool ret{};
    ImGui::Text("Buffer Count");
    ImGui::SameLine(100, 0);
    ImGui::InputInt2("##3", m_iBufferCount);
}

void CLevel_Edit::re_setting()
{

    ZeroMemory(&m_fposition, sizeof(_float) * 3);
    ZeroMemory(&m_fscale, sizeof(_float) * 3);
    for (_uint i = 0; i < 3; i++) { m_fscale[i] = 1; }
    m_fspped = 7.f;
    m_fRotfspped = 1.f;
}

HRESULT CLevel_Edit::Ready_Light()
{
    LIGHT_DESC LightDesc{};

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    return S_OK;
}

void CLevel_Edit::Set_pos()
{
    _bool ret;
    ImGui::Text("float_Pos");
    ImGui::SameLine(100, 0);
    ImGui::InputFloat3("##2", m_fposition, "%.2f");

    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.f, 0.5f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(1.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(1.f, 0.f, 1.f));
    ret = ImGui::Button("Set_Pos");
    ImGui::PopStyleColor(3);
    if (ret)
    {
        if (false == m_pGameInstance->IsGameObject(LEVEL_EDIT, CGameObject::MAP))
            return;

        _vector vPos = XMVectorSet(m_fposition[0], m_fposition[1], m_fposition[2], 1.f);
        m_pObjTransform->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, vPos);
    }
}

void CLevel_Edit::Set_scale()
{
    _bool ret;
    ImGui::Text("Set_Scale");
    ImGui::SameLine(100, 0);
    ImGui::InputFloat3("##29", m_fscale, "%.2f");

    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.f, 0.5f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(1.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(1.f, 0.f, 1.f));
    ret = ImGui::Button("Set_Scale");
    ImGui::PopStyleColor(3);
    if (ret)
    {
        if (false == m_pGameInstance->IsGameObject(LEVEL_EDIT, CGameObject::MAP))
            return;

        if (m_fscale[0] <= 0.01f || m_fscale[1] <= 0.01f || m_fscale[2] <= 0.01f)
        {
            MSG_BOX("너무작아");
            return;
        }

        m_pObjTransform->Set_Scaling(m_fscale[0], m_fscale[1], m_fscale[2]);
    }
}

void CLevel_Edit::Msg_collection()
{
    ImGui::SetNextWindowPos(ImVec2(g_iWinSizeX * 0.5f, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(200, 70), ImGuiCond_Always);

    Msg_Del_Box();
    Msg_Save_box();
    Msg_Load_box();
    Msg_ALL_Del_Box();
}

void CLevel_Edit::Msg_Del_Box()
{
    if (m_bshow_Del_MessageBox)
    {
        ImGui::Begin("Delete Box", &m_bshow_Del_MessageBox); // 제목이 "Message Box"인 창
        ImGui::Text("One Object Delete?");                   // 질문 텍스트

        // 버튼들: OK와 Cancel
        if (ImGui::Button("Delete OK"))
        {
            m_pGameInstance->Recent_GameObject(CGameObject::MAP)->Set_Dead(true);

            if (false == m_pGameInstance->IsGameObject(LEVEL_EDIT, CGameObject::MAP))
            {
                re_setting();
            }

            m_bshow_Del_MessageBox = false; // 메시지 상자 닫기
        }
        ImGui::SameLine(); // 같은 줄에 Cancel 버튼 배치
        if (ImGui::Button("Delete Cancel"))
        {
            m_bshow_Del_MessageBox = false; // 메시지 상자 닫기
        }
        ImGui::End();
    }
}

void CLevel_Edit::Msg_ALL_Del_Box()
{
    if (m_bshow_ALLDel_MessageBox)
    {
        ImGui::Begin("ALL Delete Box", &m_bshow_ALLDel_MessageBox); // 제목이 "Message Box"인 창
        ImGui::Text("ALLLLLLLLLL Object Delete?");                  // 질문 텍스트

        // 버튼들: OK와 Cancel
        if (ImGui::Button("Delete OK"))
        {
            m_pGameInstance->ObjClear(LEVEL_EDIT);

            

            if (false == m_pGameInstance->IsGameObject(LEVEL_EDIT, CGameObject::MAP))
            {
                re_setting();
               
            }
            m_bshow_ALLDel_MessageBox = false; // 메시지 상자 닫기
        }
        ImGui::SameLine(); // 같은 줄에 Cancel 버튼 배치
        if (ImGui::Button("Delete Cancel"))
        {
            m_bshow_ALLDel_MessageBox = false; // 메시지 상자 닫기
        }
        ImGui::End();
    }
}

void CLevel_Edit::Msg_Save_box()
{
    if (m_bshow_Save_MessageBox)
    {
        ImGui::Begin("Save Box", &m_bshow_Save_MessageBox); // 제목이 "Message Box"인 창
        ImGui::Text("Save?");                               // 질문 텍스트

        // 버튼들: OK와 Cancel
        if (ImGui::Button("Save_Ok"))
        {
            Save_Terrain(m_tFPath[0]);
            Save_Wall(m_tFPath[1]);
            Save_NonAniObj(m_tFPath[2]);
            Save_Ani(m_tFPath[3]);

            m_bshow_Save_MessageBox = false; // 메시지 상자 닫기
        }
        ImGui::SameLine(); // 같은 줄에 Cancel 버튼 배치
        if (ImGui::Button("Save_Cancel"))
        {
            m_bshow_Del_MessageBox = false; // 메시지 상자 닫기
        }
        ImGui::End();
    }
}

void CLevel_Edit::Msg_Load_box()
{
    if (m_bshow_Load_MessageBox)
    {
        ImGui::Begin("Load Box", &m_bshow_Load_MessageBox); // 제목이 "Message Box"인 창
        ImGui::Text("Load?");                               // 질문 텍스트

        // 버튼들: OK와 Cancel
        if (ImGui::Button("Load_Ok"))
        {
           // Load_Terrain(m_tFPath[0]);
          //  Load_Wall(m_tFPath[1]);
            Load_NonAniObj(m_tFPath[2]);
            Load_Ani(m_tFPath[3]);

            m_bshow_Load_MessageBox = false; // 메시지 상자 닫기
        }
        ImGui::SameLine(); // 같은 줄에 Cancel 버튼 배치
        if (ImGui::Button("Load_Cancel"))
        {
            m_bshow_Load_MessageBox = false; // 메시지 상자 닫기
        }
        ImGui::End();
    }
}

void CLevel_Edit::Save_Terrain(const _tchar* tFPath)
{

    list<class CGameObject*> AllSave = m_pGameInstance->Get_ALL_GameObject(LEVEL_EDIT, CGameObject::MAP);

    HANDLE hFile = CreateFile(tFPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD dwByte(0);
    DWORD dwStrByte(0);

    _vector Right = {0.f, 0.f, 0.f, 0.f};
    _vector UP = {0.f, 0.f, 0.f, 0.f};
    _vector LOOK = {0.f, 0.f, 0.f, 0.f};
    _vector POSITION = {0.f, 0.f, 0.f, 0.f};
    _uint Type{0};
    _wstring pModel;
    _tchar* pPoroto;
    _uint TileX{0};
    _uint TileY{0};

    for (auto& ObjList : AllSave)
    {

        Right = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_RIGHT);
        UP = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_UP);
        LOOK = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_LOOK);
        POSITION = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
        Type = ObjList->Get_Data();
        pModel = ObjList->Get_ComPonentName();
        pPoroto = ObjList->Get_ProtoName();

        if (Type == CGameObject::DATA_TERRAIN) {
            TileX = static_cast<CTerrain*>(ObjList)->Get_SizeX();
            TileY = static_cast<CTerrain*>(ObjList)->Get_SizeY();


            WriteFile(hFile, &Right, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &UP, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &LOOK, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &POSITION, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &Type, sizeof(_uint), &dwByte, nullptr);

            WriteFile(hFile, &TileX, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, &TileY, sizeof(_uint), &dwByte, nullptr);

            // wstring 문자열 쓰기
            DWORD strLength = static_cast<DWORD>(pModel.size());
            WriteFile(hFile, &strLength, sizeof(DWORD), &dwByte, NULL);

            // wstring 데이터 쓰기
            WriteFile(hFile, pModel.c_str(), strLength * sizeof(wchar_t), &dwByte, NULL);

            DWORD wcharDataSize = (lstrlenW(pPoroto) + 1) * sizeof(wchar_t);
            WriteFile(hFile, &wcharDataSize, sizeof(DWORD), &dwByte, NULL);
            WriteFile(hFile, pPoroto, wcharDataSize, &dwByte, NULL);
        }
    }

    CloseHandle(hFile);
}

void CLevel_Edit::Save_NonAniObj(const _tchar* tFPath)
{
    list<class CGameObject*> AllSave = m_pGameInstance->Get_ALL_GameObject(LEVEL_EDIT, CGameObject::MAP);

    HANDLE hFile = CreateFile(tFPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD dwByte(0);

    _vector Right = {0.f, 0.f, 0.f, 0.f};
    _vector UP = {0.f, 0.f, 0.f, 0.f};
    _vector LOOK = {0.f, 0.f, 0.f, 0.f};
    _vector POSITION = {0.f, 0.f, 0.f, 0.f};
    _uint Type = {0};
    _wstring pModel ={};
    _tchar* pPoroto = {};

    for (auto& ObjList : AllSave)
    {

        Right = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_RIGHT);
        UP = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_UP);
        LOOK = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_LOOK);
        POSITION = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
        Type = ObjList->Get_Data();
        pModel = ObjList->Get_ComPonentName();
        pPoroto = ObjList->Get_ProtoName();

        if (Type == CGameObject::DATA_NONANIMAPOBJ) {
            WriteFile(hFile, &Right, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &UP, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &LOOK, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &POSITION, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &Type, sizeof(_uint), &dwByte, nullptr);

            DWORD strLength = static_cast<DWORD>(pModel.size() + 1);
            WriteFile(hFile, &strLength, sizeof(DWORD), &dwByte, NULL);

            // wstring 데이터 쓰기
            WriteFile(hFile, pModel.c_str(), strLength * sizeof(_tchar), &dwByte, NULL);

            // tchar 자료형 쓰기
            DWORD Length = (lstrlenW(pPoroto) + 1) * sizeof(_tchar);
            WriteFile(hFile, &Length, sizeof(DWORD), &dwByte, NULL);
            WriteFile(hFile, pPoroto, Length, &dwByte, NULL);
        }
    }
    CloseHandle(hFile);
}

void CLevel_Edit::Save_Wall(const _tchar* tFPath)
{
    list<class CGameObject*> AllSave = m_pGameInstance->Get_ALL_GameObject(LEVEL_EDIT, CGameObject::MAP);

    HANDLE hFile = CreateFile(tFPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD dwByte(0);

    _vector Right = { 0.f, 0.f, 0.f, 0.f };
    _vector UP = { 0.f, 0.f, 0.f, 0.f };
    _vector LOOK = { 0.f, 0.f, 0.f, 0.f };
    _vector POSITION = { 0.f, 0.f, 0.f, 0.f };
    _uint Type = { 0 };
    _wstring pModel = {};
    _tchar* pPoroto = {};

    for (auto& ObjList : AllSave)
    {

        Right = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_RIGHT);
        UP = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_UP);
        LOOK = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_LOOK);
        POSITION = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
        Type = ObjList->Get_Data();
        pModel = ObjList->Get_ComPonentName();
        pPoroto = ObjList->Get_ProtoName();

        if (Type == CGameObject::DATA_WALL) {
            WriteFile(hFile, &Right, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &UP, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &LOOK, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &POSITION, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &Type, sizeof(_uint), &dwByte, nullptr);

            DWORD strLength = static_cast<DWORD>(pModel.size() + 1);
            WriteFile(hFile, &strLength, sizeof(DWORD), &dwByte, NULL);

            // wstring 데이터 쓰기
            WriteFile(hFile, pModel.c_str(), strLength * sizeof(_tchar), &dwByte, NULL);

            // tchar 자료형 쓰기
            DWORD Length = (lstrlenW(pPoroto) + 1) * sizeof(_tchar);
            WriteFile(hFile, &Length, sizeof(DWORD), &dwByte, NULL);
            WriteFile(hFile, pPoroto, Length, &dwByte, NULL);
        }
    }
    CloseHandle(hFile);
}

void CLevel_Edit::Save_Ani(const _tchar* tFPath)
{
    list<class CGameObject*> AllSave = m_pGameInstance->Get_ALL_GameObject(LEVEL_EDIT, CGameObject::MAP);

    HANDLE hFile = CreateFile(tFPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD dwByte(0);

    _vector Right = { 0.f, 0.f, 0.f, 0.f };
    _vector UP = { 0.f, 0.f, 0.f, 0.f };
    _vector LOOK = { 0.f, 0.f, 0.f, 0.f };
    _vector POSITION = { 0.f, 0.f, 0.f, 0.f };
    _uint Type = { 0 };
    _wstring pModel = {};
    _tchar* pPoroto = {};

    for (auto& ObjList : AllSave)
    {

        Right = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_RIGHT);
        UP = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_UP);
        LOOK = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_LOOK);
        POSITION = ObjList->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
        Type = ObjList->Get_Data();
        pModel = ObjList->Get_ComPonentName();
        pPoroto = ObjList->Get_ProtoName();

        if (Type == CGameObject::DATA_DOOR) {
            WriteFile(hFile, &Right, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &UP, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &LOOK, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &POSITION, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &Type, sizeof(_uint), &dwByte, nullptr);

            DWORD strLength = static_cast<DWORD>(pModel.size() + 1);
            WriteFile(hFile, &strLength, sizeof(DWORD), &dwByte, NULL);

            // wstring 데이터 쓰기
            WriteFile(hFile, pModel.c_str(), strLength * sizeof(_tchar), &dwByte, NULL);

            // tchar 자료형 쓰기
            DWORD Length = (lstrlenW(pPoroto) + 1) * sizeof(_tchar);
            WriteFile(hFile, &Length, sizeof(DWORD), &dwByte, NULL);
            WriteFile(hFile, pPoroto, Length, &dwByte, NULL);
        }
        else if (Type == CGameObject::DATA_CHEST) 
        {
            WriteFile(hFile, &Right, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &UP, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &LOOK, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &POSITION, sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &Type, sizeof(_uint), &dwByte, nullptr);

            DWORD strLength = static_cast<DWORD>(pModel.size() + 1);
            WriteFile(hFile, &strLength, sizeof(DWORD), &dwByte, NULL);

            // wstring 데이터 쓰기
            WriteFile(hFile, pModel.c_str(), strLength * sizeof(_tchar), &dwByte, NULL);

            // tchar 자료형 쓰기
            DWORD Length = (lstrlenW(pPoroto) + 1) * sizeof(_tchar);
            WriteFile(hFile, &Length, sizeof(DWORD), &dwByte, NULL);
            WriteFile(hFile, pPoroto, Length, &dwByte, NULL);
        }
    }
    CloseHandle(hFile);
}

void CLevel_Edit::Load_Terrain(const _tchar* tFPath)
{
    HANDLE hFile = CreateFile(tFPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return;
    }

    DWORD dwByte(0);
    _vector Right = {0.f, 0.f, 0.f, 0.f};
    _vector UP = {0.f, 0.f, 0.f, 0.f};
    _vector LOOK = {0.f, 0.f, 0.f, 0.f};
    _vector POSITION = {0.f, 0.f, 0.f, 0.f};
    _uint Type{0};
    _tchar* pPoroto{};
    _uint TileX{0};
    _uint TileY{0};

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
        bFile = ReadFile(hFile, pPoroto, Length , &dwByte, nullptr);
        pPoroto[Length] = L'\0';

        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
            break;
        }
    
            CGameObject::GAMEOBJ_DESC pDec;
            pDec.fSpeedPerSec = m_fspped;
            pDec.fRotationPerSec = m_fRotfspped;
            pDec.ProtoName = pPoroto;
            pDec.DATA_TYPE = static_cast<CGameObject::GAMEOBJ_DATA>( Type);
            CGameObject* pGameObject = m_pGameInstance->Clone_GameObject(pPoroto, &pDec);
            pGameObject->Set_Buffer(TileX, TileY);
            pGameObject->Set_Model(pModel);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);

            m_pGameInstance->Add_Clon_to_Layers(LEVEL_EDIT, CGameObject::MAP, pGameObject);

            m_pTerrainVT = static_cast<CTerrain*>(pGameObject)->Get_buffer();
            m_pTerrainTR = m_pGameInstance->Recent_GameObject(CGameObject::MAP)->Get_Transform();
            m_TerrainVT.push_back(m_pTerrainTR);
        
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
        
    }
    CloseHandle(hFile);
}

void CLevel_Edit::Load_NonAniObj(const _tchar* tFPath)
{
    HANDLE hFile = CreateFile(tFPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return;
    }

 DWORD dwByte(0);

    _vector Right = {0.f, 0.f, 0.f, 0.f};
    _vector UP = {0.f, 0.f, 0.f, 0.f};
    _vector LOOK = {0.f, 0.f, 0.f, 0.f};
    _vector POSITION = {0.f, 0.f, 0.f, 0.f};
    _uint Type = {0};
    wchar_t* pModel = {};
    _tchar* pPoroto = {};
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
        pModel = new wchar_t[strLength + 1]; // NULL 종단 추가
        bFile = ReadFile(hFile, pModel, strLength * sizeof(wchar_t), &dwByte, NULL);
        pModel[strLength] = L'\0';

        DWORD Length;
        bFile = ReadFile(hFile, &Length, sizeof(DWORD), &dwByte, NULL);

        pPoroto = new _tchar[Length + 1];
        bFile = ReadFile(hFile, pPoroto, Length , &dwByte, NULL);
        pPoroto[Length] = L'\0';

        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
            break;
        }
  
            CGameObject::GAMEOBJ_DESC pDec;
            pDec.fSpeedPerSec = m_fspped;
            pDec.fRotationPerSec = m_fRotfspped;
            pDec.ProtoName = pPoroto;
            pDec.DATA_TYPE = static_cast<CGameObject::GAMEOBJ_DATA>(Type);
            CGameObject* pGameObject = m_pGameInstance->Clone_GameObject(pPoroto, &pDec);

            pGameObject->Set_Model(pModel);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);

            m_pGameInstance->Add_Clon_to_Layers(LEVEL_EDIT, CGameObject::MAP, pGameObject);
        
        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }   


    CloseHandle(hFile);
}

void CLevel_Edit::Load_Wall(const _tchar* tFPath)
{
    HANDLE hFile = CreateFile(tFPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return;
    }

    DWORD dwByte(0);

    _vector Right = { 0.f, 0.f, 0.f, 0.f };
    _vector UP = { 0.f, 0.f, 0.f, 0.f };
    _vector LOOK = { 0.f, 0.f, 0.f, 0.f };
    _vector POSITION = { 0.f, 0.f, 0.f, 0.f };
    _uint Type = { 0 };
    wchar_t* pModel = {};
    _tchar* pPoroto = {};
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
        pModel = new wchar_t[strLength + 1]; // NULL 종단 추가
        bFile = ReadFile(hFile, pModel, strLength * sizeof(wchar_t), &dwByte, NULL);
        pModel[strLength] = L'\0';

        DWORD Length;
        bFile = ReadFile(hFile, &Length, sizeof(DWORD), &dwByte, NULL);

        pPoroto = new _tchar[Length + 1];
        bFile = ReadFile(hFile, pPoroto, Length, &dwByte, NULL);
        pPoroto[Length] = L'\0';

        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
            Safe_Delete_Array(pPoroto);
            break;
        }

        CGameObject::GAMEOBJ_DESC pDec;
        pDec.fSpeedPerSec = m_fspped;
        pDec.fRotationPerSec = m_fRotfspped;
        pDec.ProtoName = pPoroto;
        pDec.DATA_TYPE = static_cast<CGameObject::GAMEOBJ_DATA>(Type);
        CGameObject* pGameObject = m_pGameInstance->Clone_GameObject(pPoroto, &pDec);

        pGameObject->Set_Model(pModel);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);
         
        m_pGameInstance->Add_Clon_to_Layers(LEVEL_EDIT, CGameObject::MAP, pGameObject);

        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }


    CloseHandle(hFile);
}

void CLevel_Edit::Load_Ani(const _tchar* tFPath)
{
   HANDLE hFile = CreateFile(tFPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) // 개방 실패 시
    {
        return;
    }

    DWORD dwByte(0);

    _vector Right = { 0.f, 0.f, 0.f, 0.f };
    _vector UP = { 0.f, 0.f, 0.f, 0.f };
    _vector LOOK = { 0.f, 0.f, 0.f, 0.f };
    _vector POSITION = { 0.f, 0.f, 0.f, 0.f };
    _uint Type = { 0 };
    wchar_t* pModel = {};
    _tchar* pPoroto = {};
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
        pModel = new wchar_t[strLength + 1]; // NULL 종단 추가
        bFile = ReadFile(hFile, pModel, strLength * sizeof(wchar_t), &dwByte, NULL);
        pModel[strLength] = L'\0';

        DWORD Length;
        bFile = ReadFile(hFile, &Length, sizeof(DWORD), &dwByte, NULL);

        pPoroto = new _tchar[Length + 1];
        bFile = ReadFile(hFile, pPoroto, Length, &dwByte, NULL);
        pPoroto[Length] = L'\0';

        if (0 == dwByte)
        {
            Safe_Delete_Array(pModel);
           Safe_Delete_Array(pPoroto);
            break;
        }

        CGameObject::GAMEOBJ_DESC pDec;
        pDec.fSpeedPerSec = m_fspped;
        pDec.fRotationPerSec = m_fRotfspped;
        pDec.ProtoName = pPoroto;
        pDec.DATA_TYPE = static_cast<CGameObject::GAMEOBJ_DATA>(Type);
        CGameObject* pGameObject = m_pGameInstance->Clone_GameObject(pPoroto, &pDec);

        pGameObject->Set_Model(pModel);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, Right);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_UP, UP);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_LOOK, LOOK);
        pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, POSITION);

        m_pGameInstance->Add_Clon_to_Layers(LEVEL_EDIT, CGameObject::MAP, pGameObject);

        Safe_Delete_Array(pModel);
        Safe_Delete_Array(pPoroto);
    }
    CloseHandle(hFile);
}

void CLevel_Edit::Key_input(_float ftimedelta)
{
    if (m_pGameInstance->Get_DIMouseDown(DIM_RB))
    {
        if (false == m_pGameInstance->IsGameObject(LEVEL_EDIT, CGameObject::MAP))
            return;
        else
            Picking_Pos();
    }

    if (m_pGameInstance->Get_DIKeyDown(DIK_DELETE))
    {
        m_bshow_ALLDel_MessageBox = true;
    }

    if (m_pGameInstance->Get_DIKeyDown(DIK_SUBTRACT)) // 뺴기 버튼
    {
        m_bshow_Del_MessageBox = true;
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_UP))
    {
        m_pObjTransform->Go_Straight(ftimedelta);
        Update_Pos();
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_DOWN))
    {
        m_pObjTransform->Go_Backward(ftimedelta);
        Update_Pos();
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
    {
        m_pObjTransform->Go_Right(ftimedelta);
        Update_Pos();
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_LEFT))
    {
        m_pObjTransform->Go_Left(ftimedelta);
        Update_Pos();
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_N))
    {
        m_pObjTransform->Go_Up(ftimedelta);
        Update_Pos();
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_M))
    {
        m_pObjTransform->Go_Down(ftimedelta);
        Update_Pos();
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_R))
    {
        _long MouseMove = {0};

        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
        {
            m_pObjTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), ftimedelta * MouseMove * m_fRotfspped);
        }

        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
        {
            m_pObjTransform->Turn(m_pObjTransform->Get_TRANSFORM(CTransform::TRANSFORM_RIGHT),
                                  ftimedelta * MouseMove * m_fRotfspped);
        }
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_C))
    {
        if (m_pGameInstance->Get_DIMouseDown(DIM_LB))
        {
            _vector RayPos, RayDir;
            m_pGameInstance->Make_Ray(g_hWnd, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                                      m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir);

            CGameObject::PICKEDOBJ_DESC Desc = m_pGameInstance->Pking_onMash(RayPos, RayDir);

            if (Desc.pPickedObj)
            {
                Desc.pPickedObj->Set_Dead(true);
            }
        }
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_P))
    {

        _vector RayPos, RayDir;
        m_pGameInstance->Make_Ray(g_hWnd, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                                  m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir);

        CGameObject::PICKEDOBJ_DESC Desc = m_pGameInstance->Pking_onMash(RayPos, RayDir);

        if (Desc.pPickedObj)
        {

            m_pObjTransform = Desc.pPickedObj->Get_Transform();

            Update_Pos();
            Update_Speed();
        }
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_L))
    {
        if (m_pGameInstance->Get_DIMouseDown(DIM_LB))
        {
            _vector RayPos, RayDir;
            m_pGameInstance->Make_Ray(g_hWnd, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                                      m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir);

            CGameObject::PICKEDOBJ_DESC Desc = m_pGameInstance->Pking_onMash(RayPos, RayDir);

            if (Desc.pPickedObj)
            {
                _vector Set = RayPos + RayDir * Desc.fDis;
                m_pObjTransform->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, Set);
                Update_Pos();
            }
        }
    }
}

void CLevel_Edit::Update_Pos()
{

    _float3 vPos = {XMVectorGetX(m_pObjTransform->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)),
                    XMVectorGetY(m_pObjTransform->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)),
                    XMVectorGetZ(m_pObjTransform->Get_TRANSFORM(CTransform::TRANSFORM_POSITION))};

    m_fposition[0] = vPos.x;
    m_fposition[1] = vPos.y;
    m_fposition[2] = vPos.z;
}

void CLevel_Edit::Update_Speed()
{
    m_fspped = m_pObjTransform->Get_MoveSpeed();
    m_fRotfspped = m_pObjTransform->Get_RotSpeed();
}

void CLevel_Edit::Set_Speed()
{
    _bool ret;
    ImGui::Text("Move_speed");
    ImGui::SameLine(100, 0);
    ImGui::InputFloat("##23", &m_fspped);

    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.f, 0.5f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(1.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(1.f, 0.f, 1.f));
    ret = ImGui::Button("MSpeed");
    ImGui::PopStyleColor(3);
    if (ret)
    {
        if (false == m_pGameInstance->IsGameObject(LEVEL_EDIT, CGameObject::MAP))
            return;
        m_pObjTransform->Set_MoveSpeed(m_fspped);
    }

    _bool rt;
    ImGui::Text("Rot_speed");
    ImGui::SameLine(100, 0);
    ImGui::InputFloat("##25", &m_fRotfspped);

    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.f, 0.5f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(1.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(1.f, 0.f, 1.f));
    rt = ImGui::Button("RSpeed");
    ImGui::PopStyleColor(3);
    if (rt)
    {
        if (false == m_pGameInstance->IsGameObject(LEVEL_EDIT, CGameObject::MAP))
            return;
        m_pObjTransform->Set_RotSpeed(m_fRotfspped);
    }
}

void CLevel_Edit::Picking_Pos()
{

    if (m_pTerrainTR == nullptr)
        return;
    _float3 PickPos{};

    _float3 Pick = m_pGameInstance->Picking_OnTerrain(
        g_hWnd, m_pTerrainVT, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
        m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), m_pTerrainTR);

    _bool roop{};
    if (Pick.x == 0.f && Pick.y == 0.f && Pick.z == 0.f)
    {
        roop = true;
    }
    if (roop)
    {
        for (auto& VT : m_TerrainVT)
        {
            _float3 Picking = m_pGameInstance->Picking_OnTerrain(
                g_hWnd, m_pTerrainVT, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
                m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), VT);

            if (Picking.x != 0.f || Picking.y != 0.f || Picking.z != 0.f)
            {
                XMStoreFloat3(&Pick, XMLoadFloat3(&Picking));

                break;
            }
            else
                continue;
        }
    }
    XMStoreFloat3(&PickPos, XMLoadFloat3(&Pick));

    m_pObjTransform->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(PickPos.x, PickPos.y, PickPos.z, 1.f));
    Update_Pos();
}

CLevel_Edit* CLevel_Edit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Edit* pInstance = new CLevel_Edit(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Edit");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Edit::Free()
{
    __super::Free();

    for (size_t i = 0; i < POROTO_TYPE_END; i++)
    {
        for (auto& str : m_protokey[i]) { Safe_Delete(str); }
        m_protokey[i].clear();
        m_protokey[i].shrink_to_fit();
    }

    for (size_t i = 0; i < POROTO_TYPE_END; i++)
    {
        for (auto& str : m_protoComkey[i]) { Safe_Delete(str); }
        m_protoComkey[i].clear();
        m_protoComkey[i].shrink_to_fit();
    }
}
