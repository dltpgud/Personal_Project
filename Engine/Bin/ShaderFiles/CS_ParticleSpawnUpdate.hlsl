//===================================================================
// CS_ParticleSpawnUpdate.hlsl
// - 파티클 생성 및 업데이트 (GPU)
//===================================================================
#include "Engine_Shader_Defines.hlsli"

//===================================================================
// 구조체 정의
//===================================================================
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

//===================================================================
// 상수 버퍼
//===================================================================
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

//===================================================================
// UAV 버퍼 (파티클 풀)
//===================================================================
RWStructuredBuffer<GPU_Particle> g_Particles : register(u0);

//===================================================================
// GPU 의사난수 함수
//===================================================================
float Hash11(uint n)
{
    n = (n << 13U) ^ n;
    uint nn = (n * (n * n * 15731U + 789221U) + 1376312589U);
    return frac((float) (nn & 0x7fffffffU) / 2147483648.0f);
}

float rand(uint seed)
{
    return Hash11(seed);
}

//===================================================================
// 설정 상수
//===================================================================
static const float FRAME_PER_SEC = 24.0f;

//===================================================================
// Compute Shader Entry
//===================================================================
[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint id = DTid.x;
    if (id >= g_iMaxParticle)
        return;

    GPU_Particle p = g_Particles[id];

    //-----------------------------------------------------------------
    // 1️⃣ 파티클 재생성 (죽었거나 초기화되지 않은 경우)
    //-----------------------------------------------------------------
    if (p.fLife <= 0.0f)
    {
        // 루프 모드면 즉시 리스폰
        if (g_bLoop == 1.0f)
        {
            // 랜덤 수명
            p.fMaxLife = g_vLife.x + (g_vLife.y - g_vLife.x) * rand(id * 31);
            p.fLife = p.fMaxLife;

            // 위치: 중심 + 랜덤 범위
            p.vPos = g_vCenter + float3(
                (rand(id * 11) - 0.5f) * g_vRange.x,
                (rand(id * 37) - 0.5f) * g_vRange.y,
                (rand(id * 73) - 0.5f) * g_vRange.z
            );

            // 속도
            float speed = g_vSpeed.x + (g_vSpeed.y - g_vSpeed.x) * rand(id * 97);
            float3 dir = normalize(float3(
                rand(id * 101) - 0.5f,
                rand(id * 131),
                rand(id * 151) - 0.5f
            ));
            p.vVel = dir * speed;

            // 크기 / 색상
            p.vSize = g_vSize;
            p.vColor = float4(1, 1, 1, 1);
            p.FrameIndex = 0;
        }
        else
        {
            // 비루프 모드면 그냥 사망 처리 유지
            p.fLife = 0.0f;
        }

        g_Particles[id] = p;
        return;
    }

    //-----------------------------------------------------------------
    // 2️⃣ 살아있는 파티클 업데이트
    //-----------------------------------------------------------------
    p.fLife -= g_DeltaTime;

    if (p.fLife > 0.0f)
    {
        // 위치 업데이트
        p.vPos += p.vVel * g_DeltaTime;

        // 중력 등 물리 효과 (옵션)
        // p.vVel.y -= 9.8f * 0.3f * g_DeltaTime;

        // 프레임 진행
        float frameAdvance = FRAME_PER_SEC * g_DeltaTime;
        p.FrameIndex += (uint) frameAdvance;

        // FrameIndex overflow 방지
        if (p.FrameIndex > 1024)
            p.FrameIndex = 0;
    }
    else
    {
        // 생명 다함
        if (g_bLoop == 1.0f)
        {
            // 루프면 즉시 리스폰
            p.fLife = 0.0f; // 다음 프레임에서 다시 스폰됨
        }
        else
        {
            p.fLife = 0.0f; // 완전 종료
        }
    }

    //-----------------------------------------------------------------
    // 3️⃣ 저장
    //-----------------------------------------------------------------
    g_Particles[id] = p;
}
