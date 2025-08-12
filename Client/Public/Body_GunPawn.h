#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CActor;
END

BEGIN(Client)
class CBody_GunPawn : public CPartObject
{
public: 
	typedef struct BODY_GUNPAWN_DESC : CPartObject::PARTOBJECT_DESC
    {
        class CActor* pParentObj;
	}BODY_GUNPAWN_DESC;

private:
	CBody_GunPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_GunPawn(const CBody_GunPawn& Prototype);
	virtual ~CBody_GunPawn() = default;

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
    vector<class CStateMachine*> m_pStateMachine ;
    class CActor* m_pParentObj;
public:
	static CBody_GunPawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END