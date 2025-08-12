#pragma once

#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CShader;
class CModel;

END

BEGIN(Client)

class CShock final : public CSkill
{
public: 
	
	typedef struct CShock_DESC : public CSkill::Skill_DESC
	{
	}CShock_DESC;
private:
	CShock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShock(const CShock& Prototype);
	virtual ~CShock() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
        virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual void Dead_Rutine(_float fTimeDelta) override;
	virtual HRESULT Render() override;	

private:
	CModel*					    m_pModelCom = {nullptr};


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
;


	_float m_fScaleSpeed{ 0.f };
	_float m_fCurrentScale{ 0.f };
	_float4 m_RGB = { 1.f,1.f,0.f,1.f };
public:
	static CShock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END