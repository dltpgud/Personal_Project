// ===============================================================
// ComputeShader_TrailUpdate_Optimized.hlsl (fixed)
// - Alive만 정렬 / 배리어 최소화 / 256스레드 / 그룹 합산
// - UAV 카운터를 실제 Alive 개수와 동기화
// ===============================================================
float g_fTimeDelta;
matrix g_matView;

struct ForRender
{
    float3 vViewPosition;
};
struct ForUpdate
{
    float3 vWorldPos;
    float3 vWorldUp;
    float2 vLifeTime; // x=init, y=remain
    bool bUp;
};

RWStructuredBuffer<ForRender> RenderBuffer : register(u0);
RWStructuredBuffer<ForUpdate> UpdateBuffer : register(u1);
RWStructuredBuffer<uint> AliveIndexBuf : register(u2); // COUNTER
RWBuffer<uint> DrawArgs : register(u3);

cbuffer TrailConstantBuffer : register(b1)
{
    uint Trail_MaxPoint;
    uint Trail_NumBetween;
    float Trail_LifeTime;
    float Trail_Scale;
}

#define THREADS 256

groupshared float g_Key[THREADS]; // 정렬 키(남은 수명)
groupshared uint g_Index[THREADS]; // 원본 인덱스
groupshared uint gAliveCount; // 이 그룹의 Alive 개수
groupshared uint sAlive[THREADS]; // 합산용
groupshared uint gBase; // 이 그룹이 UAV 카운터에서 예약한 베이스

static const float DEAD_KEY = -3.4e38f; // -FLT_MAX 근사

void BitonicSortAlive(uint tid, uint count)
{
    // count <= THREADS
    for (uint size = 2; size <= count; size <<= 1)
    {
        uint
        half= size >> 1;
        for (uint step = half; step > 0; step >>= 1)
        {
            uint ix = tid ^ step;
            if (ix > tid && ix < count)
            {
                bool ascending = ((tid & size) == 0);
                float a = g_Key[tid];
                float b = g_Key[ix];
                if ((ascending && a > b) || (!ascending && a < b))
                {
                    // swap
                    uint ti = g_Index[tid];
                    g_Index[tid] = g_Index[ix];
                    g_Index[ix] = ti;

                    g_Key[tid] = b;
                    g_Key[ix] = a;
                }
            }
            GroupMemoryBarrierWithGroupSync();
        }
    }
}

[numthreads(THREADS, 1, 1)]
void CS_UPDATE(uint3 DTid : SV_DispatchThreadID,
               uint tid : SV_GroupThreadID,
               uint gid : SV_GroupID)
{
    uint base = gid * THREADS;
    uint idx = base + tid;

    // 한 번만 초기화
    if (DTid.x == 0)
    {
        DrawArgs[0] = 1; // VertexCountPerInstance
        DrawArgs[1] = 0; // InstanceCount
        DrawArgs[2] = 0;
        DrawArgs[3] = 0;
        // DrawArgs[4] 사용 안 함 (카운터는 진짜 UAV 카운터 사용)
        DrawArgs[4] = 0;
    }
    GroupMemoryBarrierWithGroupSync();

    // 로드 & 수명 갱신
    ForRender FR = (ForRender) 0;
    ForUpdate FU = (ForUpdate) 0;

    bool inRange = (idx < Trail_MaxPoint);
    if (inRange)
    {
        FR = RenderBuffer[idx];
        FU = UpdateBuffer[idx];

        float remain = FU.vLifeTime.y;
        if (remain > 0.0f)
        {
            remain -= g_fTimeDelta;
            FU.vLifeTime.y = remain;

            if (remain <= 0.0f)
            {
                // Dead
                FR = (ForRender) 0;
                FU = (ForUpdate) 0;
                g_Key[tid] = DEAD_KEY;
                g_Index[tid] = idx;
            }
            else
            {
                // Alive
                g_Key[tid] = remain;
                g_Index[tid] = idx;
                FR.vViewPosition = mul(float4(FU.vWorldPos + FU.vWorldUp, 1.0f), g_matView).xyz;
            }
        }
        else
        {
            g_Key[tid] = DEAD_KEY;
            g_Index[tid] = idx;
        }
    }
    else
    {
        g_Key[tid] = DEAD_KEY;
        g_Index[tid] = 0;
    }

    if (inRange)
    {
        RenderBuffer[idx] = FR;
        UpdateBuffer[idx] = FU;
    }
    GroupMemoryBarrierWithGroupSync();

    // Alive 개수 계산
    uint aliveFlag = (g_Key[tid] > DEAD_KEY) ? 1u : 0u;
    sAlive[tid] = aliveFlag;
    GroupMemoryBarrierWithGroupSync();

    for (uint stride = THREADS >> 1; stride > 0; stride >>= 1)
    {
        if (tid < stride)
            sAlive[tid] += sAlive[tid + stride];
        GroupMemoryBarrierWithGroupSync();
    }
    if (tid == 0)
        gAliveCount = sAlive[0];
    GroupMemoryBarrierWithGroupSync();

    // 조기 종료
    if (gAliveCount == 0)
        return;

    if (gAliveCount == 1)
    {
        if (aliveFlag == 1)
        {
            // 카운터 +1 하고 append
            uint dst = AliveIndexBuf.IncrementCounter();
            AliveIndexBuf[dst] = g_Index[tid];
        }
        // InstanceCount += 1 (그룹당 한 번)
        if (tid == 0)
            InterlockedAdd(DrawArgs[1], 1);
        return;
    }

    // Alive만 정렬
    BitonicSortAlive(tid, THREADS);
    GroupMemoryBarrierWithGroupSync();

    // ==== 핵심 수정: UAV 카운터와 실제 Alive 개수를 동기화 ====
    // tid==0 이 gAliveCount 만큼 카운터를 증가시켜 "연속 슬롯"을 예약
    if (tid == 0)
    {
        uint base0 = AliveIndexBuf.IncrementCounter(); // 첫 슬롯의 이전 값
        // 나머지 (gAliveCount-1) 만큼 증가
        [loop]
        for (uint k = 1; k < gAliveCount; ++k)
        {
            AliveIndexBuf.IncrementCounter();
        }
        gBase = base0; // 그룹 공유
        // InstanceCount += gAliveCount (그룹당 한 번)
        InterlockedAdd(DrawArgs[1], gAliveCount);
    }
    GroupMemoryBarrierWithGroupSync();

    // 정렬된 Alive만 순서대로 기록
    if (aliveFlag == 1)
    {
        // 내 rank 계산 (앞의 쓰레드 중 Alive만 카운트)
        uint rank = 0;
        [loop]
        for (uint i = 0; i < THREADS; ++i)
        {
            if ((g_Key[i] > DEAD_KEY) && (i < tid))
                rank++;
        }

        uint dst = gBase + rank; // 예약한 연속 슬롯 내 위치
        AliveIndexBuf[dst] = g_Index[tid]; // 정렬 순서대로 기록
    }
}
