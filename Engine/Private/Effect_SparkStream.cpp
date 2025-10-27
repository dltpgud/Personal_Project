#include "Effect_SparkStream.h"
#include "Shader.h"

static constexpr UINT THREADS = 64;

CEffect_SparkStream::CEffect_SparkStream(ID3D11Device* dev, ID3D11DeviceContext* ctx) : CEffectStream{dev, ctx}
{
}

HRESULT CEffect_SparkStream::Initialize(void* pArg)
{
    if (pArg)
        m_desc = *static_cast<DESC*>(pArg);

    m_iSpriteCount = m_desc.iSpriteCount;

    if (m_desc.pTexturePath)
        m_pTexture = CTexture::Create(m_pDevice, m_pContext, m_desc.pTexturePath, 1);

    createShaders();
    createBuffers();
    createCB();

    return S_OK;
}

HRESULT CEffect_SparkStream::createShaders()
{
    ID3DBlob* csBlob = nullptr;
    ID3DBlob* errBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(L"../Bin/ShaderFiles/SparkUpdateCS.hlsl", nullptr,
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE, "CSMain", "cs_5_0", 0, 0, &csBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob)
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
        return hr;
    }

    m_pDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &m_pCS_Update);
    Safe_Release(csBlob);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    m_pShader =
        CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/SparkRender.hlsl", layout, _countof(layout));
    return S_OK;
}

HRESULT CEffect_SparkStream::createBuffers()
{
    std::vector<SPAWN_REQUEST> init(m_desc.maxParticles);
    for (auto& p : init) p.valid = 0;

    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = sizeof(SPAWN_REQUEST) * m_desc.maxParticles;
    bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = sizeof(SPAWN_REQUEST);
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = init.data();

    m_pDevice->CreateBuffer(&bd, &sd, &m_pParticleBuf);

    D3D11_SHADER_RESOURCE_VIEW_DESC srv{};
    srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srv.Format = DXGI_FORMAT_UNKNOWN;
    srv.Buffer.NumElements = m_desc.maxParticles;
    m_pDevice->CreateShaderResourceView(m_pParticleBuf, &srv, &m_SRV_Particle);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uav{};
    uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uav.Format = DXGI_FORMAT_UNKNOWN;
    uav.Buffer.NumElements = m_desc.maxParticles;
    m_pDevice->CreateUnorderedAccessView(m_pParticleBuf, &uav, &m_UAV_Particle);

    return S_OK;
}

HRESULT CEffect_SparkStream::createCB()
{
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = 64;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_pDevice->CreateBuffer(&bd, nullptr, &m_pCSCB);
    m_pDevice->CreateBuffer(&bd, nullptr, &m_pVSCB);
    return S_OK;
}

HRESULT CEffect_SparkStream::Trigger_Effect(void* pArg)
{
    if (!pArg)
        return S_OK;
    SPAWN_REQUEST req = *static_cast<SPAWN_REQUEST*>(pArg);
    m_spawnQueue.push_back(req);
    return S_OK;
}

void CEffect_SparkStream::Update(_float dt)
{
    if (m_spawnQueue.empty())
        return;

    m_pContext->CSSetShader(m_pCS_Update, nullptr, 0);

    D3D11_MAPPED_SUBRESOURCE ms{};
    if (SUCCEEDED(m_pContext->Map(m_pCSCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
    {
        float data[4] = {dt, m_desc.fadeSpeed, m_desc.lifeTime, (float)m_desc.maxParticles};
        memcpy(ms.pData, data, sizeof(data));
        m_pContext->Unmap(m_pCSCB, 0);
    }
    m_pContext->CSSetConstantBuffers(0, 1, &m_pCSCB);

    ID3D11UnorderedAccessView* uavs[1] = {m_UAV_Particle};
    m_pContext->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);

    UINT groups = (m_desc.maxParticles + THREADS - 1) / THREADS;
    m_pContext->Dispatch(groups, 1, 1);

    ID3D11UnorderedAccessView* nullUAV[1] = {nullptr};
    m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
    m_pContext->CSSetShader(nullptr, nullptr, 0);

    m_spawnQueue.clear();
}

HRESULT CEffect_SparkStream::Render(CShader*)
{
    //VS_PERFRAME cb{};
    //XMStoreFloat4x4(&cb.g_View, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
    //XMStoreFloat4x4(&cb.g_Proj, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
    //
    //m_pShader->Bind_Matrix("g_View", &cb.g_View);
    //m_pShader->Bind_Matrix("g_Proj", &cb.g_Proj);
    //m_pTexture->Bind_ShaderResource(m_pShader, "g_SparkTex", 0);
    //m_pShader->Bind_SRV("g_Particles", m_SRV_Particle);
    //m_pShader->Begin(0);
    //m_pContext->Draw(m_desc.maxParticles, 0);
    return S_OK;
}

void CEffect_SparkStream::Free()
{
    __super::Free();
    Safe_Release(m_pParticleBuf);
    Safe_Release(m_SRV_Particle);
    Safe_Release(m_UAV_Particle);
    Safe_Release(m_pCSCB);
    Safe_Release(m_pVSCB);
    Safe_Release(m_pCS_Update);
    Safe_Release(m_pShader);
    Safe_Release(m_pTexture);
}
