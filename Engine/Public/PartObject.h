#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct PARTOBJECT_DESC : CGameObject::GAMEOBJ_DESC
	{
		const _float4x4*	pParentMatrix = { nullptr };
	}PARTOBJECT_DESC;
protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;

public:
	/* ���������� ȣ�� : ������ �ʿ��� ����� ���ſ� �۾����� �����Ѵ�.(��Ŷ, ���� �����) */
	virtual HRESULT Initialize_Prototype();

	/* ��Ŷ�̳� ���� ������� ���ؼ� �޾ƿ��� ���ϴ� �����鵵 �и��� �����Ѵ�. */
	/* �������� �����ϴ� �ʴ� �߰����� �ʱ�ȭ�� �ʿ��� ��� ȣ���Ѥ���. */
	virtual HRESULT Initialize(void* pArg);
	virtual _int Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	const _float4x4*				m_pParentMatrix = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END