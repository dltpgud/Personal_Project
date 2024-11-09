#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)

END

BEGIN(Client)
class CBody_BillyBoom : public CPartObject
{
public: 
	typedef struct CBody_BillyBoom_Desc : CPartObject::PARTOBJECT_DESC
	{

     } CBody_BillyBoom_Desc;

private:
	CBody_BillyBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_BillyBoom(const CBody_BillyBoom& Prototype);
     virtual ~CBody_BillyBoom() = default;

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


	void Make_Bullet();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
private:
	

	_float m_fAttackTime{ 0.f };
	_float m_pDamage = { 4.f };



	const _float4x4* m_pFindBonMatrix[4]{};
public:
    static CBody_BillyBoom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END