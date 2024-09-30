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
		LOADINGID LoadingID{};
		_float x{};

	}CLoading_DESC;

private:
	CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoading(const CLoading& Prototype);
	virtual ~CLoading() = default;

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
	 HRESULT Set_LoadPos();
	 HRESULT Set_LoadPos2();
	 void Roop(const _uint& i, CLoading_DESC desc[], CLoading_DESC pdesc[]);
private:
	LOADINGID		m_pLoadingID = {};
	CTexture*	    m_pTextureCom = {};
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CLoading_DESC  Desc[6];
	CLoading_DESC  pDesc[6];

private:
	HRESULT Add_Components();
	

public:
	static CLoading*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
