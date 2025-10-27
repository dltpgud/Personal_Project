// ===============================================================
// ComputeShader_TrailReset.hlsl
// - Render/Update 버퍼 내용 초기화
// - AliveIndexBuffer 내용은 0으로 채움(선택) + *카운터는 CPU에서 initialCounts로 0 설정 필요*
// - DrawArgs 리셋
// ===============================================================

struct ForRender
{
    float3 vViewPosition;
};

struct ForUpdate
{
    float3 vWorldPos;
    float3 vWorldUp;
    float2 vLifeTime; // x = init, y = remain
    bool bUp;
};

// UAVs
RWStructuredBuffer<ForRender> RenderBuffer : register(u0);
RWStructuredBuffer<ForUpdate> UpdateBuffer : register(u1);
RWStructuredBuffer<uint> AliveIndexBuf : register(u2); // COUNTER付き
RWBuffer<uint> DrawArgs : register(u3);

// Reset용 상수 (최대 몇 개를 지울지)
cbuffer ResetConstantBuffer : register(b0)
{
    uint Reset_iMaxPoint; // 보통 Trail_MaxPoint와 동일하게 세팅
    uint3 _padReset;
}

#define THREADS 256

[numthreads(THREADS, 1, 1)]
void CS_RESET(uint3 DTid : SV_DispatchThreadID, uint tid : SV_GroupThreadID)
{
    // 첫 스레드에서 DrawArgs 리셋
    if (DTid.x == 0)
    {
        // DrawIndirect args: 0..4 사용
        DrawArgs[0] = 1; // VertexCountPerInstance(고정 1)
        DrawArgs[1] = 0; // InstanceCount
        DrawArgs[2] = 0; // StartVertexLocation
        DrawArgs[3] = 0; // StartInstanceLocation
        DrawArgs[4] = 0; // 임시/보조 값 사용 시 초기화
    }
    GroupMemoryBarrierWithGroupSync();

    uint idx = DTid.x;

    if (idx < Reset_iMaxPoint)
    {
        // 포인트 데이터를 깔끔히 0으로
        RenderBuffer[idx] = (ForRender) 0;
        UpdateBuffer[idx] = (ForUpdate) 0;

        // (선택) AliveIndex 내용도 정리
        // 실제 그리기엔 영향 없지만 디버그/읽기 편의용
        AliveIndexBuf[idx] = 0;
    }

    // NOTE:
    // AliveIndexBuf의 *카운터*는 여기서 0으로 만들 수 없음.
    // C++에서 이 패스를 디스패치하기 직전에
    //   UINT initCounts[4] = { UINT(-1), UINT(-1), 0, UINT(-1) };
    //   context->CSSetUnorderedAccessViews(0, 4, uavs, initCounts);
    // 처럼 u2의 initialCounts를 0으로 설정해주면 카운터가 초기화된다.
}
