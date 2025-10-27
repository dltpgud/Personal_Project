#pragma once
#include "Component.h"
#include "Shader.h"

BEGIN(Engine)
class ENGINE_DLL CComputeShader abstract : public CComponent
{
protected:
    CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CComputeShader(const CComputeShader& Proto);
    virtual ~CComputeShader() = default;

public:
    virtual HRESULT Initialize_Proto();
    virtual HRESULT Initialize(void* pArg) override;

  virtual HRESULT Bind_SRV(class CShader* pShader, const _char* pConstantName);

protected:
    HRESULT Create_CS(_wstring strFilePath, _string strEntryPoint, ID3D11ComputeShader** ppComputeShader);
    HRESULT Create_SRV(ID3D11Resource* pData, D3D11_SHADER_RESOURCE_VIEW_DESC* pSRVDesc);
    HRESULT Create_UAV(ID3D11Resource* pData, D3D11_UNORDERED_ACCESS_VIEW_DESC* pUAVDesc);

    virtual HRESULT Compute(UINT X, UINT Y = 1, UINT Z = 1);

protected:
    ID3D11ComputeShader* m_pComputeShader{};
    ID3D11UnorderedAccessView* m_pUAV{};
    ID3D11ShaderResourceView* m_pSRV{};

public:
    virtual void Free() override;
    virtual CComponent* Clone(void* pArg) override
    {
        return nullptr;
    };
};
END
