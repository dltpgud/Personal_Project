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