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
        virtual HRESULT Bind_Buffers() override;
        virtual HRESULT Render() override;
    void Update(const vector<DecalInstanceData>& m_InstanceData);

        UINT m_iMaxInstances = 10000;                    // 버퍼 크기용 (고정)
        UINT m_iNumInstance = 0;                         // 이번 프레임 실제 개수
        std::vector<DecalInstanceData> m_InstanceUpload; // 업로드용 CPU 버퍼(고정)

    public:
     static CVIBuffer_DecalCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END