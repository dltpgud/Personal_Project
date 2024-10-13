#pragma once

#include "Base.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct
	{
		_float3		vCenter;
	}BOUND_DESC;

protected:
	CBounding();
	virtual ~CBounding() = default;

public:
	/* 콜라이더의 사본을 생성할 때, 호출한다. */
	virtual HRESULT Initialize(void* pArg);

public:	
	virtual void Free() override;
};

END