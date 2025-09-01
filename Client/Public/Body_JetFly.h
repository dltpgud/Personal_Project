#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)

END

BEGIN(Client)
class CBody_JetFly : public CPartObject
{
public: 
	typedef struct CBody_JetFly_Desc : CPartObject::PARTOBJECT_DESC
    {
        class CMonster* pParentObj;
	}CBody_JetFly_Desc;

private:
	CBody_JetFly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_JetFly(const CBody_JetFly& Prototype);
	virtual ~CBody_JetFly() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
    void ChangeState(_int nextState);

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();
    HRESULT Set_StateMachine();

private:
    vector<class CStateMachine*> m_pStateMachine;
    class CMonster* m_pParentObj;

public:
	static CBody_JetFly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END