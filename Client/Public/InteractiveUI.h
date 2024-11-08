#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)

class CVIBuffer_Rect;
class CTransform;
class CTexture;

END

BEGIN(Client)
class CInteractiveUI : public CUI
{
public:
	
private:
	CInteractiveUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteractiveUI(const CInteractiveUI& Prototype);
	virtual ~CInteractiveUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual void    Update_for_Edit(_float fTimeDelta) override {};
	virtual HRESULT Render() override;
	void Set_Text(const _tchar* pText){
	 	m_pText = pText;
	}
	_bool Get_Interactive();

	void  Set_Interactive(_bool Interactive);
private:
	HRESULT				Add_Components();

private:
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	const _tchar* m_pText = { nullptr };
	_vector m_vColor = {
		1.f,1.f,1.f,1.f
	};
	_bool m_bColor = false;
	_float m_ColorTime = { 0.f };
	_bool m_bInteractive = {false};
public:
	static CInteractiveUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
