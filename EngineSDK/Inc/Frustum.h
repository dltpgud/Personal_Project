#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();
	_bool isIn_WorldSpace(_fvector vTargetPos, _float fRange);
	_bool isIn_LocalSpace(_fvector vTargetPos, _float fRange = 0.f);
	void Transform_To_LocalSpace(_fmatrix WorldMatrixInv);
private:
	_float3					m_vPoints[8] = {};
	_float3					m_vWorldPoints[8] = {};
	_float4					m_WorldPlanes[6] = {};
	_float4					m_LocalPlanes[6] = {};

	class CGameInstance*	m_pGameInstance = { nullptr };

private:
	HRESULT Make_Planes(const _float3* pPoints, _float4* pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

END