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
	virtual HRESULT Initialize(void* pArg) override;

private:
	BoundingBox*			m_pBoundDesc = { nullptr };
	//BoundingBox*			m_pBoundDesc = { nullptr };

public:
	virtual void Free() override;
};

END