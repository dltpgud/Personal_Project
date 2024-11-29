#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CBody_JetFly : public CPartObject
{
public: 
	typedef struct CBody_JetFly_Desc : CPartObject::PARTOBJECT_DESC
	{
	}CBody_JetFly_Desc;

private:
	CBody_JetFly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_JetFly(const CBody_JetFly& Prototype);
	virtual ~CBody_JetFly() = default;

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
	virtual HRESULT Render_Shadow() override;
	void Make_Bullet();
private:

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_float m_pDamage = {6.f};
public:
	static CBody_JetFly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END