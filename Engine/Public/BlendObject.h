#pragma once

#include "GameObject.h"

/* 프로토타입을 통해 객체를 생성한다. */

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CGameObject
{
public:
	typedef struct BLEND_DESC  : public CGameObject::GAMEOBJ_DESC
	{		
	}BLEND_DESC;
protected:
	CBlendObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlendObject(const CBlendObject& Prototype);
	virtual ~CBlendObject() = default;

public:
	_float Get_Depth() const {
		return m_fDepth;
	}

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;

protected:
	_float					m_fDepth = {};

protected:
	void Compute_Depth();


	
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END