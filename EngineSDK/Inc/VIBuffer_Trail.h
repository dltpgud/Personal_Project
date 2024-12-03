#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
private:
    CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Trail(const CVIBuffer_Trail& Prototype);
    virtual ~CVIBuffer_Trail() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);

public:
    static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END
