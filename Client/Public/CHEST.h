#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CInteractiveUI;
class CPlayer;
class CCHEST final : public CGameObject
{
public : 
	enum chestType { ANI, NONANI, CHEST_END};
	enum State { IDLE, HOVDER, OPEN};
private:
	CCHEST(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCHEST(const CCHEST& Prototype);
	virtual ~CCHEST() = default;

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
	virtual void Set_Model(const _wstring& protoModel, _uint ILevel) override;
	virtual void Set_Buffer(_uint x, _uint y)override;

	void Interactive(_float fTimeDelta);
	
private:
	CShader*					m_pShaderCom[CHEST_END] = {nullptr};
	CModel*						m_pModelCom [CHEST_END] = {nullptr};
	_bool						m_bOpen = { false };
	_bool						m_bIcon = { false };
	_bool						m_bHover = { false };
	_bool						m_bState = { false };
	_uint						m_pWeaPonType{};



	CInteractiveUI* m_InteractiveUI ;
	CPlayer* m_pPlayer;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources(_int i);


public:
	static CCHEST* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END