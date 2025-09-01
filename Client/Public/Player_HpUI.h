#pragma once
#include "Client_Defines.h"
#include "UI.h"
BEGIN(Engine)
	class CShader;
	class CTexture;
	class CVIBuffer_Rect;
END

BEGIN(Client)

class CPlayer_HpUI: public CUI
{
public:
		typedef struct CPlayerUI_DESC : public CUI::CUI_DESC
		{

		}CPlayerUI_DESC;

private:
	CPlayer_HpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_HpUI(const CPlayer_HpUI& Prototype);
	virtual ~CPlayer_HpUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
    virtual void    ShakingEvent(_float fTimeDelta) override;

public:
	void Set_PlayerMaxHP(_int iMaxHP) 
	{
	 m_iMaxHP = iMaxHP;
	 swprintf_s(m_tfMaxHp, 50, L"%d\n", m_iMaxHP);
	}
	void Set_PlayerHP(_int iHP) 
	{ 
	 m_iHP = iHP;
	 swprintf_s(m_tfHP, 50, L"%d\n", m_iHP);
	}

	void Set_HPGage(_int GageCount) {
		if (GageCount < -1)
			return;
		
		if(m_iMaxHP == m_iHP)
			return;

		m_iGageCount = GageCount;
	};

private:
    HRESULT Add_Components();
	
private:
    CTexture* m_pTextureCom_HpBar = {nullptr};
    CShader* m_pShaderCom = {nullptr};
    CVIBuffer_Rect* m_pVIBufferCom = {nullptr};

	_int m_iHP{};
	_int m_iMaxHP{};
	_float m_fHP_Pluse = { 0.f };
	_float m_fPrXPos{}, m_fPrYPos{};
	_int   m_iGageCount = {-1};
	_int m_iHealthHP{};
    _float m_fRatio{};
	_tchar m_tfHP[50];
	_tchar m_tfMaxHp[50];

public:	
	static CPlayer_HpUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;

};

END