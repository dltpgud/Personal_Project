#include "VIBuffer_Trail.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"

CTrailGPU::CTrailGPU(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : m_pDevice(pDevice), m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CTrailGPU::Initialize(const TRAIL_CONSTANT& constant, CTexture* pTexture)
{
    m_Const = constant;
    m_iMaxPoint = constant.iMaxPoint;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail_Curved.hlsl"),
                                VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    m_pTexture = pTexture;

    Safe_AddRef(m_pShader);
    Safe_AddRef(m_pTexture);

    if (FAILED(CreateConstantBuffers()))
        return E_FAIL;
    if (FAILED(CreateComputeShaders()))
        return E_FAIL;
    if (FAILED(CreateBuffers()))
        return E_FAIL;
    return S_OK;
}

void CTrailGPU::Emit(const XMFLOAT4X4& matWorld)
{
    if (!m_pCS_Emit)
        return;

    // b2 업데이트
    D3D11_MAPPED_SUBRESOURCE mapped{};
    m_pContext->Map(m_pCB_Emit, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &matWorld, sizeof(XMFLOAT4X4));
    m_pContext->Unmap(m_pCB_Emit, 0);

    // UAV 바인딩
    ID3D11UnorderedAccessView* uavs[4] = {m_pRender_UAV, m_pUpdate_UAV, m_pAliveIndex_UAV, m_pDrawArgs_UAV};

    UINT initialCounts[4] = {UINT(-1), UINT(-1), UINT(-1), UINT(-1)};
    m_pContext->CSSetUnorderedAccessViews(0, 4, uavs, initialCounts);

    // CB 바인딩
    m_pContext->CSSetConstantBuffers(1, 1, &m_pCB_Trail);
    m_pContext->CSSetConstantBuffers(2, 1, &m_pCB_Emit);

    // 실행
    m_pContext->CSSetShader(m_pCS_Emit, nullptr, 0);
    m_pContext->Dispatch(1, 1, 1);

    // 정리
    ID3D11UnorderedAccessView* nullUAVs[4] = {nullptr, nullptr, nullptr, nullptr};
    m_pContext->CSSetUnorderedAccessViews(0, 4, nullUAVs, nullptr);
}

void CTrailGPU::Update(float fDeltaTime)
{
    if (!m_pCS_Update)
        return;

    // UAV 바인딩 (여기서는 카운터 리셋하지 않음)
    ID3D11UnorderedAccessView* uavs[4] = {m_pRender_UAV, m_pUpdate_UAV, m_pAliveIndex_UAV, m_pDrawArgs_UAV};
    UINT initialCounts[4] = {UINT(-1), UINT(-1), UINT(-1), UINT(-1)};
    m_pContext->CSSetUnorderedAccessViews(0, 4, uavs, initialCounts);

    // 상수 버퍼
    m_pContext->CSSetConstantBuffers(1, 1, &m_pCB_Trail);

    // 실행
    m_pContext->CSSetShader(m_pCS_Update, nullptr, 0);
    const UINT groups = (m_iMaxPoint + 255) / 256;
    m_pContext->Dispatch(groups, 1, 1);
    m_pContext->Flush();
    // 라이브 개수 읽기 (CopyStructureCount)
    m_iLiveCount = Get_UAVCounter(m_pAliveIndex_UAV);

    // 언바인드
    ID3D11UnorderedAccessView* nullUAVs[4] = {nullptr, nullptr, nullptr, nullptr};
    m_pContext->CSSetUnorderedAccessViews(0, 4, nullUAVs, nullptr);

}

UINT CTrailGPU::Get_UAVCounter(ID3D11UnorderedAccessView* pUAV)
{
    if (!pUAV)
        return 0;

    // 1️⃣ GPU → CPU 복사를 위한 임시 ReadBack Buffer 생성
    ID3D11Buffer* pCounterBuffer = nullptr;

    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    desc.ByteWidth = sizeof(UINT);

    if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, &pCounterBuffer)))
        return 0;

    // 2️⃣ UAV Counter 데이터를 ReadBack Buffer로 복사
    //    → GPU에서 UAV Counter 값을 CPU가 읽을 수 있는 메모리로 옮김
    m_pContext->CopyStructureCount(pCounterBuffer, 0, pUAV);

    // 3️⃣ CPU에서 Counter 값을 읽기
    D3D11_MAPPED_SUBRESOURCE mapped{};
    UINT result = 0;

    if (SUCCEEDED(m_pContext->Map(pCounterBuffer, 0, D3D11_MAP_READ, 0, &mapped)))
    {
        memcpy(&result, mapped.pData, sizeof(UINT));
        m_pContext->Unmap(pCounterBuffer, 0);
    }

    Safe_Release(pCounterBuffer);
    return result;
}


void CTrailGPU::Render()
{
    if (m_iLiveCount < 4)
        return;
    if (!m_pShader)
        return;

    m_pShader->Bind_RawValue("g_TrailConst", &m_Const, sizeof(TRAIL_CONSTANT));
    if (m_pTexture)
        m_pTexture->Bind_ShaderResource(m_pShader, "g_TrailTex",0);

    ID3D11ShaderResourceView* srvs[] = {m_pRender_SRV, m_pAliveIndex_SRV};
    m_pContext->VSSetShaderResources(0, 2, srvs);

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_pShader->Begin(0);
    m_pContext->Draw(m_iLiveCount & (~1), 0);

    ID3D11ShaderResourceView* nullSRVs[2] = {nullptr, nullptr};
    m_pContext->VSSetShaderResources(0, 2, nullSRVs);
}


HRESULT CTrailGPU::CreateConstantBuffers()
{
    // b1: TrailConstant
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = (sizeof(TRAIL_CONSTANT) + 15) / 16 * 16;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = &m_Const;

    if (FAILED(m_pDevice->CreateBuffer(&desc, &init, &m_pCB_Trail)))
        return E_FAIL;

    // b2: Emit용
    D3D11_BUFFER_DESC emitDesc{};
    emitDesc.Usage = D3D11_USAGE_DYNAMIC;
    emitDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    emitDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    emitDesc.ByteWidth = (sizeof(XMFLOAT4X4) + 15) / 16 * 16;
    if (FAILED(m_pDevice->CreateBuffer(&emitDesc, nullptr, &m_pCB_Emit)))
        return E_FAIL;

     D3D11_BUFFER_DESC desc1{};
    desc1.Usage = D3D11_USAGE_DEFAULT;
     desc1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc1.ByteWidth = 16; // Reset_iMaxPoint + pad
    if (FAILED(m_pDevice->CreateBuffer(&desc1, nullptr, &m_pCB_Reset)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrailGPU::CreateComputeShaders()
{
    ID3DBlob* pBlob = nullptr;
    ID3DBlob* pError = nullptr;

    auto CompileCS = [&](const wchar_t* file, const char* entry, ID3D11ComputeShader** outShader)
    {
        HRESULT hr = D3DCompileFromFile(file, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, "cs_5_0", 0, 0, &pBlob,
                                        &pError);
        if (FAILED(hr))
        {
            if (pError)
            {
                OutputDebugStringA((char*)pError->GetBufferPointer());
                pError->Release();
            }
            return hr;
        }
        hr = m_pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, outShader);
        Safe_Release(pBlob);
        return hr;
    };

    CompileCS(L"../Bin/ShaderFiles/CS_Curved_Trail_Ready.hlsl", "CS_EMIT", &m_pCS_Emit);
    CompileCS(L"../Bin/ShaderFiles/CS_Curved_Trail_Update.hlsl", "CS_UPDATE", &m_pCS_Update);
    CompileCS(L"../Bin/ShaderFiles/CS_Curved_Trail_Reset.hlsl", "CS_RESET", &m_pCS_Reset);

    return S_OK;
}

HRESULT CTrailGPU::CreateBuffers()
{
        // ================================
        // [1] RenderBuffer : ForRender { float3 vViewPosition }
        // ================================
        {
            D3D11_BUFFER_DESC desc{};
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
            desc.ByteWidth = sizeof(float) * 3 * m_iMaxPoint;
            desc.StructureByteStride = sizeof(float) * 3;
            desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

            if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, &m_pRender_Buffer)))
                return E_FAIL;

            D3D11_SHADER_RESOURCE_VIEW_DESC srv{};
            srv.Format = DXGI_FORMAT_UNKNOWN;
            srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            srv.Buffer.ElementOffset = 0;
            srv.Buffer.ElementWidth = m_iMaxPoint;

            if (FAILED(m_pDevice->CreateShaderResourceView(m_pRender_Buffer, &srv, &m_pRender_SRV)))
                return E_FAIL;

            D3D11_UNORDERED_ACCESS_VIEW_DESC uav{};
            uav.Format = DXGI_FORMAT_UNKNOWN;
            uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uav.Buffer.FirstElement = 0;
            uav.Buffer.NumElements = m_iMaxPoint;

            if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pRender_Buffer, &uav, &m_pRender_UAV)))
                return E_FAIL;
        }

        // ================================
        // [2] UpdateBuffer : ForUpdate { float3 vWorldPos; float3 vWorldUp; float2 vLife; bool bUp; }
        // ================================
        {
            const UINT strideUpdate = sizeof(float) * 3   // vWorldPos
                                      + sizeof(float) * 3 // vWorldUp
                                      + sizeof(float) * 2 // vLifeTime
                                      + sizeof(UINT);     // bool bUp(패딩 고려)

            D3D11_BUFFER_DESC desc{};
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
            desc.ByteWidth = strideUpdate * m_iMaxPoint;
            desc.StructureByteStride = strideUpdate;
            desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

            if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, &m_pUpdate_Buffer)))
                return E_FAIL;

            D3D11_UNORDERED_ACCESS_VIEW_DESC uav{};
            uav.Format = DXGI_FORMAT_UNKNOWN;
            uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uav.Buffer.FirstElement = 0;
            uav.Buffer.NumElements = m_iMaxPoint;

            if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pUpdate_Buffer, &uav, &m_pUpdate_UAV)))
                return E_FAIL;
        }

        // ================================
        // [3] AliveIndexBuffer : RWStructuredBuffer<uint> (UAV Counter + SRV)
        // ================================
        {
            D3D11_BUFFER_DESC desc{};
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
            desc.ByteWidth = sizeof(UINT) * m_iMaxPoint;
            desc.StructureByteStride = sizeof(UINT);
            desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

            if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, &m_pAliveIndex_Buffer)))
                return E_FAIL;

            D3D11_SHADER_RESOURCE_VIEW_DESC srv{};
            srv.Format = DXGI_FORMAT_UNKNOWN;
            srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            srv.Buffer.ElementOffset = 0;
            srv.Buffer.ElementWidth = m_iMaxPoint;

            if (FAILED(m_pDevice->CreateShaderResourceView(m_pAliveIndex_Buffer, &srv, &m_pAliveIndex_SRV)))
                return E_FAIL;

            D3D11_UNORDERED_ACCESS_VIEW_DESC uav{};
            uav.Format = DXGI_FORMAT_UNKNOWN;
            uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uav.Buffer.FirstElement = 0;
            uav.Buffer.NumElements = m_iMaxPoint;
            uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER; // <-- Counter 활성화

            if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pAliveIndex_Buffer, &uav, &m_pAliveIndex_UAV)))
                return E_FAIL;
        }

        // ================================
        // [4] DrawArgsBuffer : RWBuffer<uint> (5개 값)
        // ================================
        {
            D3D11_BUFFER_DESC desc{};
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
            desc.ByteWidth = sizeof(UINT) * 5;
            desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS; // 간접 드로우에도 사용 가능

            if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, &m_pDrawArgs_Buffer)))
                return E_FAIL;

            D3D11_UNORDERED_ACCESS_VIEW_DESC uav{};
            uav.Format = DXGI_FORMAT_R32_UINT;
            uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uav.Buffer.NumElements = 5;
            uav.Buffer.FirstElement = 0;

            if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pDrawArgs_Buffer, &uav, &m_pDrawArgs_UAV)))
                return E_FAIL;
        }

    return S_OK;
}

void CTrailGPU::Reset()
{
 
        if (!m_pCS_Reset)
            return;

        // UAV 카운터 초기화: u2만 0으로
        ID3D11UnorderedAccessView* uavs[4] = {m_pRender_UAV, m_pUpdate_UAV, m_pAliveIndex_UAV, m_pDrawArgs_UAV};
        UINT initCounts[4] = {UINT(-1), UINT(-1), 0, UINT(-1)};
        m_pContext->CSSetUnorderedAccessViews(0, 4, uavs, initCounts);

        // Reset용 CB(b0) 업데이트 (필요 시 미리 만든 m_pCB_Reset 사용)
        struct
        {
            UINT Reset_iMaxPoint;
            UINT pad[3];
        } resetData = {m_iMaxPoint, {0, 0, 0}};
        m_pContext->UpdateSubresource(m_pCB_Reset, 0, nullptr, &resetData, 0, 0);
        m_pContext->CSSetConstantBuffers(0, 1, &m_pCB_Reset);

        m_pContext->CSSetShader(m_pCS_Reset, nullptr, 0);
        const UINT groups = (m_iMaxPoint + 255) / 256;
        m_pContext->Dispatch(groups, 1, 1);

        // 언바인드
        ID3D11UnorderedAccessView* nullUAVs[4] = {nullptr, nullptr, nullptr, nullptr};
        m_pContext->CSSetUnorderedAccessViews(0, 4, nullUAVs, nullptr);

        // 카운터 0이므로 다음 프레임 Render 방지를 위해 캐시도 0으로
        m_iLiveCount = 0;
   
}

CTrailGPU* CTrailGPU::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TRAIL_CONSTANT& constant,
                              CTexture* pTexture)
{
    CTrailGPU* pInstance = new CTrailGPU(pDevice, pContext);
    if (FAILED(pInstance->Initialize(constant, pTexture)))
    {
        MSG_BOX("Failed to Create: CTrailGPU");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTrailGPU::Free()
{
    Safe_Release(m_pShader);
    Safe_Release(m_pTexture);

    Safe_Release(m_pCB_Trail);
    Safe_Release(m_pCB_Emit);

    Safe_Release(m_pCS_Emit);
    Safe_Release(m_pCS_Update);
    Safe_Release(m_pCS_Reset);

    Safe_Release(m_pRender_UAV);
    Safe_Release(m_pUpdate_UAV);
    Safe_Release(m_pAliveIndex_UAV);
    Safe_Release(m_pDrawArgs_UAV);

    Safe_Release(m_pRender_SRV);
    Safe_Release(m_pAliveIndex_SRV);

    
   Safe_Release(m_pRender_Buffer);
   Safe_Release(m_pUpdate_Buffer);
   Safe_Release(m_pDrawArgs_Buffer);
   Safe_Release(m_pAliveIndex_Buffer);
   Safe_Release(m_pCB_Reset);
   
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
