#pragma once
#include "Base.h"

BEGIN(Engine)

class CShader;
class CTexture;
class ENGINE_DLL CTrailGPU final : public CBase
{
public:
    struct TRAIL_CONSTANT
    {
        UINT iMaxPoint = 256;
        UINT Trail_NumBetween = 5;
        float Trail_LifeTime = 0.4f;
        XMFLOAT2 Trail_Scale = {0.02f, -0.02f};
    };

    struct EMIT_INFO
    {
        XMFLOAT4X4 matWorld;
    };

private:
    CTrailGPU(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CTrailGPU() = default;

public:
    HRESULT Initialize(const TRAIL_CONSTANT& constant, CTexture* pTexture);
    void Emit(const XMFLOAT4X4& matWorld);
    void Update(float fDeltaTime);
    void Render();
    void Reset();


private:
    HRESULT CreateConstantBuffers();
    HRESULT CreateComputeShaders();
    HRESULT CreateBuffers();
    UINT Get_UAVCounter(ID3D11UnorderedAccessView* pUAV);

public:
    static CTrailGPU* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TRAIL_CONSTANT& constant,
                              CTexture* pTexture =nullptr);
    virtual void Free() override;

private:
    // DX core
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // Engine resources
    CShader* m_pShader = nullptr;
    CTexture* m_pTexture = nullptr;

    // Compute Shaders
    ID3D11ComputeShader* m_pCS_Emit = nullptr;
    ID3D11ComputeShader* m_pCS_Update = nullptr;
    ID3D11ComputeShader* m_pCS_Reset = nullptr;

    // Constant Buffers
    ID3D11Buffer* m_pCB_Trail = nullptr;
    ID3D11Buffer* m_pCB_Emit = nullptr;

    // GPU Buffers
    ID3D11UnorderedAccessView* m_pRender_UAV = nullptr;
    ID3D11UnorderedAccessView* m_pUpdate_UAV = nullptr;
    ID3D11UnorderedAccessView* m_pAliveIndex_UAV = nullptr;
    ID3D11UnorderedAccessView* m_pDrawArgs_UAV = nullptr;

    ID3D11ShaderResourceView* m_pRender_SRV = nullptr;
    ID3D11ShaderResourceView* m_pAliveIndex_SRV = nullptr;

    TRAIL_CONSTANT m_Const{};
    UINT m_iMaxPoint = 0;
    UINT m_iLiveCount = 0;

   ID3D11Buffer* m_pRender_Buffer{};
    ID3D11Buffer* m_pUpdate_Buffer{};
   ID3D11Buffer* m_pDrawArgs_Buffer{};
    ID3D11Buffer* m_pAliveIndex_Buffer{};
   ID3D11Buffer* m_pCB_Reset{};
};
END