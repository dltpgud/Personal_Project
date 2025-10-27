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
    CDecal* pDecals = CDecal::Create();
    if (nullptr == pDecals)
        return E_FAIL;

    DecalProtoInfo info;
    info.filePathFmt = FilePath;
    info.texCount = TexNum-1;
    m_ProtoInfo.insert(make_pair(Key, info));
    m_ProtoDecal_Map.emplace(Key, pDecals);
 
    return S_OK;
}

HRESULT CDecal_Manager::Add_Decal(const wstring& Key, const DECAL_DESC* DecalDesc)
{
    auto iter = m_ProtoDecal_Map.find(Key);

    if (iter == m_ProtoDecal_Map.end())
        return S_OK;
    
    CDecal* pDecal = ObjectPool<CDecal>::Pop(iter->second, const_cast<DECAL_DESC*>(DecalDesc));

    m_Decals.push_back(pDecal);

    return S_OK;
}

HRESULT CDecal_Manager::Update(_float fTimeDelta)
{
    m_InstanceData.clear();
    if (m_InstanceData.capacity() < m_Decals.size())
        m_InstanceData.reserve(m_Decals.size() * 2);

    for (auto& pDecals : m_Decals)
    {
        if (pDecals)
        {
            pDecals->Update(fTimeDelta);

            DecalInstanceData inst{};
            inst.WorldInv = pDecals->Get_WorldInv();
            inst.LifeTime = pDecals->Get_LifeTime();
            inst.DecalTime = pDecals->Get_DecalTime();
            inst.TexIndex = pDecals->Get_TexIndex();
            inst.bNormal = pDecals->Is_Normal();
            inst.DecalType = pDecals->Get_iDecalType();
            inst.DecalPos = pDecals->Get_Pos();
            inst.DecalDir = pDecals->Get_Dir(); // ssd면 dir 아니면 normal
            inst.HalfSize = pDecals->Get_Size();
            
            
            const wstring& key = pDecals->Get_ProtoKey();
            const int localSlice = pDecals->Get_TexIndex(); // 프로토 내부의 컬러 인덱스 (0부터)
            int protoOffset = 0;
            auto it = m_ProtoOffset.find(key);
            if (it != m_ProtoOffset.end())
                protoOffset = (int)it->second;

            // 해당 프로토 시작 슬라이스 (여기서 +0은 노멀, +1부터 컬러)
            inst.ProtoIndex = protoOffset + localSlice;
            
            m_InstanceData.push_back(inst);
        }
    }

    m_pVIBufferCom->Update(m_InstanceData);
    
    return S_OK;
}

HRESULT CDecal_Manager::Render(CShader* pShader)
{ 
    pShader->Bind_SRV("g_DecalArray", m_pDecalArraySRV);
    pShader->Begin(0);
    m_pVIBufferCom->Bind_Buffers();
    m_pVIBufferCom->Render();

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
       
    }

    m_Decals.clear();
    return S_OK;
}

HRESULT CDecal_Manager::Delete()
{
    for (auto it = m_Decals.begin(); it != m_Decals.end();)
    {
        CDecal* obj = *it;
    
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
    ObjectPool<CDecal>::Preallocate(Find_Prototype(ProtoTag), count, desc);
}

HRESULT CDecal_Manager::BuildGlobalDecalArray()
{
    WRITE_LOCK;
    m_ProtoOffset.clear();
    m_TotalSlices = 0;
    Safe_Release(m_pDecalArraySRV);

    // 1. 각 프로토타입의 시작 오프셋 계산 (노멀 1 + 컬러 n)
    for (auto it = m_ProtoInfo.begin(); it != m_ProtoInfo.end(); ++it)
    {
        const std::wstring& key = it->first;
        const DecalProtoInfo& info = it->second;

        m_ProtoOffset[key] = m_TotalSlices;
        m_TotalSlices += (info.texCount + 1); // 노멀 포함
    }

    if (m_TotalSlices == 0)
        return E_FAIL;

    vector<ID3D11Texture2D*> srcTex(m_TotalSlices, nullptr);
    D3D11_TEXTURE2D_DESC firstDesc{};
    bool firstSet = false;
    UINT sliceIdx = 0;

    // 2. DDS 파일 로드
    for (auto it = m_ProtoInfo.begin(); it != m_ProtoInfo.end(); ++it)
    {
        const _wstring& key = it->first;
        const DecalProtoInfo& info = it->second;

        // ---- (1) Normal Map (항상 0번) ----
        {
            _tchar path[MAX_PATH] = TEXT("");
            wsprintf(path, info.filePathFmt.c_str(), 0);

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

        // ---- (2) Color DDS ----
        for (UINT i = 1; i <= info.texCount; ++i)
        {
            _tchar path[MAX_PATH] = TEXT("");
            wsprintf(path, info.filePathFmt.c_str(), i);

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
    }

    // 3. Array Texture 생성
    D3D11_TEXTURE2D_DESC arrDesc = firstDesc;
    arrDesc.ArraySize = m_TotalSlices;
    arrDesc.Usage = D3D11_USAGE_DEFAULT;
    arrDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    arrDesc.CPUAccessFlags = 0;
    arrDesc.MiscFlags = 0;

    ID3D11Texture2D* arrayTex = nullptr;
    HRESULT hrArray = m_pDevice->CreateTexture2D(&arrDesc, nullptr, &arrayTex);
    if (FAILED(hrArray) || !arrayTex)
    {
        for (size_t j = 0; j < srcTex.size(); ++j) Safe_Release(srcTex[j]);
        return E_FAIL;
    }

    const UINT totalMips = arrDesc.MipLevels;

    // 4. 복사
    for (UINT s = 0; s < m_TotalSlices; ++s)
    {
        if (!srcTex[s])
        {
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

    // 5. SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = arrDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = totalMips;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = m_TotalSlices;

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
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pDecalArraySRV); 
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
