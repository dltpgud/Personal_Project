#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CActor;
END

BEGIN(Client)
class CBody_BillyBoom : public CPartObject
{
public: 
	typedef struct CBody_BillyBoom_Desc : CPartObject::PARTOBJECT_DESC
    {
        class CActor* pParentObj;

    } CBody_BillyBoom_Desc;

    enum BONE_MAT
    {
		BM_LEFT_TOP, BM_RIGHT_TOP, BM_HAND, BM_END
    };

private:
	CBody_BillyBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_BillyBoom(const CBody_BillyBoom& Prototype);
     virtual ~CBody_BillyBoom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void SetDir();
    _vector Get_Dir() { return m_AttackDir;}
	void ChangeState(_int nextState);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
    HRESULT Set_StateMachine();

private:
	_float  m_fEmissivePower{};
	_int    m_iEmissiveMashNum{};
	_float3 m_fEmissiveColor{};
	_vector m_AttackDir{};

	const _float4x4* m_pFindAttBonMatrix[BM_END]{};
	_float4x4        m_HeandWorldMatrix{};

    class CActor* m_pParentObj{};
	CCollider* m_pHeanColl{};
    vector<class CStateMachine*> m_pStateMachine;

public:
    static CBody_BillyBoom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
