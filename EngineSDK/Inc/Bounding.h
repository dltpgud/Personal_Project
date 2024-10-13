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
	/* �ݶ��̴��� �纻�� ������ ��, ȣ���Ѵ�. */
	virtual HRESULT Initialize(void* pArg);

public:	
	virtual void Free() override;
};

END