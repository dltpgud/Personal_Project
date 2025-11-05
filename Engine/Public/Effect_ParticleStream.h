#pragma once
#include "EffectStream.h"

//====================================================================================
// GPU 구조체들 (C++ <-> HLSL 동일 레이아웃 유지)
//====================================================================================

// 파티클 시뮬레이션용 구조체 (CS_ParticleSpawnUpdate.hlsl과 일치)
__declspec(align(16)) struct GPU_Particle
{
    _float3 vPos;
    _float fLife; // 남은 수명
    _float3 vVel;
    _float fMaxLife; // 초기 수명
    _float2 vSize;
    _float2 _pad0;  // 파티클 크기
    _float4 vColor; // 파티클 컬러
    _uint FrameIndex;
    _float3 _pad1; // 현재 프레임 인덱스 (애니메이션용)
};
// sizeof(GPU_Particle) = 80 (16바이트 배수)

// 렌더 인스턴스 데이터 (CS_ParticleBuildInstance.hlsl과 일치)
__declspec(align(16)) struct GPU_ParticleInstance
{
    _float3 vPosition;
    _float fSize;   // 중심좌표 + 크기
    _float4 vColor; // 컬러
    _uint FrameIndex;
    _float3 _Pad0; // 애니메이션 프레임
};
// sizeof(GPU_ParticleInstance) = 48

// 상수 버퍼 (CB_PARTICLE_FRAME)
__declspec(align(16)) struct CB_PARTICLE_FRAME
{
    _float3 g_vCenter;
    _float g_DeltaTime;
    _float3 g_vRange;
    _float g_bLoop;
    _float2 g_vSize;
    _float2 g_vSpeed;
    _float2 g_vLife;
    _uint g_iMaxParticle;
    _float pad;
};

//====================================================================================
// View/Projection 상수버퍼 구조체 (b0과 일치)
//====================================================================================
__declspec(align(16)) struct CB_VIEWPROJ
{
    _float4x4 g_View;   // 64 bytes
    _float4x4 g_Proj;   // 64 bytes
    _float3 g_CamPosWS; // 12 bytes
    _float g_fCamFar;   // 4 bytes
};
// sizeof(CB_VIEWPROJ) = 144 bytes

//====================================================================================
// 파티클 스프라이트 시트 정보 (b1과 일치)
//====================================================================================
__declspec(align(16)) struct CB_PARTICLE_SPRITE_INFO
{
    _uint g_Columns;        // 가로 프레임 수
    _uint g_Rows;           // 세로 프레임 수
    _uint g_TotalFrames;    // 전체 프레임 수 (Columns * Rows)
    _uint g_UseSpriteSheet; // 1 = 사용, 0 = 단일 텍스처
};
// sizeof(CB_PARTICLE_SPRITE_INFO) = 16 bytes


//====================================================================================
// DESC 구조체들
//====================================================================================
struct PARTICLESTREAM_DESC
{
    _uint iNumInstance; // 최대 파티클 수
    _float3 vCenter;    // 중심 위치
    _float3 vRange;     // 스폰 범위
    _float2 vSize;      // 파티클 크기 (min,max)
    _float2 vSpeed;     // 속도 범위
    _float2 vLifeTime;  // 수명 (기본,랜덤)
    _float3 vPivot;     // 부모 기준 피벗
    _bool isLoop;       // 루프 여부

    _wstring FilePathFmt; // L"../Bin/Resources/T_Explosion_%d.dds"
    _uint TextureCount;   // 텍스처 프레임 수
};

struct PARTICLE_TRIGGER_DESC
{
     _float4x4* pParentMatrix = nullptr; // 부모 행렬
    _float3 vOffset = {0.f, 0.f, 0.f};
    _bool bForceRestart = true;
};

//====================================================================================
// CEffect_ParticleStream 클래스
//====================================================================================
BEGIN(Engine)
class CVIBuffer_Point;
class CTexture;

class ENGINE_DLL CEffect_ParticleStream final : public CEffectStream
{
public:
    explicit CEffect_ParticleStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEffect_ParticleStream() = default;

public:
    static CEffect_ParticleStream* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual HRESULT Initialize(void* pArg);

public:
    // 트리거 (외부에서 부모행렬 기반 재생)
    virtual HRESULT Trigger_Effect(void* pArg, _float fTimeDelta) override;
    // 프레임 업데이트 / 렌더
    void Update(_float fTimeDelta);
    HRESULT Render(class CShader* pShader);

private:
    // 내부 생성 루틴
    HRESULT createGPUStorageBuffers();
    HRESULT createCB();
    void Debug_ReadParticlesFromGPU();
    void Debug_ReadInstancesFromGPU();
    HRESULT DispatchSpawnUpdateCS(_float dt);
    HRESULT DispatchBuildDrawCS();
    void ResetDrawArgsOnCPU();
    void UpdateCenterFromParent();

public:
    virtual void Free() override;

private:
    // ---------------------------------------------------
    // 기본 Desc 및 실행 상태
    // ---------------------------------------------------
    PARTICLESTREAM_DESC m_Desc{};
    _float4x4* m_pParentMatrix = nullptr;
    _float3 m_vFollowOffset = {0.f, 0.f, 0.f};

    _bool m_bActive = false;
    _bool m_bLoop = false;
    _float m_AccumLife = 0.f;

private:
    CVIBuffer_Point* m_VIBuffer = nullptr;
    CTexture* m_pTextureCom = nullptr;
    CShader* m_pShader = nullptr;

    // 파티클 상태 버퍼 (Spawn/Update용)
    ID3D11Buffer* m_pParticleBuffer = nullptr;
    ID3D11ShaderResourceView* m_pParticleSRV = nullptr;
    ID3D11UnorderedAccessView* m_pParticleUAV = nullptr;

    // 인스턴스 버퍼 (렌더링용)
    ID3D11Buffer* m_pInstanceBuffer = nullptr;
    ID3D11ShaderResourceView* m_pInstanceSRV = nullptr;
    ID3D11UnorderedAccessView* m_pInstanceUAV = nullptr;

    // DrawIndirect Args
    ID3D11Buffer* m_pIndirectArgs = nullptr;
    ID3D11UnorderedAccessView* m_pIndirectArgsUAV = nullptr;

    // 상수버퍼
    ID3D11Buffer* m_pCB_ParticleFrame = nullptr;
    // 상수버퍼
    ID3D11Buffer* m_pCB_ViewProj = nullptr;
    ID3D11Buffer* m_pCB_SpriteInfo = nullptr;

    // ComputeShader
    ID3D11ComputeShader* m_pCS_SpawnUpdate = nullptr;
    ID3D11ComputeShader* m_pCS_BuildDrawData = nullptr;

private:
    static constexpr UINT THREADS = 256; // CS thread group
};
END
