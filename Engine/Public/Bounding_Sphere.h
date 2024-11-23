#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct BOUND_SPHERE_DESC : public BOUND_DESC
	{
		_float		fRadius;
	}BOUND_SPHERE_DESC;
private:
	CBounding_Sphere();
	virtual ~CBounding_Sphere() = default;

public:
	const BoundingSphere* Get_Desc() const {
		return m_pBoundDesc;
	}

public:
	virtual HRESULT Initialize(const BOUND_DESC* pBoundDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pTargetBounding) override;
	virtual _bool RayIntersect(_vector RayPos, _vector RayDir, _float& fDis)override;
	virtual _float Get_iCurRadius() override;
	virtual _float3 Get_iCurCenter() override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)override;
#endif

private:
	
	BoundingSphere*			m_pBoundDesc_Original = { nullptr };
	BoundingSphere*			m_pBoundDesc = { nullptr };
	_float                  m_iCurRadius = {0.f};
	_float3                 m_iCurCenter = {};
public:
	static CBounding_Sphere* Create(const BOUND_DESC* pBoundDesc);
	virtual void Free() override;
};

END