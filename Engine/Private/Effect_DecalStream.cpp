#include "Effect_DecalStream.h"
#include "Shader.h"
#include "GameInstance.h"


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

    if (FAILED(BuildGlobalDecalArray(pDesc->FilePathFmt, pDesc->TextureCount)))
        return E_FAIL;

    m_pVIBuffer_Cube = CVIBuffer_Cube::Create(m_pDevice, m_pContext);
    if (!m_pVIBuffer_Cube)
        return E_FAIL;

    if (FAILED(createGPUStorageBuffers()))
        return E_FAIL;

    if (FAILED(createSpawnUploadBuffer()))
        return E_FAIL;
    for (_int i = 0; i < 2; ++i)
    {
        if (FAILED(CreateRawBuffer(sizeof(UINT) * 5, &m_pIndirectArgs[i], &m_pIndirectArgsUAV[i], true)))
            return E_FAIL;
    }
    if (FAILED(createCB()))
        return E_FAIL;

    if (FAILED(Create_CS(L"../Bin/ShaderFiles/CS_DecalSpawnUpdate.hlsl", "CSMain", &m_pCS_SpawnUpdate)))
        return E_FAIL;

    if (FAILED(Create_CS(L"../Bin/ShaderFiles/CS_DecalBuildInstance.hlsl", "CSMain", &m_pCS_BuildDrawData)))
        return E_FAIL;

    if (FAILED(Create_CS(L"../Bin/ShaderFiles/CS_ClearLiveList.hlsl", "CSMain", &m_pCS_ClearLiveList)))
        return E_FAIL;
    if (FAILED(Create_CS(L"../Bin/ShaderFiles/CS_ResetArgs.hlsl", "CSMain", &m_pCS_ResetArgs)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_DecalStream::Trigger_Effect(void* pArg, _float fTimeDelta)
{
    if (!pArg)
        return S_OK;
   // auto cpuBegin = std::chrono::high_resolution_clock::now();
    DECAL_DESC* pDecal = static_cast<DECAL_DESC*>(pArg);


    DECAL_SPAWN_REQ out;

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
    out.bNormal = pDecal->bNormal ? 1 : 0;

   if (0 == pDecal->iContinuous)
   {
       UINT write = m_SpawnWrite.load(std::memory_order_relaxed);
       UINT read = m_SpawnRead.load(std::memory_order_acquire);
       UINT next = (write + 1) % kSpawnRingSize;
       if (next == read)
       {
           return S_OK;
       }
       m_SpawnRing[write] = out;
       m_SpawnWrite.store(next, std::memory_order_release);

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
          UINT write = m_SpawnWrite.load(std::memory_order_relaxed);
          UINT read = m_SpawnRead.load(std::memory_order_acquire);
          UINT next = (write + 1) % kSpawnRingSize;
          if (next == read)
          {
              return S_OK;
          }
          m_SpawnRing[write] = out;
          m_SpawnWrite.store(next, std::memory_order_release);
         
          state.LastPos = curPos;
          state.LastTime = m_TotalTime;
      }
   
        
   }

    return S_OK;
}

void CEffect_DecalStream::Update(_float fTimeDelta)
{
   m_FrameIndex++;
   writeIdx = m_FrameIndex & 1;
   
   ClearLiveList_OnGPU();
   UINT spawnCount= UploadSpawnRequestsToGPU();
   
   DispatchSpawnUpdateCS(fTimeDelta, spawnCount);
   
   ResetDrawArgsOnCPU();
   
   DispatchBuildDrawCS();
}

HRESULT CEffect_DecalStream::Render(CShader* pShader)
{
    if (!pShader)
        return E_FAIL;

    ID3D11ShaderResourceView* nullSRV[16] = {nullptr};
    m_pContext->PSSetShaderResources(0, 16, nullSRV);
    const UINT readIdx = (m_FrameIndex + 1) & 1;

    if (FAILED(pShader->Bind_SRV("g_InstanceDataVS", m_pInstanceSRV[readIdx])))
        return E_FAIL;

    if (FAILED(pShader->Bind_SRV("g_DecalArray", m_pDecalArraySRV)))
        return E_FAIL;

    m_pVIBuffer_Cube->Bind_Buffers();

    pShader->Begin(0);

    m_pContext->DrawIndexedInstancedIndirect(m_pIndirectArgs[readIdx], 0);

    return S_OK;
}


HRESULT CEffect_DecalStream::createGPUStorageBuffers()
{
    if (FAILED(CreateStructuredBuffer(m_MaxDecals, sizeof(GPU_DecalHeader), &m_pDecalSlots, &m_pDecalSlotsSRV,
                                      &m_pDecalSlotsUAV)))
        return E_FAIL;

    if (FAILED(CreateStructuredBuffer(m_MaxDecals, sizeof(UINT), &m_pLiveList, &m_pLiveListSRV, &m_pLiveListUAV)))
        return E_FAIL;
    for (_int i = 0; i < 2; ++i)
    {
        if (FAILED(CreateStructuredBuffer(m_MaxDecals, sizeof(GPU_DecalInstanceData), &m_pInstanceBuffer[i],
                                          &m_pInstanceSRV[i], &m_pInstanceUAV[i])))
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

HRESULT CEffect_DecalStream::createCB()
{
    D3D11_BUFFER_DESC cbd{};
    cbd.ByteWidth = sizeof(CB_DECAL_FRAME);
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = 0; 
    cbd.MiscFlags = 0;
    cbd.StructureByteStride = 0;
    if (FAILED( m_pDevice->CreateBuffer(&cbd, nullptr, &m_pCB_DecalFrame)))
        return E_FAIL;

    D3D11_BUFFER_DESC cb{};
    cb.ByteWidth = 16;
    cb.Usage = D3D11_USAGE_DEFAULT;
    cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    if (FAILED(m_pDevice->CreateBuffer(&cb, nullptr, &m_pCB_ResetArgs)))
        return E_FAIL;

    return S_OK;
}

UINT CEffect_DecalStream::UploadSpawnRequestsToGPU()
{
    if (m_SpawnWrite == m_SpawnRead)
        return 0;

    D3D11_MAPPED_SUBRESOURCE ms{};
    if (FAILED(m_pContext->Map(m_pSpawnUpload, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
        return 0;

    auto* out = reinterpret_cast<DECAL_SPAWN_REQ*>(ms.pData);

    for (UINT i = 0; i < m_MaxSpawnPerFrame; ++i)
    {
        out[i] = {};
        out[i].Valid = 0;
    }

    // 링 버퍼에서 pending 개수 계산
    UINT read = m_SpawnRead.load(std::memory_order_acquire);
    UINT write = m_SpawnWrite.load(std::memory_order_acquire);

    UINT pending = (write >= read) ? (write - read) : (kSpawnRingSize - read + write);

    UINT consumeCount = std::min<UINT>(pending, m_MaxSpawnPerFrame);

    for (UINT i = 0; i < consumeCount; ++i)
    {
        UINT ringIdx = (read + i) % kSpawnRingSize;
        out[i] = m_SpawnRing[ringIdx];
        out[i].Valid = 1;
    }

    m_pContext->Unmap(m_pSpawnUpload, 0);

    // 소비한 만큼 read index 전진
    UINT newRead = (read + consumeCount) % kSpawnRingSize;
    m_SpawnRead.store(newRead, std::memory_order_release);

    return consumeCount;
}

HRESULT CEffect_DecalStream::DispatchSpawnUpdateCS(float dt, UINT spawnCount)
{
    CB_DECAL_FRAME cbData = {dt, spawnCount, m_MaxDecals, 0.f};
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

    ID3D11UnorderedAccessView* nullUAV[2] = {nullptr, nullptr};
    m_pContext->CSSetUnorderedAccessViews(0, 2, nullUAV, initialCounts);

    ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
    m_pContext->CSSetShaderResources(0, 1, nullSRV);

    ID3D11Buffer* nullCB[1] = {nullptr};
    m_pContext->CSSetConstantBuffers(0, 1, nullCB);

    m_pContext->CSSetShader(nullptr, nullptr, 0);

    return S_OK;
}

void CEffect_DecalStream::ResetDrawArgsOnCPU()
{
 UINT indexCount = m_pVIBuffer_Cube->Get_Indexices();

    if (indexCount != lastIndexCount)
     {
         // CB 업데이트 (CPU → GPU)
         m_pContext->UpdateSubresource(m_pCB_ResetArgs, 0, nullptr, &indexCount, 0, 0);
         lastIndexCount = indexCount;
     }

 m_pContext->CSSetShader(m_pCS_ResetArgs, nullptr, 0);

 ID3D11Buffer* cb[] = {m_pCB_ResetArgs};
 m_pContext->CSSetConstantBuffers(0, 1, cb);

 ID3D11UnorderedAccessView* uav[] = {m_pIndirectArgsUAV[writeIdx]};
 UINT initCounts[] = {-1};
 m_pContext->CSSetUnorderedAccessViews(0, 1, uav, initCounts);

 m_pContext->Dispatch(1, 1, 1);

 ID3D11UnorderedAccessView* nullUAV[] = {nullptr};
 m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, initCounts);

 ID3D11Buffer* nullCB[] = {nullptr};
 m_pContext->CSSetConstantBuffers(0, 1, nullCB);

 m_pContext->CSSetShader(nullptr, nullptr, 0);
}


HRESULT CEffect_DecalStream::BuildGlobalDecalArray(_wstring FilePathFmt, _uint TextureCount)
{
    _uint iTotalSlices = 0;

    iTotalSlices += (TextureCount +1); // 노멀 포함
    
    if (iTotalSlices == 0)
        return E_FAIL;

    vector<ID3D11Texture2D*> srcTex(iTotalSlices, nullptr);
    D3D11_TEXTURE2D_DESC firstDesc{};
    bool firstSet = false;
    _uint sliceIdx = 0;

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

   for (_uint i = 1; i <= TextureCount; ++i)
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

    const _uint totalMips = arrDesc.MipLevels;

    for (_uint s = 0; s < iTotalSlices; ++s)
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
        const _uint copyMips = min(totalMips, sdesc.MipLevels);

        for (_uint mip = 0; mip < copyMips; ++mip)
        {
            const _uint dstSub = D3D11CalcSubresource(mip, s, totalMips);
            const _uint srcSub = D3D11CalcSubresource(mip, 0, sdesc.MipLevels);
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

HRESULT CEffect_DecalStream::DispatchBuildDrawCS()
{

    ID3D11ShaderResourceView* srvs[2] = { m_pLiveListSRV, m_pDecalSlotsSRV };
    m_pContext->CSSetShaderResources(0, 2, srvs);

    ID3D11UnorderedAccessView* uavs[2] = {m_pInstanceUAV[writeIdx], m_pIndirectArgsUAV[writeIdx]};
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

void CEffect_DecalStream::ClearLiveList_OnGPU()
{

    ID3D11UnorderedAccessView* uav = m_pLiveListUAV;
    UINT initialCount = (UINT)-1; // 사용 안 하지만 형태 맞추기용

    m_pContext->CSSetShader(m_pCS_ClearLiveList, nullptr, 0);
    m_pContext->CSSetUnorderedAccessViews(0, 1, &uav, &initialCount);

    // 1x1x1 하나만 호출
    m_pContext->Dispatch(1, 1, 1);

    // 상태 정리
    ID3D11UnorderedAccessView* nullUAV = nullptr;
    m_pContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, &initialCount);
    m_pContext->CSSetShader(nullptr, nullptr, 0);
}


void CEffect_DecalStream::Free()
{
    __super::Free();

    Safe_Release(m_pCS_SpawnUpdate);
    Safe_Release(m_pCS_BuildDrawData);
    Safe_Release(m_pCS_ResetArgs);
    
    Safe_Release(m_pDecalSlots);
    Safe_Release(m_pDecalSlotsSRV);
    Safe_Release(m_pDecalSlotsUAV);

    Safe_Release(m_pLiveList);
    Safe_Release(m_pLiveListSRV);
    Safe_Release(m_pLiveListUAV);


    for (int i = 0; i < 2; ++i)
    {
        Safe_Release(m_pInstanceBuffer[i]);
        Safe_Release(m_pInstanceSRV[i]);
        Safe_Release(m_pInstanceUAV[i]);

        Safe_Release(m_pIndirectArgs[i]);
        Safe_Release(m_pIndirectArgsUAV[i]);
    }


    Safe_Release(m_pSpawnUpload);
    Safe_Release(m_pSpawnUploadSRV);

    Safe_Release(m_pCS_ClearLiveList);
    Safe_Release(m_pCB_ResetArgs);
    


    Safe_Release(m_pDecalArraySRV);
}
