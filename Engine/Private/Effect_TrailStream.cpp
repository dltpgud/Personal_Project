#include "Effect_TrailStream.h"
#include "Shader.h"
#include "GameInstance.h"
static constexpr UINT THREADS = 64;

CEffect_TrailStream::CEffect_TrailStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEffectStream{pDevice, pContext}
{
}

//=====================================================================
// Initialize
//=====================================================================
HRESULT CEffect_TrailStream::Initialize(void* pArg)
{
    if (pArg)
        m_desc = *static_cast<TRAILSDESC*>(pArg);
  
    m_iPass = m_desc.iPass;
    m_vTrailTexUVScale = m_desc.vTrailTexUVScale;
    m_iMode = m_desc.Mode;
    if (m_desc.pTrailTexturePath)
    {
        m_pTrailTexCom = CTexture::Create(m_pDevice, m_pContext, m_desc.pTrailTexturePath, m_desc.iTextureNum);
        m_iTexTotalFrames = m_desc.iTotalSprite;
    }
    if (m_pTrailTexCom)
        Safe_AddRef(m_pTrailTexCom);

    if (FAILED(createShaders()))
        return E_FAIL;
    if (FAILED(createBuffers()))
        return E_FAIL;
    if (FAILED(createPerFrameCB()))
        return E_FAIL;
    if (FAILED(createIndirectArgs()))
        return E_FAIL;

    m_inUse.assign(m_desc.maxTrails, 0);
    m_GenerationTable.assign(m_desc.maxTrails, 0);
    m_NextTrailID = 0;
    return S_OK;
}
//=====================================================================
// Shader
//=====================================================================
HRESULT CEffect_TrailStream::createShaders()
{
    ID3DBlob* csBlob = nullptr;
    ID3DBlob* errBlob = nullptr;

    // Update CS
    HRESULT hr = D3DCompileFromFile(L"../Bin/ShaderFiles/TrailUpdateCS.hlsl", nullptr,
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE, "CSMain", "cs_5_0", 0, 0, &csBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob)
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
        return hr;
    }
    if (FAILED(m_pDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &m_pCS_Update)))
        return E_FAIL;
    Safe_Release(csBlob);

    // Interp CS
    hr = D3DCompileFromFile(L"../Bin/ShaderFiles/TrailInterpCS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                            "CSMain", "cs_5_0", 0, 0, &csBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob)
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
        return hr;
    }
    if (FAILED(m_pDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &m_pCS_Interp)))
        return E_FAIL;
    Safe_Release(csBlob);

   
    m_pShader = CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/TrailRender.hlsl", TRAIL_POINT::Elements,TRAIL_POINT::iNumElements);
    return m_pShader ? S_OK : E_FAIL;
}

//=====================================================================
// Buffer 생성
//=====================================================================
HRESULT CEffect_TrailStream::createBuffers()
{
    const UINT totalPoints = m_desc.maxTrails * m_desc.maxPointsPerTrail;

    std::vector<TrailPoint> init(totalPoints);
    for (auto& tp : init)
    {
        tp.pos = {0, 0, 0};
        tp.life = 0.f;
        tp.color = {1, 1, 1, 0};
    }

    auto makeSB = [&](ID3D11Buffer** ppBuffer, ID3D11ShaderResourceView** ppSRV, ID3D11UnorderedAccessView** ppUAV,
                      UINT elemCount, UINT elemStride, const void* initData) -> HRESULT
    {
        D3D11_BUFFER_DESC bd{};
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        bd.ByteWidth = elemCount * elemStride;
        bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bd.StructureByteStride = elemStride;
        bd.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = initData;

        if (FAILED(m_pDevice->CreateBuffer(&bd, initData ? &sd : nullptr, ppBuffer)))
            return E_FAIL;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
        srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvd.Format = DXGI_FORMAT_UNKNOWN;
        srvd.Buffer.FirstElement = 0;
        srvd.Buffer.NumElements = elemCount;
        if (FAILED(m_pDevice->CreateShaderResourceView(*ppBuffer, &srvd, ppSRV)))
            return E_FAIL;

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
        uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavd.Format = DXGI_FORMAT_UNKNOWN;
        uavd.Buffer.FirstElement = 0;
        uavd.Buffer.NumElements = elemCount;
        if (FAILED(m_pDevice->CreateUnorderedAccessView(*ppBuffer, &uavd, ppUAV)))
            return E_FAIL;

        return S_OK;
    };

    // Trail Points A/B PingPong
    if (FAILED(makeSB(&m_pTrailBufA, &m_SRV_A, &m_UAV_A, totalPoints, sizeof(TrailPoint), init.data())))
        return E_FAIL;
    if (FAILED(makeSB(&m_pTrailBufB, &m_SRV_B, &m_UAV_B, totalPoints, sizeof(TrailPoint), init.data())))
        return E_FAIL;

    // Trail Header (링버퍼 필드 포함)
    std::vector<TrailHeader> headers(m_desc.maxTrails);
    for (auto& h : headers)
    {
        h.width = 0.08f;
        h.active = 0;
        h.head = 0;
        h.count = 0;
        h.first = 0;
        h.frameIndex = 0;
        h.generation = 0;
    }

    if (FAILED(makeSB(&m_pTrailHeader, &m_SRV_Header, &m_UAV_Header, m_desc.maxTrails, sizeof(TrailHeader),
                      headers.data())))
        return E_FAIL;

    // Spawn Upload Buffer
    {
        D3D11_BUFFER_DESC bd{};
        bd.ByteWidth = sizeof(SPAWN_REQUEST) * m_desc.maxTrails;
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bd.StructureByteStride = sizeof(SPAWN_REQUEST);

        if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &m_pSpawnUpload)))
            return E_FAIL;

        D3D11_SHADER_RESOURCE_VIEW_DESC sd{};
        sd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        sd.Format = DXGI_FORMAT_UNKNOWN;
        sd.Buffer.FirstElement = 0;
        sd.Buffer.NumElements = m_desc.maxTrails;

        if (FAILED(m_pDevice->CreateShaderResourceView(m_pSpawnUpload, &sd, &m_SRV_SpawnUpload)))
            return E_FAIL;
    }

    // 1️⃣ GPU에서 TrailVertex 쓰기용 (CS 출력용)
    UINT segPerTrail = (m_desc.maxPointsPerTrail > 0) ? (m_desc.maxPointsPerTrail - 1) : 0;
    UINT maxVertsTrail = segPerTrail * 8u*6;
    UINT maxVerts = m_desc.maxTrails * maxVertsTrail;

    UINT byteWidth = maxVerts * 40; // 36B per vertex
    byteWidth = (byteWidth + 15u) & ~15u;

    D3D11_BUFFER_DESC desc{};
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
    desc.ByteWidth = byteWidth;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    desc.Usage = D3D11_USAGE_DEFAULT;
    if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, &m_pTrailVertex)))
        return E_FAIL;

    // RAW UAV
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.NumElements = desc.ByteWidth / 4;
    uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTrailVertex, &uavDesc, &m_UAV_TrailVertex)))
        return E_FAIL;

    return S_OK;
}

//=====================================================================
// Constant Buffers
//=====================================================================
HRESULT CEffect_TrailStream::createPerFrameCB()
{
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    // Update CS용
    bd.ByteWidth = sizeof(CS_PERFRAME_CS);
    bd.ByteWidth = (bd.ByteWidth + 15) & ~15;
    if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &m_pCSPerFrame)))
        return E_FAIL;

    // Interp CS용
    bd.ByteWidth = sizeof(CS_PERFRAME_INTERP);
    if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &m_pCSPerFrame_Interp)))
        return E_FAIL;

    // VS용
    bd.ByteWidth = sizeof(VS_PERFRAME);
   if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &m_pVSPerFrame)))
        return E_FAIL;
    return S_OK;
}

//=====================================================================
// Indirect Args
//=====================================================================
HRESULT CEffect_TrailStream::createIndirectArgs()
{
    D3D11_BUFFER_DESC args{};
    args.ByteWidth = 16;
    args.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    args.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS | D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    args.Usage = D3D11_USAGE_DEFAULT;

    UINT init[4] = {0, 1, 0, 0};
    D3D11_SUBRESOURCE_DATA sd{init};
    if (FAILED(m_pDevice->CreateBuffer(&args, &sd, &m_pIndirectArgs)))
        return E_FAIL;

    D3D11_UNORDERED_ACCESS_VIEW_DESC uav{};
    uav.Format = DXGI_FORMAT_R32_TYPELESS;
    uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uav.Buffer.NumElements = args.ByteWidth / 4;
    uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pIndirectArgs, &uav, &m_UAV_IndirectArgs)))
        return E_FAIL;
    return S_OK;

}

//=====================================================================
// Update
//=====================================================================
void CEffect_TrailStream::pingpong()
{
    m_AasInput = !m_AasInput;
}

HRESULT CEffect_TrailStream::Trigger_Effect(void* pArg)
{
    if (!pArg)
        return S_OK;
  
    SPAWN_REQUEST req = *static_cast<SPAWN_REQUEST*>(pArg);

    if (req.trailIndex >= m_desc.maxTrails || req.trailIndex <0)
    {
        int idx = AllocateTrail();
        if (idx < 0)
            return S_OK; // 여유 슬롯 없으면 스킵
        req.trailIndex = (UINT)idx;
        
    }
   
    req.generation = m_GenerationTable[req.trailIndex];
    m_spawnQueue.push_back(req);
    return S_OK;
}

int CEffect_TrailStream::AllocateTrail()
{
    int index = m_NextTrailID % m_desc.maxTrails;
    m_NextTrailID++;

    // 현재 인덱스 세대 증가
    m_GenerationTable[index]++;

    // 해당 인덱스 활성화
    m_inUse[index] = 1;

    return index; 
}

void CEffect_TrailStream::ReleaseTrail(int index)
{
    if (index < 0 || (UINT)index >= m_desc.maxTrails)
        return;

    m_inUse[index] = 0; // 풀에서 비활성화
}

void CEffect_TrailStream::Update(_float dt)
{
    // =========================
    // 1) TrailUpdateCS: 포인트 fade/링버퍼 + 스폰 반영
    // =========================
    // --- CS(b0) 업데이트용 CB ---
    CS_PERFRAME_CS cb{};
    cb.dt = dt;
    cb.fadeSpeed = m_desc.fadeSpeed;
    cb.lifeTime = m_desc.lifeTime;
    cb.maxTrails = m_desc.maxTrails;
    cb.maxPointsPerTrail = m_desc.maxPointsPerTrail;
    cb.spawnCount = static_cast<UINT>(min(m_spawnQueue.size(), (size_t)m_desc.maxTrails));
    cb.mode = m_iMode;
    cb.minStepDist = 0.08f; 
    cb.maxStitch = 12; 
    {
        D3D11_MAPPED_SUBRESOURCE ms{};
        if (SUCCEEDED(m_pContext->Map(m_pCSPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
        {
            memcpy(ms.pData, &cb, sizeof(cb));
            m_pContext->Unmap(m_pCSPerFrame, 0);
        }
    }

    // --- Spawn 업로드 (슬롯별 1개 요청만 유효) ---
    {
        D3D11_MAPPED_SUBRESOURCE ms{};
        if (SUCCEEDED(m_pContext->Map(m_pSpawnUpload, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
        {
            auto* out = reinterpret_cast<SPAWN_REQUEST*>(ms.pData);
            // 전체 clear
            for (UINT i = 0; i < m_desc.maxTrails; ++i)
            {
                out[i] = {};
                out[i].valid = 0;
                out[i].trailIndex = i; // (디버그 편의)
            }
            // 큐 반영
            for (const auto& rq : m_spawnQueue)
            {
                if (rq.trailIndex < m_desc.maxTrails)
                {
                    out[rq.trailIndex] = rq;
                    out[rq.trailIndex].valid = 1;
                }
            }
            m_pContext->Unmap(m_pSpawnUpload, 0);
        }
    }

    ID3D11ShaderResourceView* inSRV = m_AasInput ? m_SRV_A : m_SRV_B;   // t0
    ID3D11UnorderedAccessView* outUAV = m_AasInput ? m_UAV_B : m_UAV_A; // u0

    // --- TrailUpdateCS 바인딩 & 디스패치 ---
    {
        m_pContext->CSSetShader(m_pCS_Update, nullptr, 0);
        m_pContext->CSSetConstantBuffers(0, 1, &m_pCSPerFrame);

        ID3D11ShaderResourceView* srvs[2] = {inSRV, m_SRV_SpawnUpload}; // t0=points_in, t1=spawn
        m_pContext->CSSetShaderResources(0, 2, srvs);

        ID3D11UnorderedAccessView* uavs[2] = {outUAV, m_UAV_Header}; // u0=points_out, u1=header(uav)
        m_pContext->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);

        const UINT groups = (m_desc.maxTrails + THREADS - 1) / THREADS;
        m_pContext->Dispatch(groups, 1, 1);

        // 언바인드 (UAV/SRV 해제)
        ID3D11ShaderResourceView* nullSRV[2] = {nullptr, nullptr};
        ID3D11UnorderedAccessView* nullUAV[2] = {nullptr, nullptr};
        m_pContext->CSSetShaderResources(0, 2, nullSRV);
        m_pContext->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);
        m_pContext->CSSetShader(nullptr, nullptr, 0);
    }

    // 스폰 큐는 CPU측에서 비움
    m_spawnQueue.clear();
    // ping-pong swap (방금 outUAV에 쓴 버퍼가 다음 프레임 inSRV가 됨)
    pingpong();

    // =========================
    // 2) DrawIndirect 인자 초기화 (InterpCS가 누적할 "시작값" 0,1,0,0)
    //    !!! InterpCS 실행 '이전'에 해야 함 !!!
    // =========================
    {
        const UINT resetArgs[4] = {0, 1, 0, 0}; // vertexCount, instanceCount, startVertex, startInstance
        m_pContext->UpdateSubresource(m_pIndirectArgs, 0, nullptr, resetArgs, 0, 0);
    }

    // =========================
    // 3) TrailInterpCS: 카메라 빌보딩 + (옵션) Catmull-Rom → 최종 TrailVertex & DrawArgs 갱신
    // =========================
    // --- Interp 전용 CB 업데이트 (CS_PERFRAME_INTERP) ---
    {
        CS_PERFRAME_INTERP cbI{};
        cbI.mode = m_iPass; // 0=Texture(직선), 1=Curve(Catmull-Rom)
        cbI.maxTrails = m_desc.maxTrails;
        cbI.maxPointsPerTrail = m_desc.maxPointsPerTrail;
        cbI.lifeTime = m_desc.lifeTime;
        const _float4 CPos = *m_pGameInstance->Get_CamPosition();
        cbI.g_CamPosWS = {CPos.x, CPos.y, CPos.z};

        D3D11_MAPPED_SUBRESOURCE ms{};
        if (SUCCEEDED(m_pContext->Map(m_pCSPerFrame_Interp, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
        {
            memcpy(ms.pData, &cbI, sizeof(cbI));
            m_pContext->Unmap(m_pCSPerFrame_Interp, 0);
        }
    }

    // --- InterpCS 바인딩 & 디스패치 ---
    {
        // InterpCS 입력: 헤더 & (업데이트된) 포인트 SRV
        ID3D11ShaderResourceView* srv[2] = {m_SRV_Header, (m_AasInput ? m_SRV_A : m_SRV_B)};
        // InterpCS 출력: 최종 TrailVertex RWStructuredBuffer, DrawArgs RWByteAddressBuffer
        ID3D11UnorderedAccessView* uav[2] = {m_UAV_TrailVertex, m_UAV_IndirectArgs};

        m_pContext->CSSetShader(m_pCS_Interp, nullptr, 0);
        m_pContext->CSSetConstantBuffers(0, 1, &m_pCSPerFrame_Interp);
        m_pContext->CSSetShaderResources(0, 2, srv);
        m_pContext->CSSetUnorderedAccessViews(0, 2, uav, nullptr);

        const UINT groups = (m_desc.maxTrails + THREADS - 1) / THREADS; // InterpCS는 "trail당 1스레드" 설계
        m_pContext->Dispatch(groups, 1, 1);

        // 언바인드
        ID3D11ShaderResourceView* nullSRV[2] = {nullptr, nullptr};
        ID3D11UnorderedAccessView* nullUAV[2] = {nullptr, nullptr};
        m_pContext->CSSetShaderResources(0, 2, nullSRV);
        m_pContext->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);
        m_pContext->CSSetShader(nullptr, nullptr, 0);
    }
}

//=====================================================================
// Render
//=====================================================================
HRESULT CEffect_TrailStream::Render(CShader* /*pShader*/)
{
    UINT stride = 40;
    UINT offset = 0;
    m_pContext->IASetVertexBuffers(0, 1, &m_pTrailVertex, &stride, &offset);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    VS_PERFRAME cb{};
    XMStoreFloat4x4(&cb.g_View, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
    XMStoreFloat4x4(&cb.g_Proj, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
    const _float4 CPos = *m_pGameInstance->Get_CamPosition();
    cb.g_CamPosWS = {CPos.x, CPos.y, CPos.z};
    cb.g_TrailUVScale = m_vTrailTexUVScale;
    cb.g_TotalFrames = m_iTexTotalFrames;

    if(FAILED(m_pShader->Bind_RawValue("g_TotalFrames", &cb.g_TotalFrames, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_View", &cb.g_View)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_Proj", &cb.g_Proj)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_CamPosWS", &cb.g_CamPosWS, sizeof(_float3))))
        return E_FAIL;
    if(FAILED(m_pShader->Bind_RawValue("g_TrailUVScale", &cb.g_TrailUVScale, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_SRV("g_Header", m_SRV_Header)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;
    if (m_pTrailTexCom)
    {
        if (FAILED(m_pTrailTexCom->Bind_ShaderResource(m_pShader, "g_TrailTex", 0)))
            return E_FAIL;
    }
   
    m_pShader->Begin(m_iPass);

    m_pContext->DrawInstancedIndirect(m_pIndirectArgs, 0);

    return S_OK;
}

CEffect_TrailStream* CEffect_TrailStream::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pDesc)
{
    CEffect_TrailStream* pInstance = new CEffect_TrailStream(pDevice, pContext);
    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX("Failed to Create : CEffect_TrailStream");
        Safe_Release(pInstance);
    }
    return pInstance;
}

//=====================================================================
// Free
//=====================================================================
void CEffect_TrailStream::Free()
{
    __super::Free();

    Safe_Release(m_pTrailBufA);
    Safe_Release(m_pTrailBufB);
    Safe_Release(m_SRV_A);
    Safe_Release(m_SRV_B);
    Safe_Release(m_UAV_A);
    Safe_Release(m_UAV_B);
    Safe_Release(m_pTrailHeader);
    Safe_Release(m_SRV_Header);
    Safe_Release(m_UAV_Header);
    Safe_Release(m_pSpawnUpload);
    Safe_Release(m_SRV_SpawnUpload);
    Safe_Release(m_pCSPerFrame);
    Safe_Release(m_pVSPerFrame);
    Safe_Release(m_pIndirectArgs);
    Safe_Release(m_pShader);
    Safe_Release(m_pTrailTexCom);
    Safe_Release(m_pCS_Update);
    Safe_Release(m_pCS_Interp);
    Safe_Release(m_pTrailVertex);
    Safe_Release(m_UAV_TrailVertex);
    Safe_Release(m_pCSPerFrame_Interp);
}
