#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CSpriteTexture :  public CUI
{
public:
	typedef struct CSpriteTexture_DESC : public CUI::CUI_DESC
	{
		LOADINGID LoadingID{};
		_int TexIndex{};
		_float interver{};
		
	}CSpriteTexture_DESC;

private:
	CSpriteTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpriteTexture(const CSpriteTexture& Prototype);
	virtual ~CSpriteTexture() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual void    Update_for_Edit(_float fTimeDelta) override {};
	virtual HRESULT Render() override;

private : 
	 HRESULT Initialize_GORGE();


private:
	LOADINGID		m_pLoadingID = {};
	CTexture*	    m_pTextureCom = {};
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	const _tchar* m_wKey{};
	_int m_TexIndex{};
	_int m_index{};
	_float m_fTick{};
	_float m_interver{};

private:
	HRESULT Add_Components();
	

public:
	static CSpriteTexture*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
