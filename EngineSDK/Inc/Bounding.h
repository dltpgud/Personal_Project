#pragma once

#include "Base.h"
#include "DebugDraw.h"
#include "Collider.h"

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
	virtual HRESULT Initialize(const BOUND_DESC* pBoundDesc);
	virtual void Update(_fmatrix WorldMatrix) {} // �浹�� ���� ������(����)�� ����� ��ȯ�Ѵ�. 
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pTargetBounding) = 0;
	virtual _bool RayIntersect(_vector RayPos, _vector RayDir, _float& fDis) = 0;
	virtual _float Get_iCurRadius() { return 0.f; }
	virtual _float3 Get_iCurCenter() { return _float3(); }
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) { return S_OK; }
#endif

public:
	virtual void Free() override;
};

END