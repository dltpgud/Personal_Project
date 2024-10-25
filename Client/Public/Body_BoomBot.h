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
		const _uint* pParentState = { nullptr };
     } CBody_BoomBot_Desc;

private:
     CBody_BoomBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_BoomBot(const CBody_BoomBot& Prototype);
     virtual ~CBody_BoomBot() = default;

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

	void Set_HitAttackMotion(_bool motion) { m_bHitAttackMotion = motion; }
	_bool Get_HitAttackMotion() { return m_bHitAttackMotion; }
private:

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_bool m_bHitAttackMotion = false;
public:
        static CBody_BoomBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END