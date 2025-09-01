#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CShootEffect final : public CUI
{
public :
	enum STATE { NOMAL, BOW, ST_END };
public: 
	
	typedef struct CShootEffect_DESC : public CUI::CUI_DESC
	{
		_vector vPos{};
		_vector vTgetPos{};
		_vector Local{};
	    const _float4x4* WorldPtr{};
		_uint iWeaponType{};
	}CShootEffect_DESC;
private:
	CShootEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShootEffect(const CShootEffect& Prototype);
	virtual ~CShootEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
        virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;	

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = {nullptr};
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	_float						m_fFrame = { 0.f };
	_float4 m_RGB[2]{};
	const _float4x4* m_WorldPtr ={nullptr};
	_vector m_Local{};
	_vector m_vTget{};
	_vector m_vPos{};
	_uint m_iWeaponType{};
	_uint m_iPass{};
	
public:
	static CShootEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END