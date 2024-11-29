#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)

END

BEGIN(Client)
class CBody_BillyBoom : public CPartObject
{
public: 
	typedef struct CBody_BillyBoom_Desc : CPartObject::PARTOBJECT_DESC
	{
		CGameObject* pPickeObj{};
    } CBody_BillyBoom_Desc;

private:
	CBody_BillyBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_BillyBoom(const CBody_BillyBoom& Prototype);
     virtual ~CBody_BillyBoom() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;



	HRESULT Make_Barre( );
	HRESULT Make_ShockWave( );
	HRESULT Make_Laser();
	void SetDir();



private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
private:
	

	_float m_fAttackTime{ 0.f };
	_float m_pDamage = { 4.f };
	CGameObject* m_pParent = nullptr;
	
	_float m_fEmissivePower = false;
	_bool	m_bEmissive = false;
	_uint m_iEmissiveMashNum{ 0 };
	_float3 m_fEmissiveColor{};
	_bool m_bEmissiveStart = false;
	_float m_fTimeSum{ 0.f };

	_bool m_bBerrle = false;

	_vector m_AttackDir{};


	_bool m_bTurnBeam = false;
	_float m_BeamX{};
	_float m_BeamY{-120.f};
	_float m_BeamZ{ 35.f };

	const _float4x4* m_pFindAttBonMatrix[4]{};



	_float4x4 m_HeandWorldMatrix{};

	CCollider* m_pHeanColl{ nullptr };
public:
    static CBody_BillyBoom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END