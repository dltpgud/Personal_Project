#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CPlayer_ShootingStateUI:  public CUI
{
public :
	enum ShootEF {Prrr,CLack,Taa,Tra, TaKa, CLack2, Flink, ShootEF_END};
public:
	typedef struct CShootingUI_DESC : public CUI::CUI_DESC
	{
       const _uint* iWeaPonState{};
		const _uint* iWeaPonTYPE{};
	}CShootingUI_DESC;

private:
	CPlayer_ShootingStateUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_ShootingStateUI(const CPlayer_ShootingStateUI& Prototype);
	virtual ~CPlayer_ShootingStateUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_RandomPos(_bool WeaponUP, _bool WeaponDown, _bool WeaponSide);
	void Set_PosClack(_float X, _float Y);
private:
	CTexture*	    m_pTextureCom [ShootEF_END] = {};
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	_uint m_iShootEF{};
	const _uint* m_iWeaPonState{};
	const _uint* m_iWeaPonTYPE{};
	_uint m_iTex{ 0 };
	_uint m_iTex2{ 0 };
	_bool m_bWeaponUP = true;
	_bool m_bWeaponDown = true;
	_bool m_bWeaponSide = true;
	_int m_iPass{-1};
	_float4 m_RGB{};
private:
	HRESULT Add_Components();

public:
	static CPlayer_ShootingStateUI*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
