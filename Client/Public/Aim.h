#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CAim :  public CUI
{
public:
	typedef struct CAim_DESC : public CUI::CUI_DESC
	{
		LOADINGID LoadingID{};
		_bool  bshakingX{};
		_bool  bshakingY{};
		_float fpositive_OR_negative{};
	}CAim_DESC;

private:
	CAim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAim(const CAim& Prototype);
	virtual ~CAim() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private : 



private:
	LOADINGID		m_pLoadingID = {};
	CTexture*	    m_pTextureCom = {};
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float m_fPrXPos{};
	_float m_fPrYPos{};

	_float m_positive_OR_negative{};
	_bool m_bshakingX = { false };
	_bool m_bshakingY = {false };
private:
	HRESULT Add_Components();
	

public:
	static CAim*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
