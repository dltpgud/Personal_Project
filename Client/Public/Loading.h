#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CLoading :  public CUI
{
public:
	typedef struct CLoading_DESC : public CUI::CUI_DESC
	{
		_int TexIndex{};

	}CLoading_DESC;

private:
	CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoading(const CLoading& Prototype);
	virtual ~CLoading() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*	    m_pTextureCom = {};
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	_int            m_iTexIndex{};
	_float          m_fFirstX{};
	CLoading_DESC   Desc{};
private:
	HRESULT Add_Components();
    HRESULT Set_LoadPos(CLoading_DESC* pArg);

public:
	static CLoading*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
