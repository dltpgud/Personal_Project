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
        UIID_PlayerWeaPon,
        UIID_PlayerWeaPon_Aim,
        UIID_Monster,
        UIID_InteractiveUI,
        UIID_END
    };
    enum Hover_STATE
    {
        HS_Hover,
        HS_NonHover,
        HS_END
    };

    typedef struct CUI_DESC : CGameObject::GAMEOBJ_DESC
    {
        _float fX{}, fY{}, fZ{}, fSizeX{}, fSizeY{};
        UIID UID{};
        _bool PrUpdate{}, Update{}, LateUpdate{};
        _uint iDepth{};
        Hover_STATE Hoverst;
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
    virtual void Update_for_Edit(_float fTimeDelta){};
    virtual HRESULT Render();

    void Set_UI_Pos(void* pArg);
    void UI_shaking(_float fTimeDelta);
    void Set_UI_shaking( _float fShakingTime, _float fPowerX, _float fPowerY);
    
    public:
    void Set_Open(_bool open)
    {
        m_bPrUpdate = open;
        m_bUpdate = open;
        m_bLateUpdate = open;
    }

    void Set_PrUpdate(_bool open)
    {
        m_bPrUpdate = open;
    }
    void Set_Update(_bool open)
    {
        m_bUpdate = open;
    }
    void Set_LateUpdate(_bool open)
    {
        m_bLateUpdate = open;
    }

    const _bool& Get_PrUpdate()
    {
        return m_bPrUpdate;
    }
    const _bool& Get_Update()
    {
        return m_bUpdate;
    }
    const _bool& Get_LateUpdate()
    {
        return m_bLateUpdate;
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
    Hover_STATE m_Hoverst;
    /*메뉴간 겹치는 지 안겹치는지 소팅을 위한 변수*/
    _uint m_iDepth = {};
    // 프리 업데이트, 레이트 업데이트,
    _bool m_bPrUpdate{};
    _bool m_bUpdate{};
    _bool m_bLateUpdate{};

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