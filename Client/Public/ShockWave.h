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

int type = 0;
	_float m_fScaleSpeed{ 0.f };
	_float m_fCurrentScale{ 0.f };
	_float4 m_RGB = { 0.95f,0.95f,0.f,1.f };
	_float m_fTime{ 0.f };
public:
	static CShockWave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END