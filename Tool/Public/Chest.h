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
private:
	CChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChest(const CChest& Prototype);
	virtual ~CChest() = default;

public:
	/* ���������� ȣ�� : ������ �ʿ��� ����� ���ſ� �۾����� �����Ѵ�.(��Ŷ, ���� �����) */
	virtual HRESULT Initialize_Prototype() override;

	/* ��Ŷ�̳� ���� ������� ���ؼ� �޾ƿ��� ���ϴ� �����鵵 �и��� �����Ѵ�. */
	/* �������� �����ϴ� �ʴ� �߰����� �ʱ�ȭ�� �ʿ��� ��� ȣ���Ѥ���. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;	

	virtual void Set_Model(const _wstring& protoModel) override;
	virtual CModel* Get_Model() override { return m_pModelCom; }
	virtual _wstring Get_ComPonentName() override { return m_wModel; }
	virtual _tchar* Get_ProtoName() override;
	virtual void Set_Buffer(_uint x, _uint y) override { m_WeaPon = y; };

	virtual _uint  Get_Scalra()override {return m_WeaPon;}
	virtual _float check_BoxDist(_vector RayPos, _vector RayDir)override ;
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