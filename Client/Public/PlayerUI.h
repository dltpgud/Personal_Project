#pragma once
#include "Client_Defines.h"
#include "UI.h"
BEGIN(Engine)
	class CShader;
	class CTexture;
	class CVIBuffer_Rect;
	END

BEGIN(Client)

class CPlayerUI: public CUI
{
public:
		typedef struct CPlayerUI_DESC : public CUI::CUI_DESC
		{

		}CPlayerUI_DESC;

private:
	CPlayerUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerUI(const CPlayerUI& Prototype);
	virtual ~CPlayerUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual void    Update_for_Edit(_float fTimeDelta) override {};
	virtual HRESULT Render() override;

public:
	void Set_PlayerMaxHP(_float fMaxHP) { m_fMaxHP = fMaxHP;
	swprintf_s(m_tfMaxHp, 50, L"%.f\n", m_fMaxHP);
	}
	void Set_PlayerHP(_float fHP) { m_fHP = fHP;
	swprintf_s(m_tfHP, 50, L"%.f\n", m_fHP);
	}

	void Set_HPGage(_int GageCount) {
		if (GageCount < -1)
			return;
		
		if(m_fMaxHP == m_fHP)
			return;

		m_iGageCount = GageCount;
	};



private:
	CTexture* m_pTextureCom_HpBar = {nullptr};
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
private:
	HRESULT Add_Components();
	_float m_fHP{};
	_float m_fMaxHP{};
	_float m_fHP_Pluse = { 0.f };
	_float m_fPrXPos{}, m_fPrYPos{};
	_int   m_iGageCount = {-1};
	_float m_fHealthHP{};


	_tchar	m_tfHP[50];

	_tchar	m_tfMaxHp[50];
public:	
	static CPlayerUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;

};

END