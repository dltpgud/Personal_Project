#pragma once
#include "Base.h"

BEGIN(Engine)
class CBone;
class CBoneBufferManager final : public CBase
{
private:
    CBoneBufferManager();
    virtual ~CBoneBufferManager() = default;

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, UINT maxInstances, UINT maxBones);

    void UploadBones(UINT instanceID, const vector<CBone*>& Bones, const vector<_float4x4>& OffsetMatrices);

    ID3D11ShaderResourceView* GetSRV()
    {
        return m_pSRV;
    }

    UINT GetMaxBones() const
    {
        return m_maxBones;
    }

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    ID3D11Buffer* m_pBuffer = nullptr;
    ID3D11ShaderResourceView* m_pSRV = nullptr;

    UINT m_maxInstances = 0;
    UINT m_maxBones = 0;


public:
    static CBoneBufferManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, UINT maxInstances,
                                      UINT maxBones);
    virtual void Free() override;
};
END
