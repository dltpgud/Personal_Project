#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI abstract : public CGameObject
{
public:
   
    typedef struct CUI_DESC : CGameObject::GAMEOBJ_DESC
    {
        _float fX{}, fY{}, fZ{}, fSizeX{}, fSizeY{};
        _uint UID{};
        _uint iDeleteLevel{};
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
    virtual void ShakingEvent(_float fTimeDelta) {};

    void Set_UI_Pos(void* pArg);
    void UI_shaking(_float fTimeDelta);
    void Set_UI_shaking( _float fShakingTime, _float fPowerX, _float fPowerY);
    _bool Check_Deleate(_uint Level);
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

    
	void Show(class CGameObject* pOwner)
    {
        m_pOwner = pOwner;
        m_bUpdate = true;
    }

    void Hide(class CGameObject* pRequest)
    {
        if (m_pOwner == pRequest)
        {
            m_bUpdate = false;
            m_pOwner = nullptr;
        }
    }

    const _uint& Get_UIID()
    {
        return m_UIID;
    }

protected:
    _float m_fX{}, m_fY{}, m_fZ{}, m_fSizeX{}, m_fSizeY{};
    _float4x4 m_ViewMatrix{}, m_ProjMatrix{};
    _uint m_UIID{};
  class CGameObject* m_pOwner{};
    _uint m_iDeleteLevel{};
    _bool m_bUpdate{true};
    /*wincx ,wincy 가져오기*/
    _uint iNumViewports = {1};
     D3D11_VIEWPORT ViewportDesc{};
    _float m_fShaking_X{}, m_fShaking_Y{}, m_fShakingTime{}, m_fShakingPower_X{}, m_fShakingPower_Y{};
 
public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};

END