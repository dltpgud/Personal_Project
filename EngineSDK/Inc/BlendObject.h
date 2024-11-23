#pragma once

#include "GameObject.h"

/* ������Ÿ���� ���� ��ü�� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CGameObject
{
public:
	typedef struct BLEND_DESC  : public CGameObject::GAMEOBJ_DESC
	{		
	}BLEND_DESC;
protected:
	CBlendObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlendObject(const CBlendObject& Prototype);
	virtual ~CBlendObject() = default;

public:
	_float Get_Depth() const {
		return m_fDepth;
	}

public:
	/* ���������� ȣ�� : ������ �ʿ��� ����� ���ſ� �۾����� �����Ѵ�.(��Ŷ, ���� �����) */
	virtual HRESULT Initialize_Prototype() override;

	/* ��Ŷ�̳� ���� ������� ���ؼ� �޾ƿ��� ���ϴ� �����鵵 �и��� �����Ѵ�. */
	/* �������� �����ϴ� �ʴ� �߰����� �ʱ�ȭ�� �ʿ��� ��� ȣ���Ѥ���. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;

protected:
	_float					m_fDepth = {};

protected:
	void Compute_Depth();


	
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END