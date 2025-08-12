#pragma once
#include "Client_Defines.h"
#include "UI.h"
#include "Weapon.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CWeaponUI: public CUI
{
public:
public:
    typedef struct CWeaponUI_DESC : public CUI::CUI_DESC{
    } CWeaponUI_DESC;

private:
    CWeaponUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CWeaponUI(const CWeaponUI& Prototype);
    virtual ~CWeaponUI() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual void    ShakingEvent(_float fTimeDelta) override;
public:
    void            Set_ScecondWeapon(const _uint& type);
   
private:
    CTexture*       m_pTextureCom  = {nullptr};
    CShader*        m_pShaderCom   = {nullptr};
    CVIBuffer_Rect* m_pVIBufferCom = {nullptr};

private:
    _bool m_bChooseWeaPon          = {true};
    _bool m_bRander                  {};
    _bool m_change                   {};

    CWeaponUI_DESC  m_Desc[6]        {};
    CWeaponUI_DESC  m_ScecondDesc[5] {};
    _uint           m_WeaPonUI       {};
    _uint           m_ScecondWeaPonUI{};
    _float          m_fXPos          {};
    _float          m_fYPos          {};
    _float          m_fPrXPos[6]     {};
    _float          m_fPrYPos[6]     {};            
    _float          m_fPrXPos2[5]    {};
    _float          m_fPrYPos2[5]    {};
    _int            m_iPreBullet     {};
    _tchar          m_wCurrentBullte[50]{};
    _uint           m_iCurrentBullet {};
    _tchar          m_wMaxBullte[50] {};
    _uint           m_iMaxBullet     {};
    _vector         m_vColar         {};

private:
    HRESULT Add_Components();
    HRESULT Set_WeaponUI_Pos();
    HRESULT Set_WeaponUI_Pos2();

public:
    static CWeaponUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END