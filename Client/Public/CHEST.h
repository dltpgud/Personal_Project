#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CInteractiveUI;
class CCHEST final : public CGameObject
{
public : 
	enum State { IDLE, HOVDER, OPEN};
    enum FLAG : _ubyte
    {
        HOVER = 1 << 0, 
        ICON = 1 << 1   
    };

private:
	CCHEST(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCHEST(const CCHEST& Prototype);
	virtual ~CCHEST() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual void Set_Model(const _wstring& protoModel, _uint ILevel) override;
	
private:
	CShader*					m_pShaderCom = {nullptr};
	CModel*						m_pModelCom = {nullptr};
    _ubyte                      m_flags = 0; 
	_uint						m_pWeaPonType{};
    _uint                       m_iBoneIndex;
	CInteractiveUI* m_InteractiveUI{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
    HRESULT Init_CallBakc();
public:
	static CCHEST* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END