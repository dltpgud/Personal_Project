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

private:


private:
	CTexture* m_pTextureCom_HpBar = {nullptr};
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
private:
	HRESULT Add_Components();
	_float m_HP = 100.f;
	_float m_MaxHP = 100.f;
	_float m_HP_Pluse = 0.f;

public:
	static CPlayerUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;

};

END