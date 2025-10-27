#pragma once

#include "Base.h"

BEGIN(Engine)

class CQuadTree final : public CBase
{
public:
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void Culling(class CGameInstance* pGameInstance, const _float3* pVerticesPos, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrixInv);
    _bool Picking_Ray(const _float3* pVerticesPos, _vector RayPos, _vector RayDir, _float& fBestDist, _float3& vHitPos, _float3& vHitNormal);
 
	_bool Intersect_Node(const BoundingSphere& sphere, const _float3* pVerticesPos, OUT _float3* pHitPos, OUT _float3* pHitNormal, OUT _float* pBestPenetration);
    _bool Intersect_Node(const BoundingBox& box, const _float3* pVerticesPos, OUT _float3* pHitPos, OUT _float3* pHitNormal, OUT _float* pBestPenetration);
    _bool Intersect_Node(const BoundingOrientedBox& obb, const _float3* pVerticesPos, OUT _float3* pHitPos, OUT _float3* pHitNormal, OUT _float* pBestPenetration);

    _bool TestSphereTriangle(const BoundingSphere& sphere, const _float3& a, const _float3& b, const _float3& c, OUT _float3* oHit, OUT _float3* oNormal, OUT _float* oPen);
    _bool TestAABBTriangle(const BoundingBox& box, const _float3& a, const _float3& b, const _float3& c,OUT _float3* oHit, OUT _float3* oNormal, OUT _float* oPen);
    _bool TestOBBTriangle(const BoundingOrientedBox& obb, const _float3& a, const _float3& b, const _float3& c, OUT _float3* oHit, OUT _float3* oNormal, OUT _float* oPen);

private:
	CQuadTree*				m_Children[CORNER_END] = { nullptr, nullptr, nullptr, nullptr };
	_uint					m_iCorners[CORNER_END] = { 0 };
	_uint					m_iCenter = {};

private:
	_bool isDraw(class CGameInstance* pGameInstance, const _float3* pVerticesPos, _fmatrix WorldMatrixInv);

public:
	static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free() override;
};

END