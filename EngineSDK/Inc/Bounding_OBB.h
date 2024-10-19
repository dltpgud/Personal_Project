#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct BOUND_OBB_DESC : public BOUND_DESC
	{
		_float3		vExtents;
		_float3		vRotation;
	}BOUND_OBB_DESC;
private:
	CBounding_OBB();
	virtual ~CBounding_OBB() = default;

public:
	const BoundingOrientedBox* Get_Desc() const {
		return m_pBoundDesc;
	}


public:
	virtual HRESULT Initialize(const BOUND_DESC* pBoundDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pTargetBounding) override;
	virtual _bool RayIntersect(_vector RayPos, _vector RayDir, _float& fDis)override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:

	BoundingOrientedBox*			m_pBoundDesc_Original = { nullptr };
	BoundingOrientedBox*			m_pBoundDesc = { nullptr };

public:
	static CBounding_OBB* Create(const BOUND_DESC* pBoundDesc);
	virtual void Free() override;
};

END