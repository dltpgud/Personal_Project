#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
 public:
	typedef struct CAMERA_DESC : CGameObject::GAMEOBJ_DESC
	{
		_float4					vEye{}, vAt{};
		_float					fFovy = { 0.0f };
		_float					fAspect = { 0.f };
		_float					fNearZ = { 0.f };
		_float					fFarZ = { 0.f };
	}CAMERA_DESC;

 protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

 public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _int Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

 protected:
	_float					m_fFovy		= { 0.f };
	_float					m_fAspect	= { 0.f };
	_float					m_fNearZ	= { 0.f };
	_float					m_fFarZ		= { 0.f };

 public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();
};
END
