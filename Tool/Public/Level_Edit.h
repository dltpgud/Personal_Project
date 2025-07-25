#pragma once

#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Terrain;
class CTerrain;
class CGameObject;
class CComponent;
END

BEGIN(Tool)

class CLevel_Edit final : public CLevel
{
public:
    enum POROTO_TYPE
    {
        TERRAIN,
        WALL,
        NONANIMAPOBJ,
        ANIMAPOBJ,
        MONSTER,
        NPC,
        POROTO_TYPE_END
    };

private:
    CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_Edit() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    void Tool();
    void Map();
    void Another();
    void Menu_File();
    void ComboType();
    void MapNONANIObj_ListBox();
    void MapANIObj_ListBox();
    void Terrain_ListBox();
    void Wall_ListBox();
    void Monster_ListBox();
    void NPC_ListBox();
    HRESULT Create_Layer_Obj(POROTO_TYPE type, const _wstring& pLayerTag, _int Iindex, _uint Comindex);

    void Push_Proto_vec();
    void Push_ProtoCom_vec();
    void Create_Leyer_Botton(POROTO_TYPE type, _uint Objnum, _uint Comnum);
    HRESULT Ready_ToolCamera(const _wstring& pLayerTag);
    HRESULT Ready_Light();

    void Msg_collection();
    void Msg_Del_Box();
    void Msg_ALL_Del_Box();
    void Msg_Save_box();
    void Msg_Load_box();

    void Save_Terrain(const _tchar* tFPath);
    void Save_NonAniObj(const _tchar* tFPath);
    void Save_Wall(const _tchar* tFPath);
    void Save_Ani(const _tchar* tFPath);
    void Save_Navigation (const _tchar* tFPath);
    void Save_Monster(const _tchar* tFPath);
    void Save_NPC(const _tchar* tFPath);

    void Load_Terrain(const _tchar* tFPath);
    void Load_NonAniObj(const _tchar* tFPath);
    void Load_Wall(const _tchar* tFPath);
    void Load_Ani(const _tchar* tFPath);
    void Load_Navigation(const _tchar* tFPath);
    void Load_Monster(const _tchar* tFPath);
    void Load_NPC(const _tchar* tFPath);

    void ChsetWeapon();
    void Key_input(_float ftimedelta);

    void Update_Pos();
    void Update_Speed();

    void Set_Speed();
    void Set_pos();
    void Set_scale();
    void Set_Tile();
    void Set_Cell_Type();
    void Set_Door_Type();
    void re_setting();

    void Picking_Pos();
    void Picking_Cell(_uint i);


private:
    /*imgui 창 변수*/
    _bool m_bshow_Map_window = false;
    _bool m_bshow_another_window = false;
    _bool m_bshow_win_Terrian = false;
    _bool m_bshow_win_Wall = false;
    _bool m_bshow_win_NONaniObj = false;
    _bool m_bshow_win_aniObj = false;
    _bool m_bshow_win_Monster = false;
    _bool m_bshow_win_NPC = false;
    /*메세지 박스*/
    _bool m_bshow_Del_MessageBox = false;
    _bool m_bshow_Save_MessageBox = false;
    _bool m_bshow_Load_MessageBox = false;
    _bool m_bshow_ALLDel_MessageBox = false;

private:
    /*실질적 기능 구현에 필요한 변수*/
    /*int */
    _int m_iMapObj_Type{};
    _int m_iScene_Type{};
    _int m_iItem_selected_idx[6]{};
    _int m_iIcomtem_selected_idx[6]{};
    _int m_iBufferCount[2]{};
    _int m_CopyiItem_selected_idx{};
    _int m_CopyiIcomtem_selected_idx{};
    _int m_WeaPon{};
    _int m_iCellType{};

    _uint m_iScene{};
    _uint m_iCellCount{};
    POROTO_TYPE m_pType{};

    /*float*/
    _float m_fspped{};
    _float m_fRotfspped{};
    _float m_fposition[3];
    _float m_fscale[3];
    _float m_FireOffset{};
    _float3 m_fCellPoint[3];

    _bool m_bCell = {false};
    _bool m_Key = {false};
    _bool m_bSetCellW = false;
    _bool m_bLoadCell = false;
    _bool m_bFireTile = false;
    CTransform* m_pObjTransform = {nullptr};

    _wstring m_wPrototype_Key{};
    const _tchar* m_tFPath[7]{};

    vector<_char*> m_protokey[POROTO_TYPE_END];
    vector<_char*> m_protoComkey[POROTO_TYPE_END];
    vector<CGameObject*> m_vTerrain;

    CGameObject* m_PicObj = {nullptr};
    CGameObject* m_PicObjTerrain = { nullptr };
    CGameObject* m_Terrain = {nullptr};
    CComponent* m_pNavigation = {nullptr};
    _int m_DoorType{ 0 };
   // float a = 865, b = 502, c = -1500, d = -1139,e =208,f =203;
    //_vector m_Eye = XMVectorSet(143, 394, 432, 0.f);
    //_vector m_Dire = XMVectorSet(139, 17, 2, 0.f);
    float a = 143, b = 394, c = 432, d = 139, e = 10.f, f = 0;

public:
    static CLevel_Edit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END