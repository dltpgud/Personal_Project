#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct BOUND_AABB_DESC : public BOUND_DESC
	{
		_float3		vExtents;
	}BOUND_AABB_DESC;
private:
	CBounding_AABB();
	virtual ~CBounding_AABB() = default;


public:
	const BoundingBox* Get_Desc() const {
		return m_pBoundDesc;
	}


public:
	virtual HRESULT Initialize(const BOUND_DESC* pBoundDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pTargetBounding) override;
	virtual _bool RayIntersect(_vector RayPos, _vector RayDir, _float& fDis)override;
    virtual void Set_Info(BOUND_DESC* pBoundDesc) override;
    virtual _bool IsInside(const _float3& pos) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	/* 충돌체를 위한 데이터. */
	/* 충돌을 수행하려면 이 데이터들이 최소 월드 스페이스 가지는변환이 필요하다. */
	BoundingBox* m_pBoundDesc_Original = { nullptr };
	BoundingBox* m_pBoundDesc = { nullptr };

public:
	static CBounding_AABB* Create(const BOUND_DESC* pBoundDesc);
	virtual void Free() override;
};

END