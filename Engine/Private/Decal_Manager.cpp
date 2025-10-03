#include "Decal_Manager.h"
#include "Decal.h"
#include "ObjectPool.h"
#include "ViBuffer_DecalCube.h"
#include "Shader.h"
#include "Texture.h"
CDecal_Manager::CDecal_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{pDevice}, m_pContext{pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CDecal_Manager::Initialize()
{
    m_pVIBufferCom = CVIBuffer_DecalCube::Create(m_pDevice, m_pContext);
    return S_OK;
}

HRESULT CDecal_Manager::Add_DecalProto(const wstring& Key, const _tchar* FilePath, const _uint& TexNum)
{
    CDecal* pDecals = CDecal::Create(m_pDevice, m_pContext, FilePath, TexNum);
    // CreateTexture2DArray( FilePath, TexNum);
    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, FilePath, TexNum);
    if (nullptr == pDecals)
        return E_FAIL;
    
    m_ProtoDecal_Map.emplace(Key, pDecals);
    return S_OK;
}


HRESULT CDecal_Manager::Add_Decal(const wstring& Key, const DECAL_DESC* DecalDesc)
{
    auto iter = m_ProtoDecal_Map.find(Key);

    if (iter == m_ProtoDecal_Map.end())
        return S_OK;
    
    CDecal* pDecal = ObjectPool<CDecal>::Pop(*iter->second, const_cast<DECAL_DESC*>(DecalDesc), iter->second->Get_Texture());

    m_Decals.push_back(pDecal);

    return S_OK;
}

HRESULT CDecal_Manager::Update(_float fTimeDelta)
{
    for (auto& pDecals : m_Decals)
    {
        if (pDecals)
            pDecals->Update(fTimeDelta);
    }

    return S_OK;
}

HRESULT CDecal_Manager::Render(CShader* pShader)
{
  //for (auto& pDecals : m_Decals)
  //{
  //    if (pDecals)
  //        pDecals->Render(pShader);
  //}

  m_InstanceData.clear();
    m_InstanceData.reserve(m_Decals.size()); 
  // 1) 데칼 분류
  for (auto& pDecal : m_Decals)
  {
      if (!pDecal)
          continue;
  
          DecalInstanceData inst{};
          inst.WorldInv = pDecal->Get_WorldInv();
          inst.Tangent = pDecal->Get_Tangent();
          inst.Binormal = pDecal->Get_Binormal();
          inst.Normal = pDecal->Get_Normal();
          inst.LifeTime = pDecal->Get_LifeTime();
          inst.DecalTime = pDecal->Get_DecalTime();
          inst.TexIndex = pDecal->Get_TexIndex();
          inst.bNormal = pDecal->Is_Normal();
          inst.DecalType = pDecal->Get_iDecalType();
          inst.DecalPos = pDecal->Get_Pos(); 
          inst.DecalDir = pDecal->Get_Dir(); 
          inst.HalfSize = pDecal->Get_Size(); 
          m_InstanceData.push_back(inst);
  }
  
  if (!m_InstanceData.empty())
  {
      m_pVIBufferCom->Update(m_InstanceData);
  
    m_pTextureCom->Bind_ShaderResource(pShader, "g_DecalArray", 0);
   // pShader->Bind_SRV("g_DecalArray", m_pDecalArraySRV);
    pShader->Begin(0);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();
  }
  
    return S_OK;
}

HRESULT CDecal_Manager::Clear()
{
    for (auto& obj : m_Decals)
    {
        if (!obj)
            continue;

        const auto state = obj->Get_LifeState();
        if (state == OBJ_POOL)
            ObjectPool<CDecal>::Push(obj); // 풀로
        else
            Safe_Release(obj); // 완전 해제
    }
    m_Decals.clear();
    return S_OK;
}

HRESULT CDecal_Manager::Delete()
{
    for (auto it = m_Decals.begin(); it != m_Decals.end();)
    {
        CDecal* obj = *it;
        if (!obj)
        {
            it = m_Decals.erase(it);
            continue;
        }

        const auto state = obj->Get_LifeState();

        if (state == OBJ_POOL)
        {
            ObjectPool<CDecal>::Push(obj);
            it = m_Decals.erase(it);
        }
        else if (state == OBJ_DEAD)
        {
            Safe_Release(obj);
            it = m_Decals.erase(it);
        }
        else
            ++it; 
    }
    return S_OK;
}

CDecal* CDecal_Manager::Find_Prototype(const wstring& ProtoKey)
{
    auto iter = m_ProtoDecal_Map.find(ProtoKey);

    if (iter == m_ProtoDecal_Map.end())
        return nullptr;
    return iter->second;
}

void CDecal_Manager::Preallocate(_wstring ProtoTag, size_t count, void* desc)
{
    ObjectPool<CDecal>::Preallocate(*Find_Prototype(ProtoTag), count, desc);
}

HRESULT CDecal_Manager::CreateTexture2DArray(const _tchar* FilePathFmt, const _uint& TexNum)
{
    if (TexNum == 0)
        return E_FAIL;

    std::vector<ID3D11Texture2D*> srcTex(TexNum, nullptr);

    // 1) 모든 DDS 로드 + ID3D11Texture2D로 캐스팅
    for (UINT i = 0; i < TexNum; ++i)
    {
        _tchar path[MAX_PATH] = TEXT("");
        wsprintf(path, FilePathFmt, i);

        ID3D11Resource* res = nullptr;
        HRESULT hr = CreateDDSTextureFromFile(m_pDevice, path, &res, nullptr);
        if (FAILED(hr) || !res)
        {
            OutputDebugString(L"[Error] DDS 로드 실패\n");
            for (auto* t : srcTex)
                if (t)
                    t->Release();
            return E_FAIL;
        }

        hr = res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&srcTex[i]);
        res->Release();
        if (FAILED(hr) || !srcTex[i])
        {
            OutputDebugString(L"[Error] DDS를 ID3D11Texture2D로 캐스팅 실패\n");
            for (auto* t : srcTex)
                if (t)
                    t->Release();
            return E_FAIL;
        }
    }

    // 2) 공통 스펙 검증 (Format/Width/Height/MipLevels/SampleDesc 동일해야 "모든 mip 복사"가 가능)
    D3D11_TEXTURE2D_DESC first{};
    srcTex[0]->GetDesc(&first);

    for (UINT i = 1; i < TexNum; ++i)
    {
        D3D11_TEXTURE2D_DESC d{};
        srcTex[i]->GetDesc(&d);

        bool same = d.Format == first.Format && d.Width == first.Width && d.Height == first.Height &&
                    d.MipLevels == first.MipLevels && d.SampleDesc.Count == first.SampleDesc.Count &&
                    d.SampleDesc.Quality == first.SampleDesc.Quality;

        if (!same)
        {
            OutputDebugString(L"[Error] Array 슬라이스 간 Format/크기/MipLevels/SampleDesc 불일치. 모든 DDS를 동일 "
                              L"스펙으로 맞추세요.\n");
            for (auto* t : srcTex)
                if (t)
                    t->Release();
            return E_FAIL;
        }
    }

    // 3) Array 텍스처 생성 (모든 mip을 담을 만큼 MipLevels 고정)
    D3D11_TEXTURE2D_DESC arrDesc = first;
    arrDesc.ArraySize = TexNum;
    // 밉을 "복사"만 할 거라면 GENERATE_MIPS 불필요. (자동생성 안함)
    arrDesc.Usage = D3D11_USAGE_DEFAULT;
    arrDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    arrDesc.CPUAccessFlags = 0;
    arrDesc.MiscFlags = 0; // (오토밉이 필요하면 D3D11_RESOURCE_MISC_GENERATE_MIPS + RTV 필요)

    ID3D11Texture2D* arrayTex = nullptr;
    HRESULT hrArr = m_pDevice->CreateTexture2D(&arrDesc, nullptr, &arrayTex);
    if (FAILED(hrArr) || !arrayTex)
    {
        OutputDebugString(L"[Error] Array Texture 생성 실패\n");
        for (auto* t : srcTex)
            if (t)
                t->Release();
        return E_FAIL;
    }

    // 4) 모든 슬라이스의 모든 mip 복사
    //    D3D11CalcSubresource(mip, arraySlice, totalMipLevels)
    const UINT totalMips = arrDesc.MipLevels;
    for (UINT slice = 0; slice < TexNum; ++slice)
    {
        D3D11_TEXTURE2D_DESC sdesc{};
        srcTex[slice]->GetDesc(&sdesc);

        // 안전장치: 혹시라도 소스 mip 수가 다르면 최소치까지만 복사
        const UINT copyMips = min(totalMips, sdesc.MipLevels);

        for (UINT mip = 0; mip < copyMips; ++mip)
        {
            const UINT dstSub = D3D11CalcSubresource(mip, slice, totalMips);
            const UINT srcSub = D3D11CalcSubresource(mip, 0, sdesc.MipLevels); // 단일 텍스처이므로 arraySlice=0

            // 영역 지정 없이 전체 복사: 크기/블록 압축은 런타임이 알아서 처리
            m_pContext->CopySubresourceRegion(arrayTex, dstSub, 0, 0, 0, srcTex[slice], srcSub, nullptr);
        }
    }

    // 5) SRV 생성 (모든 mip 노출)
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = arrDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = totalMips;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = TexNum;

    ID3D11ShaderResourceView* srv = nullptr;
    HRESULT hrSrv = m_pDevice->CreateShaderResourceView(arrayTex, &srvDesc, &srv);
    arrayTex->Release();

    for (auto* t : srcTex)
        if (t)
            t->Release();

    if (FAILED(hrSrv) || !srv)
    {
        OutputDebugString(L"[Error] SRV 생성 실패\n");
        return E_FAIL;
    }

    m_pDecalArraySRV = srv;
    return S_OK;
}



CDecal_Manager* CDecal_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDecal_Manager* pInstance = new CDecal_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CDecal_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDecal_Manager::Free()
{
    __super::Free();

    ObjectPool<CDecal>::ClearAll();

    for (auto& obj : m_Decals) Safe_Release(obj);
    m_Decals.clear();

    for (auto& kv : m_ProtoDecal_Map) { Safe_Release(kv.second); };

    m_ProtoDecal_Map.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pDecalArraySRV);
}
