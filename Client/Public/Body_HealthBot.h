#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CBody_HealthBot : public CPartObject
{
public: 
	typedef struct BODY_HEALTHBOT_DESC : CPartObject::PARTOBJECT_DESC
	{

	}BODY_HEALTHBOT_DESC;

private:
	CBody_HealthBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_HealthBot(const CBody_HealthBot& Prototype);
	virtual ~CBody_HealthBot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void    Update(_float fTimeDelta) override;
	virtual void    Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_HealthBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END