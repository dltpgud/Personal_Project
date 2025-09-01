#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
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

	void Set_Fade(_bool fade, _bool LoadingFade = false); 

	_bool FinishPade();

private:
	CShader*	    m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr }; 

	_float m_fTimeSum = {1.f };
	_bool m_bSetFade{ true };
    _bool m_bSetLoadingFade{};
private:
	HRESULT Add_Components();
	
public:
	static CFade*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
