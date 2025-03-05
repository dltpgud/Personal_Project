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
        _bool  Update{};
        _uint iDepth{};
    } CUI_DESC;

protected:
    CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI(const CUI& Prototype);
    virtual ~CUI() = default;

public:
    virtual HRESULT Initialize_Prototype();

    virtual HRESULT Initialize(void* pArg);

    virtual _int Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

    void Set_UI_Pos(void* pArg);
    void UI_shaking(_float fTimeDelta);
    void Set_UI_shaking( _float fShakingTime, _float fPowerX, _float fPowerY);
    
    public:
    void Set_Open(_bool open)
    {
        m_bUpdate = open;
    }

    const _bool& Get_Update()
    {
        return m_bUpdate;
    }
    const UIID& Get_UIID()
    {
        return m_UIID;
    }
    const _uint& Get_iDepth()
    {
        return m_iDepth;
    }

protected:
    _float m_fX{}, m_fY{}, m_fZ{}, m_fSizeX{}, m_fSizeY{};
    _float4x4 m_ViewMatrix{}, m_ProjMatrix{};
    UIID m_UIID{};
  
    /*메뉴간 겹치는 지 안겹치는지 소팅을 위한 변수*/
    _uint m_iDepth = {};
    // 프리 업데이트, 레이트 업데이트,
    _bool m_bUpdate{};


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