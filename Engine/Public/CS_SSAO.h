#pragma once
#include "ComputeShader.h"
#include "VIBuffer_Rect.h"
BEGIN(Engine)
class CSSAO_ComputeShader final : public CComputeShader
{
private:
    CSSAO_ComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSSAO_ComputeShader(const CSSAO_ComputeShader& rhs);
    virtual ~CSSAO_ComputeShader() = default;

public:
    virtual  HRESULT Initialize_Proto() override;
    HRESULT DispatchSSAO(const _wstring& strDepthTargetTag, const _wstring& strNormalTargetTag);
    virtual HRESULT Bind_SRV(class CShader* pShader, const _char* pConstantName) override;
 #ifdef _DEBUG
    HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
    {
        XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

        m_WorldMatrix._11 = fSizeX;
        m_WorldMatrix._22 = fSizeY;

        _uint iNumViewports = {1};

        D3D11_VIEWPORT ViewportDesc{};

        m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
        m_WorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
        m_WorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

        return S_OK;
    }
    HRESULT Render(CShader* pShader,   CVIBuffer_Rect* pVIBuffer)
    {
        if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
            return E_FAIL;

        if (FAILED(pShader->Bind_SRV("g_Texture", m_pFinalSRV)))
            return E_FAIL;

        pShader->Begin(0);

        pVIBuffer->Render();

        return S_OK;
    }
#endif

private:
    __declspec(align(16)) struct SSAO_CB
    {
        XMFLOAT4X4 View;     // 64 bytes
        XMFLOAT4X4 Proj;     // 64 bytes
        XMFLOAT4X4 ProjInv;  // 64 bytes
        _float4 ProjParams;  // 16 bytes
        UINT Width;          // 4
        UINT Height;         // 4
        float Radius;        // 4
        float Bias;          // 4  ← 여기까지 16바이트 경계
        float AOIntensity;   // 4
        _float3 _padding;     // 12 (GPU 정렬 보정)
    };


    XMFLOAT4X4 m_WorldMatrix{};

    ID3D11ComputeShader* m_pCS_SSAO = nullptr;
    ID3D11ComputeShader* m_pCS_BlurX = nullptr;
    ID3D11ComputeShader* m_pCS_BlurY = nullptr;

    // AO result (raw)
    ID3D11Texture2D* m_pAOTex = nullptr;
    ID3D11ShaderResourceView* m_pAOSRV = nullptr;
    ID3D11UnorderedAccessView* m_pAOUAV = nullptr;

    // Blur X temp
    ID3D11Texture2D* m_pBlurXTex = nullptr;
    ID3D11ShaderResourceView* m_pBlurXSRV = nullptr;
    ID3D11UnorderedAccessView* m_pBlurXUAV = nullptr;

    // Final (Blur Y)
    ID3D11Texture2D* m_pFinalTex = nullptr;
    ID3D11ShaderResourceView* m_pFinalSRV = nullptr; // ← 라이트에서 사용
    ID3D11UnorderedAccessView* m_pFinalUAV = nullptr;

    ID3D11Buffer* m_pCB_SSAO = nullptr;

    UINT m_Width = 0;
    UINT m_Height = 0;

public:
    static CSSAO_ComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};
END