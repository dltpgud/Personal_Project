#include "Effect_DecalStream.h"
#include "Shader.h"
#include "GameInstance.h"
#include <d3dcompiler.h>

CEffect_DecalStream::CEffect_DecalStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEffectStream(pDevice, pContext)
{
}

CEffect_DecalStream* CEffect_DecalStream::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CEffect_DecalStream* pInstance = new CEffect_DecalStream(pDevice, pContext);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CEffect_DecalStream");
        Safe_Release(pInstance);
    }
    return pInstance;
}

HRESULT CEffect_DecalStream::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    DECALSTREAM_DESC* pDesc = static_cast<DECALSTREAM_DESC*>(pArg);
    m_MaxDecals = pDesc->MaxDecals;
    m_MaxSpawnPerFrame = pDesc->MaxSpawnPerFrame;

    // Texture2DArray   
    if (FAILED(BuildGlobalDecalArray(pDesc->FilePathFmt, pDesc->TextureCount)))
        return E_FAIL;

    // 1) 큐브 지오메트리 (기본 데칼 볼륨 메시)
    if (FAILED(createGeometryBuffers()))
        return E_FAIL;

    // 2) GPU persistent buffers
    if (FAILED(createGPUStorageBuffers()))
        return E_FAIL;

    // 3) CPU->GPU 업로드용 spawn buffer
    if (FAILED(createSpawnUploadBuffer()))
        return E_FAIL;

    // 4) DrawIndirect args buffer
    if (FAILED(createIndirectArgsBuffer()))
        return E_FAIL;

    if (FAILED(createCB()))
        return E_FAIL;


    // 5) 컴퓨트 셰이더 로드
    if (FAILED(createComputeShaders()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_DecalStream::Trigger_Effect(void* pArg, _float fTimeDelta)
{
    if (!pArg)
        return S_OK;

    DECAL_DESC* pDecal = static_cast<DECAL_DESC*>(pArg);

    CPU_DECAL_REQUEST r{};
    DECAL_SPAWN_REQ& out = r.Req;

    // 방향 선택 (SSD면 Dir, BOX면 Normal)
    _vector useDir = (pDecal->iType == DECAL_DESC::TYPE_SSD) ? pDecal->vDir : pDecal->vNormal;

    out.Pos = _float3(XMVectorGetX(pDecal->vPos), XMVectorGetY(pDecal->vPos), XMVectorGetZ(pDecal->vPos));

    out.Valid = 1; 

    out.Dir = _float3(XMVectorGetX(useDir), XMVectorGetY(useDir), XMVectorGetZ(useDir));

    out.Size = pDecal->fSize;
    out.Depth = pDecal->fDepth;
    out.LifeTime = pDecal->fLifeTime;
    out.DeltaScale = pDecal->DeltaScaling;
    out.TexIndex = pDecal->iTexIndex;
    out.DecalType = pDecal->iType;
    out.bNormal = pDecal->bNormal ? 1 : 0;

    if (0 == pDecal->iContinuous)
    {
        m_SpawnQueue.push_back(r);

        if (pDecal->bOnce)
            pDecal->bActive = true;

        return S_OK;
    }
    else
    {
        CONTINUOUS_STATE& state = m_ContinuousMap[pDecal->iContinuous];
        m_TotalTime += fTimeDelta;

        _vector curPos = pDecal->vPos;
        _bool canSpawn = false;
        if (!state.Initialized)
        {
            canSpawn = true;
            state.Initialized = true;
        }
        else
        {
            _float dist = XMVectorGetX(XMVector3Length(curPos - state.LastPos));
            _float elapsed = m_TotalTime - state.LastTime;

            if (dist >= fkMinDistance && elapsed >= kCooldownMs)
                canSpawn = true;
        }

        if (canSpawn)
        {
            m_SpawnQueue.push_back(r);
            state.LastPos = curPos;
            state.LastTime = m_TotalTime;
        }

        return S_OK;
    }

    return S_OK;
}

void CEffect_DecalStream::Update(_float fTimeDelta)
{
    vector<UINT> zeros(m_MaxDecals, 0);
    m_pContext->UpdateSubresource(m_pLiveList, 0, nullptr, zeros.data(), 0, 0);

    uploadSpawnRequestsToGPU();

    dispatchSpawnUpdateCS(fTimeDelta);

    resetDrawArgsOnCPU();

    dispatchBuildDrawCS();

    m_SpawnQueue.clear();
}

HRESULT CEffect_DecalStream::Render(CShader* pShader)
{
    if (!pShader)
        return E_FAIL;

    UINT stride = sizeof(XMFLOAT3) + sizeof(XMFLOAT2);
    UINT offset = 0;
    m_pContext->IASetVertexBuffers(0, 1, &m_pVB_Cube, &stride, &offset);
    m_pContext->IASetIndexBuffer(m_pIB_Cube, DXGI_FORMAT_R32_UINT, 0);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D11ShaderResourceView* nullSRV[16] = {nullptr};
    m_pContext->PSSetShaderResources(0, 16, nullSRV);

    // VS용 StructuredBuffer 바인딩
    if (FAILED(pShader->Bind_SRV("g_InstanceDataVS", m_pInstanceSRV)))
        return E_FAIL;

    if (FAILED(pShader->Bind_SRV("g_DecalArray", m_pDecalArraySRV)))
        return E_FAIL;

    pShader->Begin(0);

    m_pContext->DrawIndexedInstancedIndirect(m_pIndirectArgs, 0);
    return S_OK;
}

HRESULT CEffect_DecalStream::createGeometryBuffers()
{
    struct VTX
    {
        XMFLOAT3 pos;
        XMFLOAT2 uv;
    };
    VTX verts[8] = {
        {{-0.5f, 0.5f, -0.5f}, {0, 0}},  {{0.5f, 0.5f, -0.5f}, {1, 0}},  {{0.5f, -0.5f, -0.5f}, {1, 1}},
        {{-0.5f, -0.5f, -0.5f}, {0, 1}}, {{-0.5f, 0.5f, 0.5f}, {0, 0}},  {{0.5f, 0.5f, 0.5f}, {1, 0}},
        {{0.5f, -0.5f, 0.5f}, {1, 1}},   {{-0.5f, -0.5f, 0.5f}, {0, 1}},
    };

    UINT idx[36] = {1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7, 4, 5, 1, 4, 1, 0,
                    3, 2, 6, 3, 6, 7, 5, 4, 7, 5, 7, 6, 0, 1, 2, 0, 2, 3};

    // VB
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = sizeof(verts);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = verts;
    if (FAILED(m_pDevice->CreateBuffer(&bd, &sd, &m_pVB_Cube)))
        return E_FAIL;

    // IB
    D3D11_BUFFER_DESC idesc{};
    idesc.ByteWidth = sizeof(idx);
    idesc.Usage = D3D11_USAGE_DEFAULT;
    idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA isd{};
    isd.pSysMem = idx;
    if (FAILED(m_pDevice->CreateBuffer(&idesc, &isd, &m_pIB_Cube)))
        return E_FAIL;

    m_iIndexCount = 36;
    return S_OK;
}

HRESULT CEffect_DecalStream::createGPUStorageBuffers()
{
    {
        D3D11_BUFFER_DESC bd{};
        bd.ByteWidth = sizeof(GPU_DecalHeader) * m_MaxDecals;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bd.StructureByteStride = sizeof(GPU_DecalHeader);

        if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &m_pDecalSlots)))
            return E_FAIL;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
        srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvd.Format = DXGI_FORMAT_UNKNOWN;
        srvd.Buffer.FirstElement = 0;
        srvd.Buffer.NumElements = m_MaxDecals;
        if (FAILED(m_pDevice->CreateShaderResourceView(m_pDecalSlots, &srvd, &m_pDecalSlotsSRV)))
            return E_FAIL;

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
        uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavd.Format = DXGI_FORMAT_UNKNOWN;
        uavd.Buffer.FirstElement = 0;
        uavd.Buffer.NumElements = m_MaxDecals;
        if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pDecalSlots, &uavd, &m_pDecalSlotsUAV)))
            return E_FAIL;
    }

    {
        D3D11_BUFFER_DESC bd{};
        bd.ByteWidth = sizeof(UINT) * m_MaxDecals;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bd.StructureByteStride = sizeof(UINT);

        if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &m_pLiveList)))
            return E_FAIL;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
        srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvd.Format = DXGI_FORMAT_UNKNOWN;
        srvd.Buffer.NumElements = m_MaxDecals;
        if (FAILED(m_pDevice->CreateShaderResourceView(m_pLiveList, &srvd, &m_pLiveListSRV)))
            return E_FAIL;

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
        uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavd.Format = DXGI_FORMAT_UNKNOWN;
        uavd.Buffer.NumElements = m_MaxDecals;
        if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pLiveList, &uavd, &m_pLiveListUAV)))
            return E_FAIL;
    }

    {
        D3D11_BUFFER_DESC bd{};
        bd.ByteWidth = sizeof(GPU_DecalInstanceData) * m_MaxDecals;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bd.StructureByteStride = sizeof(GPU_DecalInstanceData);

        if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &m_pInstanceBuffer)))
            return E_FAIL;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
        srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvd.Format = DXGI_FORMAT_UNKNOWN;
        srvd.Buffer.NumElements = m_MaxDecals;
        if (FAILED(m_pDevice->CreateShaderResourceView(m_pInstanceBuffer, &srvd, &m_pInstanceSRV)))
            return E_FAIL;

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
        uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavd.Format = DXGI_FORMAT_UNKNOWN;
        uavd.Buffer.NumElements = m_MaxDecals;
        if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pInstanceBuffer, &uavd, &m_pInstanceUAV)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CEffect_DecalStream::createSpawnUploadBuffer()
{
    // CPU -> GPU (MAP/DISCARD)
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = sizeof(DECAL_SPAWN_REQ) * m_MaxSpawnPerFrame;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = sizeof(DECAL_SPAWN_REQ);

    if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &m_pSpawnUpload)))
        return E_FAIL;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
    srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvd.Format = DXGI_FORMAT_UNKNOWN;
    srvd.Buffer.NumElements = m_MaxSpawnPerFrame;
    if (FAILED(m_pDevice->CreateShaderResourceView(m_pSpawnUpload, &srvd, &m_pSpawnUploadSRV)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_DecalStream::createIndirectArgsBuffer()
{
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = sizeof(UINT) * 5;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS; // CS에서 InstanceCount 채우게
    bd.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS | D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

    UINT initArgs[5] = {m_iIndexCount, 0, 0, 0, 0};
    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = initArgs;

    if (FAILED(m_pDevice->CreateBuffer(&bd, &sd, &m_pIndirectArgs)))
        return E_FAIL;

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
    uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavd.Format = DXGI_FORMAT_R32_UINT;
    uavd.Buffer.NumElements = 5;
    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pIndirectArgs, &uavd, &m_pIndirectArgsUAV)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_DecalStream::createComputeShaders()
{
    ID3DBlob* csBlob = nullptr;
    ID3DBlob* errBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(L"../Bin/ShaderFiles/CS_DecalSpawnUpdate.hlsl", nullptr,
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE, "CSMain", "cs_5_0", 0, 0, &csBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob)
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
        return hr;
    }
    if (FAILED(m_pDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr,
                                              &m_pCS_SpawnUpdate)))
        return E_FAIL;
    Safe_Release(csBlob);

    hr = D3DCompileFromFile(L"../Bin/ShaderFiles/CS_DecalBuildInstance.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                            "CSMain", "cs_5_0", 0, 0, &csBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob)
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
        return hr;
    }
    if (FAILED(m_pDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr,
                                              &m_pCS_BuildDrawData)))
        return E_FAIL;
    Safe_Release(csBlob);


    return S_OK;
}

HRESULT CEffect_DecalStream::createCB()
{
    D3D11_BUFFER_DESC cbd{};
    cbd.ByteWidth = sizeof(CB_DECAL_FRAME);
    cbd.Usage = D3D11_USAGE_DEFAULT; //  DEFAULT로
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = 0; 
    cbd.MiscFlags = 0;
    cbd.StructureByteStride = 0;
    if (FAILED( m_pDevice->CreateBuffer(&cbd, nullptr, &m_pCB_DecalFrame)))
        return E_FAIL;
    return S_OK;
}

HRESULT CEffect_DecalStream::uploadSpawnRequestsToGPU()
{
    D3D11_MAPPED_SUBRESOURCE ms{};
    if (FAILED(m_pContext->Map(m_pSpawnUpload, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
        return E_FAIL;

    auto* out = reinterpret_cast<DECAL_SPAWN_REQ*>(ms.pData);

    for (UINT i = 0; i < m_MaxSpawnPerFrame; ++i)
    {
        out[i] = {};
        out[i].Valid = 0;
    }

    UINT writeCount = (UINT)std::min<size_t>(m_SpawnQueue.size(), m_MaxSpawnPerFrame);
    for (UINT i = 0; i < writeCount; ++i)
    {
        out[i] = m_SpawnQueue[i].Req;
        out[i].Valid = 1;
    }

    m_pContext->Unmap(m_pSpawnUpload, 0);

    return S_OK;
}

HRESULT CEffect_DecalStream::dispatchSpawnUpdateCS(float dt)
{
    CB_DECAL_FRAME cbData = {dt, (UINT)m_SpawnQueue.size(), m_MaxDecals, 0.f};
    m_pContext->UpdateSubresource(m_pCB_DecalFrame, 0, nullptr, &cbData, 0, 0);

    m_pContext->CSSetShader(m_pCS_SpawnUpdate, nullptr, 0);
    m_pContext->CSSetConstantBuffers(0, 1, &m_pCB_DecalFrame);

    ID3D11ShaderResourceView* srvs[1] = {m_pSpawnUploadSRV};
    m_pContext->CSSetShaderResources(0, 1, srvs);

    ID3D11UnorderedAccessView* uavs[2] = {m_pDecalSlotsUAV, m_pLiveListUAV};
    UINT initialCounts[2] = {(UINT)-1, (UINT)-1};
    m_pContext->CSSetUnorderedAccessViews(0, 2, uavs, initialCounts);

    UINT groups = (m_MaxDecals + THREADS - 1) / THREADS;
    m_pContext->Dispatch(groups, 1, 1);

    m_pContext->Flush();

    ID3D11UnorderedAccessView* nullUAV[2] = {nullptr, nullptr};
    m_pContext->CSSetUnorderedAccessViews(0, 2, nullUAV, initialCounts);

    ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
    m_pContext->CSSetShaderResources(0, 1, nullSRV);

    ID3D11Buffer* nullCB[1] = {nullptr};
    m_pContext->CSSetConstantBuffers(0, 1, nullCB);

    m_pContext->CSSetShader(nullptr, nullptr, 0);

    return S_OK;
}

void CEffect_DecalStream::resetDrawArgsOnCPU()
{
    UINT initArgs[5] = {m_iIndexCount, 0, 0, 0, 0};
    m_pContext->UpdateSubresource(m_pIndirectArgs, 0, nullptr, initArgs, 0, 0);
}

#ifdef _DEBUG

void CEffect_DecalStream::DebugGPUState()
{
    cout << "========== [DecalStream GPU Debug] ==========\n";

    // 1 DrawIndirectArgs 확인
    {
        ID3D11Buffer* pStaging = nullptr;
        D3D11_BUFFER_DESC desc{};
        m_pIndirectArgs->GetDesc(&desc);
        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;

        if (SUCCEEDED(m_pDevice->CreateBuffer(&desc, nullptr, &pStaging)))
        {
            m_pContext->CopyResource(pStaging, m_pIndirectArgs);

            D3D11_MAPPED_SUBRESOURCE mapped{};
            if (SUCCEEDED(m_pContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped)))
            {
                UINT* args = reinterpret_cast<UINT*>(mapped.pData);
                cout << "[IndirectArgs] IndexCountPerInstance=" << args[0] << ", InstanceCount=" << args[1]
                          << ", StartIndex=" << args[2] << ", BaseVertex=" << args[3] << ", StartInstance=" << args[4]
                          << "\n";

                m_pContext->Unmap(pStaging, 0);
            }
            Safe_Release(pStaging);
        }
        else
            cout << "[IndirectArgs] ❌ Failed to create staging buffer\n";
    }

    // 2️nstanceBuffer 확인 (앞부분 3~5개만)
    {
        ID3D11Buffer* pStaging = nullptr;
        D3D11_BUFFER_DESC desc{};
        m_pInstanceBuffer->GetDesc(&desc);
        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;

        if (SUCCEEDED(m_pDevice->CreateBuffer(&desc, nullptr, &pStaging)))
        {
            m_pContext->CopyResource(pStaging, m_pInstanceBuffer);

            D3D11_MAPPED_SUBRESOURCE mapped{};
            if (SUCCEEDED(m_pContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped)))
            {
                auto* data = reinterpret_cast<GPU_DecalInstanceData*>(mapped.pData);
                 cout << "[InstanceBuffer] showing first 5:\n";
                for (UINT i = 0; i < min(5u, m_MaxDecals); ++i)
                {
                    const auto& d = data[i];
                    cout << "  [" << i << "] Pos(" << d.DecalPos.x << "," << d.DecalPos.y << "," << d.DecalPos.z
                              << ") Tex=" << d.TexIndex << " Life=" << d.LifeTime << " Time=" << d.DecalTime
                              << " Type=" << d.DecalType << " bNormal=" << d.bNormal << "\n";
                }
                m_pContext->Unmap(pStaging, 0);
            }
            Safe_Release(pStaging);
        }
        else
           cout << "[InstanceBuffer] ❌ Failed to create staging buffer\n";
    }

    // 3️LiveList (살아있는 데칼 슬롯 인덱스)
    {
        ID3D11Buffer* pStaging = nullptr;
        D3D11_BUFFER_DESC desc{};
        m_pLiveList->GetDesc(&desc);
        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;

        if (SUCCEEDED(m_pDevice->CreateBuffer(&desc, nullptr, &pStaging)))
        {
            m_pContext->CopyResource(pStaging, m_pLiveList);

            D3D11_MAPPED_SUBRESOURCE mapped{};
            if (SUCCEEDED(m_pContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped)))
            {
                auto* ids = reinterpret_cast<UINT*>(mapped.pData);
                cout << "[LiveList] first 8 indices:";
                for (UINT i = 0; i < min(8u, m_MaxDecals); ++i) { std::cout << " " << ids[i]; }
                std::cout << "\n";
                m_pContext->Unmap(pStaging, 0);
            }
            Safe_Release(pStaging);
        }
        else
            cout << "[LiveList] ❌ Failed to create staging buffer\n";
    }

    cout << "=============================================\n";
}
#endif // _DEBUG
HRESULT CEffect_DecalStream::BuildGlobalDecalArray(_wstring FilePathFmt, _uint TextureCount)
{
   _int iTotalSlices = 0;

    iTotalSlices += (TextureCount +1); // 노멀 포함
    
    if (iTotalSlices == 0)
        return E_FAIL;

    vector<ID3D11Texture2D*> srcTex(iTotalSlices, nullptr);
    D3D11_TEXTURE2D_DESC firstDesc{};
    bool firstSet = false;
    UINT sliceIdx = 0;

   {
       _tchar path[MAX_PATH] = TEXT("");
       wsprintf(path, FilePathFmt.c_str(), 0);
   
       ID3D11Resource* res = nullptr;
       HRESULT hr = CreateDDSTextureFromFile(m_pDevice, path, &res, nullptr);
       if (FAILED(hr) || !res)
       {
           for (size_t j = 0; j < srcTex.size(); ++j) Safe_Release(srcTex[j]);
           return E_FAIL;
       }
   
       hr = res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&srcTex[sliceIdx]);
       res->Release();
   
       if (FAILED(hr) || !srcTex[sliceIdx])
       {
           for (size_t j = 0; j < srcTex.size(); ++j) Safe_Release(srcTex[j]);
           return E_FAIL;
       }
   
       if (!firstSet)
       {
           srcTex[sliceIdx]->GetDesc(&firstDesc);
           firstSet = true;
       }
       else
       {
           D3D11_TEXTURE2D_DESC d{};
           srcTex[sliceIdx]->GetDesc(&d);
           _bool same =
               (d.Format == firstDesc.Format && d.Width == firstDesc.Width && d.Height == firstDesc.Height &&
                d.MipLevels == firstDesc.MipLevels && d.SampleDesc.Count == firstDesc.SampleDesc.Count &&
                d.SampleDesc.Quality == firstDesc.SampleDesc.Quality);
           if (!same)
           {
               for (size_t j = 0; j < srcTex.size(); ++j) Safe_Release(srcTex[j]);
               return E_FAIL;
           }
       }
   
       ++sliceIdx;
   }

   for (UINT i = 1; i <= TextureCount; ++i)
   {
       _tchar path[MAX_PATH] = TEXT("");
       wsprintf(path, FilePathFmt.c_str(), i);
 
      ID3D11Resource* res = nullptr;
      HRESULT hr = CreateDDSTextureFromFile(m_pDevice, path, &res, nullptr);
      if (FAILED(hr) || !res)
      {
          for (size_t j = 0; j < srcTex.size(); ++j) Safe_Release(srcTex[j]);
          return E_FAIL;
      }
 
      hr = res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&srcTex[sliceIdx]);
      res->Release();
 
      if (FAILED(hr) || !srcTex[sliceIdx])
      {
          for (size_t j = 0; j < srcTex.size(); ++j) Safe_Release(srcTex[j]);
          return E_FAIL;
      }
 
      D3D11_TEXTURE2D_DESC d{};
      srcTex[sliceIdx]->GetDesc(&d);
      _bool same = (d.Format == firstDesc.Format && d.Width == firstDesc.Width && d.Height == firstDesc.Height &&
                    d.MipLevels == firstDesc.MipLevels && d.SampleDesc.Count == firstDesc.SampleDesc.Count &&
                    d.SampleDesc.Quality == firstDesc.SampleDesc.Quality);
 
      if (!same)
      {
          for (size_t j = 0; j < srcTex.size(); ++j) Safe_Release(srcTex[j]);
          return E_FAIL;
      }
 
      ++sliceIdx;
   }

    D3D11_TEXTURE2D_DESC arrDesc = firstDesc;
    arrDesc.ArraySize = iTotalSlices;
    arrDesc.Usage = D3D11_USAGE_DEFAULT;
    arrDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    arrDesc.CPUAccessFlags = 0;
    arrDesc.MiscFlags = 0;
    arrDesc.ArraySize = sliceIdx;
    ID3D11Texture2D* arrayTex = nullptr;
    HRESULT hrArray = m_pDevice->CreateTexture2D(&arrDesc, nullptr, &arrayTex);
    if (FAILED(hrArray) || !arrayTex)
    {
        for (size_t j = 0; j < srcTex.size(); ++j) Safe_Release(srcTex[j]);
        return E_FAIL;
    }

    const UINT totalMips = arrDesc.MipLevels;

    for (UINT s = 0; s < iTotalSlices; ++s)
    {
        if (!srcTex[s])
        {
            continue;
        }
        if (!srcTex[s])
        {
            wchar_t msg[256];
            swprintf_s(msg, L"❌ srcTex[%d] is null!\n", s);
            OutputDebugStringW(msg);
            continue;
        }
        D3D11_TEXTURE2D_DESC sdesc{};
        srcTex[s]->GetDesc(&sdesc);
        const UINT copyMips = min(totalMips, sdesc.MipLevels);

        for (UINT mip = 0; mip < copyMips; ++mip)
        {
            const UINT dstSub = D3D11CalcSubresource(mip, s, totalMips);
            const UINT srcSub = D3D11CalcSubresource(mip, 0, sdesc.MipLevels);
            m_pContext->CopySubresourceRegion(arrayTex, dstSub, 0, 0, 0, srcTex[s], srcSub, nullptr);
        }
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = arrDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = totalMips;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = iTotalSlices;

    ID3D11ShaderResourceView* srv = nullptr;
    HRESULT hrSrv = m_pDevice->CreateShaderResourceView(arrayTex, &srvDesc, &srv);
    if (FAILED(hrSrv) || !srv)
    {
        Safe_Release(arrayTex);
        for (size_t j = 0; j < srcTex.size(); ++j) Safe_Release(srcTex[j]);
        return E_FAIL;
    }

    m_pDecalArraySRV = srv;

    Safe_Release(arrayTex);
    for (size_t j = 0; j < srcTex.size(); ++j) Safe_Release(srcTex[j]);

    return S_OK;
}

HRESULT CEffect_DecalStream::dispatchBuildDrawCS()
{
    ID3D11ShaderResourceView* srvs[2] = { m_pLiveListSRV, m_pDecalSlotsSRV };
    m_pContext->CSSetShaderResources(0, 2, srvs);

    ID3D11UnorderedAccessView* uavs[2] = {  m_pInstanceUAV, m_pIndirectArgsUAV };
    m_pContext->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);

    m_pContext->CSSetShader(m_pCS_BuildDrawData, nullptr, 0);

    UINT groups = (m_MaxDecals + THREADS - 1) / THREADS;
    m_pContext->Dispatch(groups, 1, 1);

    ID3D11ShaderResourceView* nullSRV[2] = {nullptr, nullptr};
    ID3D11UnorderedAccessView* nullUAV[2] = {nullptr, nullptr};
    m_pContext->CSSetShaderResources(0, 2, nullSRV);
    m_pContext->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);
    m_pContext->CSSetShader(nullptr, nullptr, 0);

    return S_OK;
}

void CEffect_DecalStream::Free()
{
    __super::Free();

    Safe_Release(m_pCS_SpawnUpdate);
    Safe_Release(m_pCS_BuildDrawData);

    Safe_Release(m_pVB_Cube);
    Safe_Release(m_pIB_Cube);

    Safe_Release(m_pDecalSlots);
    Safe_Release(m_pDecalSlotsSRV);
    Safe_Release(m_pDecalSlotsUAV);

    Safe_Release(m_pLiveList);
    Safe_Release(m_pLiveListSRV);
    Safe_Release(m_pLiveListUAV);

    Safe_Release(m_pInstanceBuffer);
    Safe_Release(m_pInstanceSRV);
    Safe_Release(m_pInstanceUAV);

    Safe_Release(m_pSpawnUpload);
    Safe_Release(m_pSpawnUploadSRV);

    Safe_Release(m_pIndirectArgs);
    Safe_Release(m_pIndirectArgsUAV);

    Safe_Release(m_pDecalArraySRV);
}
