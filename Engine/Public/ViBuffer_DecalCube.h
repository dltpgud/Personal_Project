#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_DecalCube final : public CVIBuffer_Instancing
{
private:
    CVIBuffer_DecalCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_DecalCube(const CVIBuffer_DecalCube& Prototype);
    virtual ~CVIBuffer_DecalCube() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
        void Create_InstanceBuffer(UINT maxCount);
    void Update(const vector<DecalInstanceData>& m_InstanceData);

private:
     _uint m_iMaxInstances = 10000;                


public:
     static CVIBuffer_DecalCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END