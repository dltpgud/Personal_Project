#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectRoop final : public CVIBuffer
{
private:
	CVIBuffer_RectRoop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_RectRoop(const CVIBuffer_RectRoop& Prototype);
	virtual ~CVIBuffer_RectRoop() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

private:
	_uint					m_iNumVerticesX = {};
	_uint					m_iNumVerticesZ = {};
public:
	static CVIBuffer_RectRoop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*    Clone(void* pArg) override;
	virtual void		   Free() override;
};

END
