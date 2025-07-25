#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI abstract : public CGameObject
{
public:
    enum UIID
    {
        UIID_Menu,
        UIID_Cursor,
        UIID_Loading,
        UIID_PlayerHP,
        UIID_PlayerState,
        UIID_PlayerWeaPon,
        UIID_PlayerWeaPon_Aim,
        UIID_PlayerShooting,
        UIID_Monster,
        UIID_InteractiveUI,
        UIID_BossHP,
        UIID_Pade,
        UIID_END
    };

    typedef struct CUI_DESC : CGameObject::GAMEOBJ_DESC
    {
        _float fX{}, fY{}, fZ{}, fSizeX{}, fSizeY{};
        UIID UID{};
        _uint iDeleteLevel{};
        _uint iMaxLevel{};
        _bool* pEnable_Level = {nullptr};
        _bool Update{};

    } CUI_DESC;

protected:
    CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI(const CUI& Prototype);
    virtual ~CUI() = default;

public:
    virtual HRESULT Initialize_Prototype();

    virtual HRESULT Initialize(void* pArg);

    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

    void Set_UI_Pos(void* pArg);
    void UI_shaking(_float fTimeDelta);
    void Set_UI_shaking( _float fShakingTime, _float fPowerX, _float fPowerY);
    _bool Check_Deleate(_uint Level);
    _bool Check_Level(_uint iLEVEL)
    {
        return static_cast<_bool>(m_pEnable_Level[iLEVEL]);
    }
    _bool Check_Update()
    {
        return m_bUpdate;
    }
    _float Get_fz()
    {
        return m_fZ;
    } 
 public:
    void Set_Open(_bool open)
    {
        m_bUpdate = open;
    }

  
    const UIID& Get_UIID()
    {
        return m_UIID;
    }
protected:
    _float m_fX{}, m_fY{}, m_fZ{}, m_fSizeX{}, m_fSizeY{};
    _float4x4 m_ViewMatrix{}, m_ProjMatrix{};
    UIID m_UIID{};
 
    _uint m_iDeleteLevel{};
    _bool* m_pEnable_Level = {nullptr};
    _bool m_bUpdate{true};
    /*wincx ,wincy 가져오기*/
    _uint iNumViewports = {1};
     D3D11_VIEWPORT ViewportDesc{};
    _float m_fShaking_X{}, m_fShaking_Y{}, m_fShakingTime{}, m_fShakingPower_X{}, m_fShakingPower_Y{};
    _bool m_IsShaking = false;
public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};

END