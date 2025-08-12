#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBossIntroBG :  public CUI
{
public:
	enum TYPE { TYPE_Mask, TYPE_Tex, TYPE_END};
public:
	typedef struct CBossIntroBG_DESC : public CUI::CUI_DESC
	{
	  _float interver{};

	}CBossIntroBG_DESC;

private:
	CBossIntroBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossIntroBG(const CBossIntroBG& Prototype);
	virtual ~CBossIntroBG() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*	    m_pTextureCom[TYPE_END] = {};
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	_float			m_interver{};
	CBossIntroBG_DESC IntroDesc[3];
	_float         m_TimeSum = { 0.f };

private:
	HRESULT Add_Components();
    HRESULT Set_Pos();
    void Set_IntroPos(void* pArg);

public:
	static CBossIntroBG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
