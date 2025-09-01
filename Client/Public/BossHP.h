#pragma once
#include "Client_Defines.h"
#include "UI.h"
BEGIN(Engine)
	class CShader;
	class CTexture;
	class CVIBuffer_Rect;
END

BEGIN(Client)

class CBossHPUI: public CUI
{
public:
		typedef struct CBossHPUI_DESC : public CUI::CUI_DESC
		{
			_int fMaxHP{};
			_int fHP{};
		}CBossHPUI_DESC;

private:
	CBossHPUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossHPUI(const CBossHPUI& Prototype);
	virtual ~CBossHPUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
    virtual void    ShakingEvent(_float fTimeDelta) override;

public:
	void Set_BossMaxHP(_int fMaxHP) { m_imaxHP = fMaxHP; }
	void Set_BossHP   (_int fHP)    { m_iHP = fHP; }
	void Set_HPGage   (_int GageCount) 
	{
		if (GageCount < -1)
			return;
		
		if(m_imaxHP == m_iHP)
			return;

		m_iGageCount = GageCount;
	};

private:
	CTexture*       m_pTextureCom = {nullptr};
	CShader*        m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
private:
	HRESULT Add_Components();
private:

	_int m_iHP{};
	_int m_imaxHP{};
	_float m_fHP_Pluse = { 0 };
	_float m_fPrXPos{}, m_fPrYPos{};
	_int   m_iGageCount = {-1};
	_float m_fHealthHP{};
    _float m_CurRito{};
	_tchar m_tfHP[50];
	_tchar m_tfMaxHp[50];
	_float m_fRatio{ 0.f };

public:	
	static CBossHPUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;

};

END