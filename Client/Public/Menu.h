#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CMenu :  public CUI
{
private:
			CMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
			CMenu(const CMenu& Prototype);
	virtual ~CMenu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*		m_pTextureCom = { nullptr };
	CShader*	    m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr }; 
	CUI_DESC		Desc[3]{};
	_bool           m_bHoverSound = { false };
private:
	HRESULT Add_Components();
    HRESULT Set_MenuPos();

public:
	static CMenu*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
