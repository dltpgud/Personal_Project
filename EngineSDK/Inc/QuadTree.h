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

	struct ChildPick
    {
        CQuadTree* node;
        _float dist;
    };

public:
    HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB, const _float3* pVerticesPos);
	void Culling(const _float3* pVerticesPos, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrixInv);
    _bool Picking_Ray(const _float3* pVerticesPos, _vector RayPos, _vector RayDir, _float& fBestDist, _float3& vHitPos, _float3& vHitNormal);
    _bool RayIntersectsTriangle(_vector RayPos, _vector RayDir, const _float3& v0, const _float3& v1, const _float3& v2, _float& ioBestDist, _float3& outHitPos, _float3& outNormal);
       


private:
	CQuadTree*				m_Children[CORNER_END] = { nullptr, nullptr, nullptr, nullptr };
	_uint					m_iCorners[CORNER_END] = { 0 };
	_uint					m_iCenter = {};
    BoundingBox             m_Bounds; // 쿼드 트리 피킹용.  
    class CGameInstance*    m_pGameInstance{};

private:
	_bool isDraw(const _float3* pVerticesPos, _fmatrix WorldMatrixInv);

public:
        static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB, const _float3* pVerticesPos);
	virtual void Free() override;
};

END