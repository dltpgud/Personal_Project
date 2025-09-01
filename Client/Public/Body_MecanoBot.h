#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)

END

BEGIN(Client)
class CBody_MecanoBot : public CPartObject
{
public: 
	typedef struct CBody_MecanoBot_Desc : CPartObject::PARTOBJECT_DESC
	{
            class CMonster* pParentObj;
     } CBody_MecanoBot_Desc;

private:
	CBody_MecanoBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_MecanoBot(const CBody_MecanoBot& Prototype);
     virtual ~CBody_MecanoBot() = default;

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
    class CMonster* m_pParentObj;
    vector<class CStateMachine*> m_pStateMachine ;
public:
    static CBody_MecanoBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END