// ===============================================================
// ComputeShader_TrailUpdate_Optimized.hlsl
// - Alive만 정렬 / 배리어 최소화 / 256스레드 / 그룹 합산
// - 기존 버퍼/상수 레이아웃은 그대로
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
RWStructuredBuffer<uint> AliveIndexBuffer : register(u2);
RWBuffer<uint> DrawArgs : register(u3);

cbuffer TrailConstantBuffer : register(b1)
{
    uint Trail_MaxPoint;
    uint Trail_NumBetween;
    float Trail_LifeTime;
    float Trail_Scale;
}

// ────────────────────────────────────────────────────────────────
// 스레드 구성 및 LDS
// ────────────────────────────────────────────────────────────────
#define THREADS 256

groupshared float g_Key[THREADS]; // 정렬 키(남은 수명)
groupshared uint g_Index[THREADS]; // 포인트 인덱스
groupshared uint gAliveCount; // 이 그룹의 Alive 개수
groupshared uint sAlive[THREADS];
// Dead를 앞으로/뒤로 빠르게 밀어내기 위한 키
static const float DEAD_KEY = -3.4e38f; // -FLT_MAX 근사

// ────────────────────────────────────────────────────────────────
// 로컬 비토닉 (Alive 범위만)
// ────────────────────────────────────────────────────────────────
void BitonicSortAlive(uint tid, uint count)
{
    // count는 Alive 개수 (≤ THREADS)
    // 오름차순(작을수록 앞). 우리는 이후 뒤집어서 사용.
    for (uint size = 2; size <= count; size <<= 1)
    {
        uint
        half= size >> 1;
        for (uint step = half; step > 0; step >>= 1)
        {
            uint ix = tid ^ step;
            if (ix > tid && ix < count)
            {
                // size 블록 기준 오름·내림 판단
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
void CS_UPDATE(uint3 DTid : SV_DispatchThreadID, uint tid : SV_GroupThreadID, uint gid : SV_GroupID)
{
    // 그룹 베이스 인덱스
    uint base = gid * THREADS;
    uint idx = base + tid;

    // 첫 스레드에서 DrawArgs 초기화 (한 번만)
    if (DTid.x == 0)
    {
        // DrawIndirect args:
        // 0: VertexCountPerInstance, 1: InstanceCount, 2: StartVertex, 3: StartInstance, (4:optional)
        DrawArgs[0] = 1;
        DrawArgs[1] = 0;
        DrawArgs[2] = 0;
        DrawArgs[3] = 0;
        DrawArgs[4] = 0;
    }
    GroupMemoryBarrierWithGroupSync();

    // 로컬 로드
    ForRender FR = (ForRender) 0;
    ForUpdate FU = (ForUpdate) 0;

    bool inRange = (idx < Trail_MaxPoint);
    if (inRange)
    {
        FR = RenderBuffer[idx];
        FU = UpdateBuffer[idx];

        // 남은 수명 업데이트
        float remain = FU.vLifeTime.y;
        if (remain > 0.0f)
        {
            remain -= g_fTimeDelta;
            FU.vLifeTime.y = remain;

            if (remain <= 0.0f)
            {
                // Dead로 전환
                FR = (ForRender) 0;
                FU = (ForUpdate) 0;
                g_Key[tid] = DEAD_KEY;
                g_Index[tid] = idx;
            }
            else
            {
                // Alive: 키는 남은 수명
                g_Key[tid] = remain;
                g_Index[tid] = idx;

                // View pos 갱신
                FR.vViewPosition = mul(float4(FU.vWorldPos + FU.vWorldUp, 1.0f), g_matView).xyz;
            }
        }
        else
        {
            // 이미 죽은 항목
            g_Key[tid] = DEAD_KEY;
            g_Index[tid] = idx;
        }
    }
    else
    {
        // 범위 밖: 정렬에서 제외
        g_Key[tid] = DEAD_KEY;
        g_Index[tid] = 0;
    }

    // 변경사항 반영
    if (inRange)
    {
        RenderBuffer[idx] = FR;
        UpdateBuffer[idx] = FU;
    }
    GroupMemoryBarrierWithGroupSync();

    // 그룹 Alive 개수 계산(Dead는 DEAD_KEY)
    // 1) Alive=1, Dead=0 로 플래그화
    uint aliveFlag = (g_Key[tid] > DEAD_KEY) ? 1u : 0u;

    // 2) Warp‐local prefix도 가능하지만 간단한 그룹 축약으로 합산
    //    (THREADS가 256이라도 부담 적음)
    //    여기서는 바이너리 트리 축약

    sAlive[tid] = aliveFlag;
    GroupMemoryBarrierWithGroupSync();

    // reduce
    for (uint stride = THREADS >> 1; stride > 0; stride >>= 1)
    {
        if (tid < stride)
            sAlive[tid] += sAlive[tid + stride];
        GroupMemoryBarrierWithGroupSync();
    }
    if (tid == 0)
        gAliveCount = sAlive[0];
    GroupMemoryBarrierWithGroupSync();

    // 조기 종료: Alive 0 또는 1 이하면 정렬/쓰기 최소화
    if (gAliveCount <= 1)
    {
        // Alive 1개인 경우만, 카운터/DrawArgs 올려주고 끝낸다.
        if (gAliveCount == 1 && aliveFlag == 1)
        {
            uint baseAlive;
            AliveIndexBuffer.IncrementCounter(); // 자리 확보(우선 카운터만 증가)
            // 간단히 자기 idx를 기록
            // (정렬이 무의미하니 그냥 자기 인덱스 그대로)
            AliveIndexBuffer[idx] = idx;

            InterlockedAdd(DrawArgs[1], 1); // InstanceCount += 1
        }
        return;
    }

    // ────────────────────────────────────────────────────────────
    // Alive만 정렬 (DEAD_KEY는 가장 작아서 앞으로 몰림)
    // 정렬 후에는 [0..gAliveCount-1] 구간이 Alive 오름차순(남은 수명 작은→큰)
    // 우리는 "젊은→오래된" 순으로 쓰고 싶으면 뒤집어 쓰면 됨.
    // ────────────────────────────────────────────────────────────
    BitonicSortAlive(tid, THREADS);
    GroupMemoryBarrierWithGroupSync();

    // 그룹 Alive 개수만큼 전역 AliveIndexBuffer 범위 할당
    // (그룹 하나가 한 번만 InterlockedAdd)
    uint globalBase = 0;
    if (tid == 0)
        globalBase = AliveIndexBuffer.IncrementCounter(); // DirectX SM5 UAV counter는 +1씩이므로 아래서 오프셋 스킴 사용
    GroupMemoryBarrierWithGroupSync();

    // ↑ Counter API는 +1 단위라 범위 할당에 부적합.
    // 간단 대안: DrawArgs[4]를 그룹 Alive 전용 베이스로 활용
    //  - tid==0에서 DrawArgs[4] += gAliveCount; 이전 값을 베이스로 쓸 수 있음
    uint groupBase = 0;
    if (tid == 0)
        InterlockedAdd(DrawArgs[4], gAliveCount, groupBase); // groupBase = 이전 값
    GroupMemoryBarrierWithGroupSync();

    // 정렬된 Alive만 글로벌 AliveIndexBuffer에 연속 기록
    // 뒤집어서(젊은→오래된) 쓰고 싶으면 dstIdx를 (groupBase + (gAliveCount-1 - rank)) 로 쓰면 됨.
    // 여기서는 "남은 수명 작은→큰" 순(오래된→젊은)을 유지하고,
    // VS에서 u를 0..1로 쓰면 꼬리→헤드가 자연스럽게 됨.
    if (aliveFlag == 1)
    {
        // 내 스레드가 Alive 구간에서 차지하는 "순위"를 구해야 하는데,
        // DEAD_KEY가 앞쪽으로 몰렸으니, Alive 순위는
        // "내 g_Key가 DEAD_KEY보다 큰 항목들의 개수"가 됨.
        // 간단히 로컬 카운팅 (THREADS가 256이라 충분히 가벼움)
        uint rank = 0;
        [loop]
        for (uint i = 0; i < THREADS; ++i)
        {
            // 자신보다 "앞에 있는" 스레드들 중 Alive만 카운트
            if ((g_Key[i] > DEAD_KEY) && ((i < tid)))
                rank++;
        }

        uint dst = groupBase + rank;
        // 정렬된 인덱스 쓰기
        AliveIndexBuffer[dst] = g_Index[tid];

        // DrawArgs(InstanceCount)도 그룹 단위로 한 번만 더하면 되지만,
        // 간단히 Alive마다 1씩 추가(원래 코드와 동일)
        InterlockedAdd(DrawArgs[1], 1);
    }
}
