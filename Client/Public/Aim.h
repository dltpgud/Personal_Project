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
		_bool  bshakingX{};
		_bool  bshakingY{};
		_float fpositive_OR_negative{};
        _float fPrXPos{};
        _float fPrYPos{};
	}CAim_DESC;

private:
	CAim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAim(const CAim& Prototype);
	virtual ~CAim() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
    virtual void    ShakingEvent(_float fTimeDelta) override;

private:
    CTexture*       m_pTextureCom = {nullptr};
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CAim_DESC       m_AimDesc[4];
private:
	HRESULT Add_Components();
    HRESULT Set_Desc();
public:
	static CAim*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
