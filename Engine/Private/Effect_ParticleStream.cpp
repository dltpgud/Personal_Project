#include "Effect_ParticleStream.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "Texture.h"
#include <iomanip>
CEffect_ParticleStream::CEffect_ParticleStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEffectStream(pDevice, pContext)
{
}

CEffect_ParticleStream* CEffect_ParticleStream::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CEffect_ParticleStream* pInstance = new CEffect_ParticleStream(pDevice, pContext);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CEffect_ParticleStream");
        Safe_Release(pInstance);
    }
    return pInstance;
}

HRESULT CEffect_ParticleStream::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    PARTICLESTREAM_DESC* pDesc = static_cast<PARTICLESTREAM_DESC*>(pArg);
    m_Desc = *pDesc;
    m_bLoop = m_Desc.isLoop;
    m_bActive = false;
    m_AccumLife = 0.f;
    m_Desc.vCenter = _float3(0.f, 0.f, 0.f);
    m_Desc.vPivot = _float3(0.f, 0.f, 0.f);
    m_vFollowOffset = _float3(0.f, 0.f, 0.f);
    // -------------------------------------------------------------------
    // 텍스처 로드 (스프라이트 시트)
    // -------------------------------------------------------------------
    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, pDesc->FilePathFmt.c_str(), pDesc->TextureCount);
    if (!m_pTextureCom)
        return E_FAIL;

    // -------------------------------------------------------------------
    // 1포인트 버퍼 생성 (빌보드로 확장)
    // -------------------------------------------------------------------
   // m_VIBuffer = CVIBuffer_Point::Create(m_pDevice, m_pContext);
   // if (!m_VIBuffer)
   //     return E_FAIL;
   //
    // -------------------------------------------------------------------
    // GPU 스토리지 버퍼 생성
    // -------------------------------------------------------------------
    if (FAILED(createGPUStorageBuffers()))
        return E_FAIL;

    // -------------------------------------------------------------------
    // DrawIndirect용 RAW버퍼
    // -------------------------------------------------------------------
    if (FAILED(CreateRawBuffer(sizeof(UINT) * 4, &m_pIndirectArgs, &m_pIndirectArgsUAV, true)))
        return E_FAIL;

    // -------------------------------------------------------------------
    // 상수버퍼
    // -------------------------------------------------------------------
    if (FAILED(createCB()))
        return E_FAIL;

    // -------------------------------------------------------------------
    // 컴퓨트 셰이더 로드
    // -------------------------------------------------------------------
    if (FAILED(Create_CS(L"../Bin/ShaderFiles/CS_ParticleSpawnUpdate.hlsl", "CSMain", &m_pCS_SpawnUpdate)))
        return E_FAIL;
    if (FAILED(Create_CS(L"../Bin/ShaderFiles/CS_ParticleBuildInstance.hlsl", "CSMain", &m_pCS_BuildDrawData)))
        return E_FAIL;

    // -------------------------------------------------------------------
    // 렌더 셰이더 로드 (스프라이트 시트 포함)
    // -------------------------------------------------------------------
    m_pShader = CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_Particle.hlsl", nullptr, 0);
    if (!m_pShader)
        return E_FAIL;


    // ViewProj CB
    {
        D3D11_BUFFER_DESC bd{};
        bd.ByteWidth = sizeof(_float4x4) * 2 + sizeof(_float3) + sizeof(_float); // 안전하게 144바이트
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &m_pCB_ViewProj)))
            return E_FAIL;
    }

    // SpriteInfo CB
    {
        D3D11_BUFFER_DESC bd{};
        bd.ByteWidth = sizeof(UINT) * 4;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
     if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &m_pCB_SpriteInfo)))
         return E_FAIL;
    }



    return S_OK;
}

HRESULT CEffect_ParticleStream::Trigger_Effect(void* pArg, _float fTimeDelta)
{
    PARTICLE_TRIGGER_DESC req = *static_cast<PARTICLE_TRIGGER_DESC*>(pArg);
    m_pParentMatrix = req.pParentMatrix;
    m_vFollowOffset = req.vOffset;

    m_bActive = true;
    m_AccumLife = 0.f;
  
    return S_OK;
}

void CEffect_ParticleStream::Update(_float fTimeDelta)
{
    if (!m_bActive)
        return;

    UpdateCenterFromParent();

    if (!m_bLoop)
    {
        m_AccumLife += fTimeDelta;
        if (m_AccumLife > m_Desc.vLifeTime.x + m_Desc.vLifeTime.y)
        {
            m_bActive = false;
            return;
        }
    }

    ResetDrawArgsOnCPU();
    DispatchSpawnUpdateCS(fTimeDelta);
    DispatchBuildDrawCS();

    if (m_pGameInstance->Get_DIKeyDown(DIK_0))
        Debug_ReadParticlesFromGPU();

}

HRESULT CEffect_ParticleStream::Render(CShader* pShader)
{
    //if (!m_bActive)
    //    return E_FAIL;
    //
    //// 뷰/프로젝션 매트릭스
    //CB_VIEWPROJ cbViewProj{};
    //cbViewProj.g_View = *m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
    //cbViewProj.g_Proj = *m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
    //
    //_float4 CamPos = *m_pGameInstance->Get_CamPosition();
    //
    //cbViewProj.g_CamPosWS = {CamPos.x, CamPos.y, CamPos.z};
    //cbViewProj.g_fCamFar = *m_pGameInstance->Get_CamFar();
    //m_pContext->UpdateSubresource(m_pCB_ViewProj, 0, nullptr, &cbViewProj, 0, 0);
    //
    //// 스프라이트 시트 정보
    //CB_PARTICLE_SPRITE_INFO cbSprite{};
    //cbSprite.g_Columns = 4; // 예시: 4x4 시트
    //cbSprite.g_Rows = 2;
    //cbSprite.g_TotalFrames = cbSprite.g_Columns * cbSprite.g_Rows;
    //cbSprite.g_UseSpriteSheet = (cbSprite.g_TotalFrames > 1) ? 1 : 0;
    //m_pContext->UpdateSubresource(m_pCB_SpriteInfo, 0, nullptr, &cbSprite, 0, 0);
    //
    //// 상수버퍼 바인딩 (VS, PS)
    //m_pContext->VSSetConstantBuffers(0, 1, &m_pCB_ViewProj);
    //m_pContext->PSSetConstantBuffers(0, 1, &m_pCB_ViewProj);
    //m_pContext->VSSetConstantBuffers(1, 1, &m_pCB_SpriteInfo);
    //m_pContext->PSSetConstantBuffers(1, 1, &m_pCB_SpriteInfo);
    //
    //// SRV 초기화 및 바인딩
    //ID3D11ShaderResourceView* nullSRV[16] = {nullptr};
    //m_pContext->PSSetShaderResources(0, 16, nullSRV);
    //m_pShader->Bind_SRV("g_InstanceDataVS", m_pInstanceSRV);
    //m_pTextureCom->Bind_ShaderResource(m_pShader, "g_ParticleTexture", 0);
    //
    //// 버퍼 & 드로우
   //// m_VIBuffer->Bind_Buffers();
   //// m_pShader->Begin(1);
   //// m_pContext->DrawInstancedIndirect(m_pIndirectArgs, 0);
    //
    //// 1) 인덱스 버퍼 해제
    //m_pContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    //
    //// 2) 빈(또는 dummy) VB로 세팅하거나 아예 0개로
    //UINT stride = 0;
    //UINT offset = 0;
    //ID3D11Buffer* nullVB = nullptr;
    //m_pContext->IASetVertexBuffers(0, 1, &nullVB, &stride, &offset);
    //
    //// 3) 우리가 원하는 토폴로지
    //m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    //
    //m_pShader->Begin(1);
    //m_pContext->DrawInstancedIndirect(m_pIndirectArgs, 0);

        if (!m_bActive)
        return E_FAIL;

    //==============================
    // 1️⃣ View / Projection 상수 세팅
    //==============================
    CB_VIEWPROJ cbViewProj{};
    cbViewProj.g_View = *m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
    cbViewProj.g_Proj = *m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

    _float4 CamPos = *m_pGameInstance->Get_CamPosition();
    cbViewProj.g_CamPosWS = {CamPos.x, CamPos.y, CamPos.z};
    cbViewProj.g_fCamFar = *m_pGameInstance->Get_CamFar();
    m_pContext->UpdateSubresource(m_pCB_ViewProj, 0, nullptr, &cbViewProj, 0, 0);

    //==============================
    // 2️⃣ Sprite Sheet 상수 세팅
    //==============================
    CB_PARTICLE_SPRITE_INFO cbSprite{};
    cbSprite.g_Columns = 4;
    cbSprite.g_Rows = 2;
    cbSprite.g_TotalFrames = cbSprite.g_Columns * cbSprite.g_Rows;
    cbSprite.g_UseSpriteSheet = (cbSprite.g_TotalFrames > 1) ? 1 : 0;
    m_pContext->UpdateSubresource(m_pCB_SpriteInfo, 0, nullptr, &cbSprite, 0, 0);

    // 상수버퍼 바인딩
    m_pContext->VSSetConstantBuffers(0, 1, &m_pCB_ViewProj);
    m_pContext->PSSetConstantBuffers(0, 1, &m_pCB_ViewProj);
    m_pContext->VSSetConstantBuffers(1, 1, &m_pCB_SpriteInfo);
    m_pContext->PSSetConstantBuffers(1, 1, &m_pCB_SpriteInfo);

    //==============================
    // 3️⃣ 텍스처 / 인스턴스 버퍼 바인딩
    //==============================
    ID3D11ShaderResourceView* nullSRV[16] = {nullptr};
    m_pContext->PSSetShaderResources(0, 16, nullSRV);

    // Instance SRV 바인딩
    m_pShader->Bind_SRV("g_InstanceDataVS", m_pInstanceSRV);
    m_pTextureCom->Bind_ShaderResource(m_pShader, "g_ParticleTexture", 0);

    //==============================
    // 4️⃣ Input Assembler 설정
    //==============================
    // 우리는 VS에서 SV_VertexID를 사용하므로 실제 VB는 필요 없음.
    m_pContext->IASetInputLayout(nullptr); // Shader::Begin()에서 nullptr 레이아웃 쓸 수 있게
    m_pContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

    // 정점 버퍼 없음
    UINT stride = 0;
    UINT offset = 0;
    ID3D11Buffer* nullVB = nullptr;
    m_pContext->IASetVertexBuffers(0, 1, &nullVB, &stride, &offset);

    // POINTLIST (GS가 쿼드 확장)
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);


    //==============================
    // 5️⃣ 셰이더 바인딩 & 드로우
    //==============================
    m_pShader->Begin(0); // pass index = 0
    m_pContext->DrawInstancedIndirect(m_pIndirectArgs, 0);
    return S_OK;
}

void CEffect_ParticleStream::Debug_ReadParticlesFromGPU()
{
#ifdef _DEBUG
    if (!m_pParticleBuffer)
    {
        std::cout << "[ParticleStream] No particle buffer found." << std::endl;
        return;
    }

    // 1️⃣ 스테이징 버퍼 생성 (CPU 접근용)
    D3D11_BUFFER_DESC desc{};
    m_pParticleBuffer->GetDesc(&desc);

    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;

    ID3D11Buffer* pStaging = nullptr;
    HRESULT hr = m_pDevice->CreateBuffer(&desc, nullptr, &pStaging);
    if (FAILED(hr))
    {
        std::cout << "[ParticleStream] Failed to create staging buffer." << std::endl;
        return;
    }

    // 2️⃣ GPU → CPU 복사
    m_pContext->CopyResource(pStaging, m_pParticleBuffer);

    // 3️⃣ 맵핑해서 데이터 읽기
    D3D11_MAPPED_SUBRESOURCE mapped{};
    hr = m_pContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped);
    if (FAILED(hr))
    {
        std::cout << "[ParticleStream] Failed to map particle buffer." << std::endl;
        Safe_Release(pStaging);
        return;
    }

    GPU_Particle* pData = reinterpret_cast<GPU_Particle*>(mapped.pData);

    // 4️⃣ 일부만 출력 (너무 많으면 콘솔 터지니까 앞 10개만)
    std::cout << "================ GPU PARTICLE DEBUG ================" << std::endl;
    for (UINT i = 0; i < min(10u, m_Desc.iNumInstance); ++i)
    {
        const auto& p = pData[i];
        std::cout << std::fixed << std::setprecision(2) << "[" << i << "] Pos(" << p.vPos.x << ", " << p.vPos.y
                  << ", " << p.vPos.z << ") "
                  << " Life: " << p.fLife << " MaxLife: " << p.fMaxLife << " Frame: " << p.FrameIndex << std::endl;
    }
    std::cout << "===================================================" << std::endl;

    // 5️⃣ 정리
    m_pContext->Unmap(pStaging, 0);
    Safe_Release(pStaging);
#endif
}


void CEffect_ParticleStream::Debug_ReadInstancesFromGPU()
{
#ifdef _DEBUG
    if (!m_pInstanceBuffer)
    {
        std::cout << "[ParticleStream] No instance buffer found." << std::endl;
        return;
    }

    // 1️⃣ 스테이징 버퍼 생성
    D3D11_BUFFER_DESC desc{};
    m_pInstanceBuffer->GetDesc(&desc);

    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;

    ID3D11Buffer* pStaging = nullptr;
    HRESULT hr = m_pDevice->CreateBuffer(&desc, nullptr, &pStaging);
    if (FAILED(hr))
    {
        std::cout << "[ParticleStream] Failed to create staging buffer for instances." << std::endl;
        return;
    }

    // 2️⃣ GPU → CPU 복사
    m_pContext->CopyResource(pStaging, m_pInstanceBuffer);

    // 3️⃣ 매핑
    D3D11_MAPPED_SUBRESOURCE mapped{};
    hr = m_pContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped);
    if (FAILED(hr))
    {
        std::cout << "[ParticleStream] Failed to map instance buffer." << std::endl;
        Safe_Release(pStaging);
        return;
    }

    // 4️⃣ 데이터 읽기
    struct GPU_ParticleInstance
    {
        _float3 vPosition;
        float fSize;
        _float4 vColor;
        UINT FrameIndex;
        _float3 _Pad0;
    };

    GPU_ParticleInstance* pData = reinterpret_cast<GPU_ParticleInstance*>(mapped.pData);

    std::cout << "================ GPU INSTANCE DEBUG ================" << std::endl;
    for (UINT i = 0; i < min(10u, m_Desc.iNumInstance); ++i)
    {
        const auto& inst = pData[i];
        std::cout << std::fixed << std::setprecision(2) << "[" << i << "] Pos(" << inst.vPosition.x << ", "
                  << inst.vPosition.y << ", " << inst.vPosition.z << ") "
                  << " Size: " << inst.fSize << " Frame: " << inst.FrameIndex << " Color: (" << inst.vColor.x << ", "
                  << inst.vColor.y << ", " << inst.vColor.z << ", " << inst.vColor.w << ")" << std::endl;
    }
    std::cout << "===================================================" << std::endl;

    // 5️⃣ 정리
    m_pContext->Unmap(pStaging, 0);
    Safe_Release(pStaging);
#endif
}



HRESULT CEffect_ParticleStream::createGPUStorageBuffers()
{
    if (FAILED(CreateStructuredBuffer(m_Desc.iNumInstance, sizeof(GPU_Particle), &m_pParticleBuffer, &m_pParticleSRV,
                                      &m_pParticleUAV)))
        return E_FAIL;

    if (FAILED(CreateStructuredBuffer(m_Desc.iNumInstance, sizeof(GPU_ParticleInstance), &m_pInstanceBuffer,
                                      &m_pInstanceSRV, &m_pInstanceUAV)))
        return E_FAIL;

    return S_OK;
}

//====================================================================================
// 상수버퍼 생성
//====================================================================================
HRESULT CEffect_ParticleStream::createCB()
{
    D3D11_BUFFER_DESC cbd{};
    cbd.ByteWidth = sizeof(CB_PARTICLE_FRAME);
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    return m_pDevice->CreateBuffer(&cbd, nullptr, &m_pCB_ParticleFrame);
}

//====================================================================================
// 파티클 스폰 / 업데이트 CS
//====================================================================================
HRESULT CEffect_ParticleStream::DispatchSpawnUpdateCS(_float dt)
{
    CB_PARTICLE_FRAME cb{};
    cb.g_DeltaTime = dt;
    cb.g_vCenter = m_Desc.vCenter;
    cb.g_vRange = m_Desc.vRange;
    cb.g_vSize = m_Desc.vSize;
    cb.g_vSpeed = m_Desc.vSpeed;
    cb.g_vLife = m_Desc.vLifeTime;
    cb.g_bLoop = m_bLoop ? 1.f : 0.f;
    cb.g_iMaxParticle = m_Desc.iNumInstance;

    m_pContext->UpdateSubresource(m_pCB_ParticleFrame, 0, nullptr, &cb, 0, 0);

    m_pContext->CSSetShader(m_pCS_SpawnUpdate, nullptr, 0);
    m_pContext->CSSetConstantBuffers(0, 1, &m_pCB_ParticleFrame);

    ID3D11UnorderedAccessView* uavs[1] = {m_pParticleUAV};
    m_pContext->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);

    UINT groups = (m_Desc.iNumInstance + THREADS - 1) / THREADS;
    m_pContext->Dispatch(groups, 1, 1);

    if (m_pGameInstance->Get_DIKeyDown(DIK_0))
    Debug_ReadInstancesFromGPU();

    ID3D11UnorderedAccessView* nullUAV[1] = {nullptr};
    m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
    m_pContext->CSSetShader(nullptr, nullptr, 0);

    return S_OK;
}

//====================================================================================
// 인스턴스 빌드 CS (DrawIndirectArgs 포함)
//====================================================================================
HRESULT CEffect_ParticleStream::DispatchBuildDrawCS()
{
    // CB 바인딩 (g_iMaxParticle 사용)
    m_pContext->CSSetConstantBuffers(0, 1, &m_pCB_ParticleFrame);

    // 입력 SRV
    ID3D11ShaderResourceView* srvs[1] = {m_pParticleSRV};
    m_pContext->CSSetShaderResources(0, 1, srvs);

    // 출력 UAV
    ID3D11UnorderedAccessView* uavs[2] = {m_pInstanceUAV, m_pIndirectArgsUAV};
    m_pContext->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);

    m_pContext->CSSetShader(m_pCS_BuildDrawData, nullptr, 0);

    UINT groups = (m_Desc.iNumInstance + THREADS - 1) / THREADS;
    m_pContext->Dispatch(groups, 1, 1);

    // 정리
    ID3D11UnorderedAccessView* nullUAV[2] = {nullptr, nullptr};
    ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
    m_pContext->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);
    m_pContext->CSSetShaderResources(0, 1, nullSRV);
    m_pContext->CSSetShader(nullptr, nullptr, 0);

    return S_OK;
}

//====================================================================================
// DrawIndirect 초기화
//====================================================================================
void CEffect_ParticleStream::ResetDrawArgsOnCPU()
{
    UINT initArgs[4] = {1, 0, 0, 0};
    m_pContext->UpdateSubresource(m_pIndirectArgs, 0, nullptr, initArgs, 0, 0);
}

//====================================================================================
// 부모 중심 좌표 추적
//====================================================================================
void CEffect_ParticleStream::UpdateCenterFromParent()
{
    if (!m_pParentMatrix)
        return;

    _float3 parentPos(m_pParentMatrix->_41, m_pParentMatrix->_42, m_pParentMatrix->_43);

    m_Desc.vCenter =
        _float3(parentPos.x + m_Desc.vPivot.x + m_vFollowOffset.x, parentPos.y + m_Desc.vPivot.y + m_vFollowOffset.y,
                parentPos.z + m_Desc.vPivot.z + m_vFollowOffset.z);
}

//====================================================================================
// 자원 해제
//====================================================================================
void CEffect_ParticleStream::Free()
{
    __super::Free();

    Safe_Release(m_pCS_SpawnUpdate);
    Safe_Release(m_pCS_BuildDrawData);

    Safe_Release(m_pParticleBuffer);
    Safe_Release(m_pParticleSRV);
    Safe_Release(m_pParticleUAV);

    Safe_Release(m_pInstanceBuffer);
    Safe_Release(m_pInstanceSRV);
    Safe_Release(m_pInstanceUAV);

    Safe_Release(m_pIndirectArgs);
    Safe_Release(m_pIndirectArgsUAV);

    Safe_Release(m_pCB_ParticleFrame);
    //Safe_Release(m_VIBuffer);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShader);

    Safe_Release(m_pCB_ViewProj);
    Safe_Release(m_pCB_SpriteInfo);

}
