#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;

END

BEGIN(Tool)

class CChest final : public CGameObject
{
public:
    struct CChest_DESC : CGameObject::GAMEOBJ_DESC
    {
      
    };


private:
	CChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChest(const CChest& Prototype);
	virtual ~CChest() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;	
	virtual HRESULT Render_Shadow() override;
	virtual void Set_Model(const _wstring& protoModel, _uint ILevel) override;
	_wstring Get_ComPonentName(){ return m_wModel; }
	_tchar* Get_ProtoName() ;
    void Set_WeaPon(_uint Weapon) { m_WeaPon = Weapon;};

	_uint  Get_Weapon() {return  m_WeaPon;}

private:

	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_wstring m_wModel;
	_tchar* m_Proto;
	_int m_istate;
	_int m_WeaPon;
public:
 	static CChest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END