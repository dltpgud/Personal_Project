#include "CS_SSAO.h"
#include "Shader.h"
#include "GameInstance.h"

CSSAO_ComputeShader::CSSAO_ComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComputeShader(pDevice, pContext)
{
}

HRESULT CSSAO_ComputeShader::Initialize_Proto()
{
    m_Width = 1920.f / 2;
    m_Height = 1080.f / 2;

    //=============================
    // 1. Compute Shaders
    //=============================
    if (FAILED(Create_CS(L"../Bin/ShaderFiles/SSAO_Compute.hlsl", "CS_SSAO", &m_pCS_SSAO)))
        return E_FAIL;
    if (FAILED(Create_CS(L"../Bin/ShaderFiles/SSAO_Compute.hlsl", "CS_BlurX", &m_pCS_BlurX)))
        return E_FAIL;
    if (FAILED(Create_CS(L"../Bin/ShaderFiles/SSAO_Compute.hlsl", "CS_BlurY", &m_pCS_BlurY)))
        return E_FAIL;

    //=============================
    // 2. AO / Blur Textures
    //=============================
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = m_Width;
    desc.Height = m_Height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = desc.Format;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    // AO Output
    if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pAOTex)))
        return E_FAIL;
    if (FAILED(m_pDevice->CreateShaderResourceView(m_pAOTex, &srvDesc, &m_pAOSRV)))
        return E_FAIL;
    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pAOTex, &uavDesc, &m_pAOUAV)))
        return E_FAIL;

    // Blur X Temp
    if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pBlurXTex)))
        return E_FAIL;
    if (FAILED(m_pDevice->CreateShaderResourceView(m_pBlurXTex, &srvDesc, &m_pBlurXSRV)))
        return E_FAIL;
    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pBlurXTex, &uavDesc, &m_pBlurXUAV)))
        return E_FAIL;

    // Final AO (after BlurY)
    if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pFinalTex)))
        return E_FAIL;
    if (FAILED(m_pDevice->CreateShaderResourceView(m_pFinalTex, &srvDesc, &m_pFinalSRV)))
        return E_FAIL;
    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pFinalTex, &uavDesc, &m_pFinalUAV)))
        return E_FAIL;

    //=============================
    // 3. Constant Buffer
    //=============================
    D3D11_BUFFER_DESC cbDesc{};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(SSAO_CB);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    if (FAILED(m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pCB_SSAO)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSSAO_ComputeShader::DispatchSSAO(const _wstring& strDepthTargetTag, const _wstring& strNormalTargetTag)
{
    const _float4x4* pView = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
    const _float4x4* pProj = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
    const _float4x4* pProjInv = m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ);

    _float fFar = *m_pGameInstance->Get_CamFar();
    _float fNear = *m_pGameInstance->Get_CamNear();
    _float4 ProjParams = {1.f / fFar, fFar, fNear, 0.f};

    // Constant buffer 업데이트
    SSAO_CB cb{};
    cb.View = *pView;
    cb.Proj = *pProj;
    cb.ProjInv = *pProjInv;
    cb.ProjParams = ProjParams;
    cb.Width = m_Width;
    cb.Height = m_Height;
    cb.Radius = 0.15f;
    cb.Bias = 0.02f;
    cb.AOIntensity = 0.6f;
    m_pContext->UpdateSubresource(m_pCB_SSAO, 0, nullptr, &cb, 0, 0);

    ID3D11ShaderResourceView* depthSRV = m_pGameInstance->Get_SRV(strDepthTargetTag);
    ID3D11ShaderResourceView* normalSRV = m_pGameInstance->Get_SRV(strNormalTargetTag);

    UINT gx = (m_Width + 15) / 16;
    UINT gy = (m_Height + 15) / 16;

    auto UnbindAll = [&]()
    {
        ID3D11UnorderedAccessView* nullUAV[3] = {nullptr, nullptr, nullptr};
        ID3D11ShaderResourceView* nullSRV[4] = {nullptr, nullptr, nullptr, nullptr};
        m_pContext->CSSetUnorderedAccessViews(0, 3, nullUAV, nullptr);
        m_pContext->CSSetShaderResources(0, 4, nullSRV);
        m_pContext->CSSetShader(nullptr, nullptr, 0);
    };

    //=============================
    // 1️⃣ SSAO 계산 패스
    //=============================
    {
        ID3D11ShaderResourceView* SRVs[2] = {depthSRV, normalSRV};
        m_pContext->CSSetShader(m_pCS_SSAO, nullptr, 0);
        m_pContext->CSSetConstantBuffers(0, 1, &m_pCB_SSAO);
        m_pContext->CSSetShaderResources(0, 2, SRVs);
        m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pAOUAV, nullptr);
        m_pContext->Dispatch(gx, gy, 1);
        UnbindAll();
    }

    //=============================
    // 2️⃣ Blur X 패스
    //=============================
    {
        m_pContext->CSSetShader(m_pCS_BlurX, nullptr, 0);
        m_pContext->CSSetConstantBuffers(0, 1, &m_pCB_SSAO);
        m_pContext->CSSetShaderResources(2, 1, &m_pAOSRV);
        m_pContext->CSSetUnorderedAccessViews(1, 1, &m_pBlurXUAV, nullptr);
        m_pContext->Dispatch(gx, gy, 1);
        UnbindAll();
    }

    //=============================
    // 3️⃣ Blur Y 패스
    //=============================
    {
        m_pContext->CSSetShader(m_pCS_BlurY, nullptr, 0);
        m_pContext->CSSetConstantBuffers(0, 1, &m_pCB_SSAO);
        m_pContext->CSSetShaderResources(3, 1, &m_pBlurXSRV);
        m_pContext->CSSetUnorderedAccessViews(2, 1, &m_pFinalUAV, nullptr);
        m_pContext->Dispatch(gx, gy, 1);
        UnbindAll();
    }

    return S_OK;
}

//=============================
// SSAO 최종 결과를 다른 셰이더에 바인딩
//=============================
HRESULT CSSAO_ComputeShader::Bind_SRV(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_SRV(pConstantName, m_pFinalSRV);
}

CSSAO_ComputeShader* CSSAO_ComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSSAO_ComputeShader* pInstance = new CSSAO_ComputeShader(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Proto()))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CSSAO_ComputeShader::Free()
{
    __super::Free();

    Safe_Release(m_pCB_SSAO);

    Safe_Release(m_pCS_SSAO);
    Safe_Release(m_pCS_BlurX);
    Safe_Release(m_pCS_BlurY);

    Safe_Release(m_pAOTex);
    Safe_Release(m_pAOSRV);
    Safe_Release(m_pAOUAV);

    Safe_Release(m_pBlurXTex);
    Safe_Release(m_pBlurXSRV);
    Safe_Release(m_pBlurXUAV);

    Safe_Release(m_pFinalTex);
    Safe_Release(m_pFinalSRV);
    Safe_Release(m_pFinalUAV);
}
