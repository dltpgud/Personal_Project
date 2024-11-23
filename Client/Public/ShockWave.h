#pragma once

#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CShader;
class CModel;

END

BEGIN(Client)

class CShockWave final : public CSkill
{
public: 
	
	typedef struct CShockWave_DESC : public CSkill::Skill_DESC
	{
	}CShockWave_DESC;
private:
	CShockWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShockWave(const CShockWave& Prototype);
	virtual ~CShockWave() = default;

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

private:
	CModel*					    m_pModelCom = {nullptr};


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
;


	_float m_fScaleSpeed{ 0.f };
	_float m_fCurrentScale{ 0.f };

	_float m_fTimedelta{ 0.f };
	_float m_RingRadius = { 3.f };
public:
	static CShockWave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END