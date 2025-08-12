#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CFade :  public CUI
{
private:
	CFade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFade(const CFade& Prototype);
	virtual ~CFade() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
        virtual void Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Fade(_bool fade) {
		m_bSetPade = fade;
         true == fade ? m_fTimeSum = 0.0f : m_fTimeSum= 1.0f;
	}

	_bool FinishPade();

private:
	CTexture*		m_pTextureCom = { nullptr };
	CShader*	    m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr }; 

	_float m_fTimeSum = {1.f };
	_bool m_bSetPade{ true };
private:
	HRESULT Add_Components();
	
public:
	static CFade*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
