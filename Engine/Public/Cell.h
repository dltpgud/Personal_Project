#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
    enum LINE   { LINE_AB, LINE_BC, LINE_CA, LINE_END};

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;
	

public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighbors[eLine] = pNeighbor->m_iIndex;  // 라인에 따른 인덱스 설정..
	}

public:
	HRESULT Initialize(const _float3* pPoints, _uint iIndex);
    _bool isIn(_fvector vLocalPos, _int* pNeighborIndex);
	_bool Compare_Points(_fvector vSour, _fvector vDest);
#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif


private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_float3					m_vPoints[POINT_END]   = {};
	_uint					m_iIndex = {};
	_int					m_iNeighbors[LINE_END] = { -1, -1, -1 };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*		m_pVIBuffer = { nullptr };	
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END