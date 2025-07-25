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

     } CBody_BoomBot_Desc;

private:
     CBody_BoomBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_BoomBot(const CBody_BoomBot& Prototype);
     virtual ~CBody_BoomBot() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
        virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	void Make_Bullet();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
private:
	
	_float m_fAttackTime{ 0.f };
	_float m_pDamage = { 5.f };

public:
        static CBody_BoomBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END