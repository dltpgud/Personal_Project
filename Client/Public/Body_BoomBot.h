#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)

END

BEGIN(Client)
class CBody_BoomBot : public CPartObject
{
public: 
	typedef struct CBody_BoomBot_Desc : CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
     } CBody_BoomBot_Desc;

private:
     CBody_BoomBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_BoomBot(const CBody_BoomBot& Prototype);
     virtual ~CBody_BoomBot() = default;

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

	void Set_HitAttackMotion(_bool motion) { m_bHitAttackMotion = motion; }
	_bool Get_HitAttackMotion() { return m_bHitAttackMotion; }
private:

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_bool m_bHitAttackMotion = false;
public:
        static CBody_BoomBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END