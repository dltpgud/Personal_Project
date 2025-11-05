#include "Engine_Shader_Defines.hlsli"

//=====================================================================
// 파티클 상태 버퍼 구조 (SpawnUpdate CS에서 갱신된 결과)
//=====================================================================
struct GPU_Particle
{
    float3 vPos;
    float fLife;

    float3 vVel;
    float fMaxLife;

    float2 vSize;
    float2 _pad0;

    float4 vColor;
    uint FrameIndex;
    float3 _pad1;
};

//=====================================================================
// GPU로 넘길 인스턴스 버퍼 구조 (VS에서 읽음)
//=====================================================================
struct GPU_ParticleInstance
{
    float3 vPosition;
    float fSize;
    float4 vColor;
    uint FrameIndex;
    float3 _Pad0;
};

//=====================================================================
// 상수 버퍼 (프레임별 데이터)
//=====================================================================
cbuffer CB_PARTICLE_FRAME : register(b0)
{
    float3 g_vCenter;
    float g_DeltaTime;

    float3 g_vRange;
    float g_bLoop;

    float2 g_vSize;
    float2 g_vSpeed;

    float2 g_vLife;
    uint g_iMaxParticle;
    float pad;
};

//=====================================================================
// 입력 / 출력 리소스
//=====================================================================
StructuredBuffer<GPU_Particle> g_Particles : register(t0);
RWStructuredBuffer<GPU_ParticleInstance> g_InstanceOut : register(u0);
RWByteAddressBuffer g_IndirectArgs : register(u1); // DrawIndirectArgs

//=====================================================================
// Helper: Instance Count 증가
//  g_IndirectArgs 구조 (DrawInstancedIndirect용):
//    0 : VertexCountPerInstance
//    4 : InstanceCount
//    8 : StartVertexLocation
//   12 : StartInstanceLocation
//=====================================================================
uint ReserveInstanceIndex()
{
    uint prev;
    g_IndirectArgs.InterlockedAdd(4, 1, prev);
    return prev;
}

//=====================================================================
// Compute Shader Entry
//=====================================================================
[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint id = DTid.x;
    if (id >= g_iMaxParticle)
        return;

    //-----------------------------------------------------------------
    // 0번 스레드가 DrawIndirectArgs 초기화
    //-----------------------------------------------------------------
    if (id == 0)
    {
        // POINTLIST 렌더링에서는 "1"
        g_IndirectArgs.Store(0, 1); // VertexCountPerInstance = 1

        // InstanceCount = 0 (빌드 중 증가 예정)
        g_IndirectArgs.Store(4, 0);

        g_IndirectArgs.Store(8, 0); // StartVertexLocation
        g_IndirectArgs.Store(12, 0); // StartInstanceLocation
    }

    GroupMemoryBarrierWithGroupSync();

    //-----------------------------------------------------------------
    // 파티클 하나씩 검사 후 살아있으면 InstanceOut에 기록
    //-----------------------------------------------------------------
    GPU_Particle p = g_Particles[id];

    if (p.fLife <= 0.0f)
        return; // 죽은 파티클은 스킵

    uint instIndex = ReserveInstanceIndex();

    GPU_ParticleInstance outInst;
    outInst.vPosition = p.vPos;
    outInst.fSize = (p.vSize.x + p.vSize.y) * 0.5f;
    outInst.vColor = p.vColor;
    outInst.FrameIndex = p.FrameIndex;
    outInst._Pad0 = float3(0, 0, 0);

    g_InstanceOut[instIndex] = outInst;
}
