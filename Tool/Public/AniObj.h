#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Tool)

class CAniObj final : public CGameObject
{
private:
	CAniObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAniObj(const CAniObj& Prototype);
	virtual ~CAniObj() = default;

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
	virtual CTransform* Get_Transform();
	virtual CModel* Get_Model() override { return m_pModelCom; }
	virtual _wstring Get_ComPonentName() override { return m_wModel; }
	virtual _tchar* Get_ProtoName() override;
private:

	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_wstring m_wModel;
	_tchar* m_Proto;

public:
	static CAniObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END