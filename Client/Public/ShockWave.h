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
	virtual void Dead_Rutine() override;
	virtual HRESULT Render() override;	
	virtual _bool Comput_SafeZone(_fvector vPlayerPos) override;

private:
	CModel*   m_pModelCom     = {};
    _float    m_fScaleSpeed   = {};
    _float    m_fCurrentScale = {};
    _float4   m_fColor        = {};
    _float    m_fTimeSum      = {};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CShockWave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END