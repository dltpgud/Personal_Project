#pragma once

#include "VIBuffer.h"

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct
	{
		/* 로컬상에 상태를 셋팅하낟. */
		_uint		iNumInstance;
		_float3		vCenter;
		_float3		vRange;
		_float2		vSize;
		_float2		vSpeed;
		_float2		vLifeTime;
		_float3		vPivot;
		_bool		isLoop;

	}INSTANCING_DESC;

protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& Prototype);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;
	virtual HRESULT Bind_Buffers() override;

public:
	void Spread(_float fTimeDelta);
	virtual void Drop(_float fTimeDelta);


protected:
	D3D11_BUFFER_DESC		m_InstanceBufferDesc = {};
	D3D11_SUBRESOURCE_DATA	m_InstanceInitialDesc = {};

	ID3D11Buffer*			m_pVBInstance = { nullptr };
	VTXMATRIX*				m_pInstanceVertices = { nullptr };

	_uint					m_iNumIndexPerInstance = {};
	_uint					m_iInstanceVertexStride = {};
	_uint					m_iNumInstance = { 0 };

	_float*					m_pSpeed = { nullptr };
	_float3					m_vPivot = {};
	_bool					m_isLoop = { false };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

