#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)

class CVIBuffer_Rect;
class CTransform;
class CTexture;

END

class CCursor : public CUI
{
private:
	CCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCursor(const CCursor& Prototype);
	virtual ~CCursor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual void    Update_for_Edit(_float fTimeDelta) override {};
	virtual HRESULT Render() override;
private:
	HRESULT				Add_Components();

private:
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

public:
	static CCursor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};

