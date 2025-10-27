// TrailCountCS.hlsl
// - 활성 Trail을 세고(InstanceCount), 활성 trailId들을 ActiveList에 압축 기록
// - DrawInstancedIndirect용 인자 버퍼(g_DrawArgs)도 GPU에서 채움

cbuffer CountCB : register(b0)
{
    uint g_MaxTrails;
    uint _padC0[3];
}

struct TrailHeader
{
    float width;
    float3 _pad0;
    uint active;
    uint head;
    uint count;
    uint first;
};

StructuredBuffer<TrailHeader> g_Header : register(t0);

// DrawInstancedIndirect args (RAW UAV, ByteAddressBuffer)
// layout (16 bytes):
//   0: VertexCountPerInstance (uint)
//   4: InstanceCount          (uint)
//   8: StartVertexLocation    (uint)
//  12: StartInstanceLocation  (uint)
RWByteAddressBuffer g_DrawArgs : register(u0);

// 활성 trail id 들을 0..(active-1)로 압축해 담을 리스트 (크기 = g_MaxTrails)
RWStructuredBuffer<uint> g_ActiveList : register(u1);

[numthreads(64, 1, 1)]
void CSMain(uint3 tid : SV_DispatchThreadID)
{
    uint id = tid.x;
    if (id >= g_MaxTrails)
        return;

    // 쓰레드 0이 DrawArgs 초기화
    if (id == 0)
    {
        g_DrawArgs.Store(0, 1); // VertexCountPerInstance = 1 (VS에서 포인트 1개)
        g_DrawArgs.Store(4, 0); // InstanceCount = 0 (이후 활성 trail마다 ++)
        g_DrawArgs.Store(8, 0); // StartVertexLocation = 0
        g_DrawArgs.Store(12, 0); // StartInstanceLocation = 0
    }

    GroupMemoryBarrierWithGroupSync();

    // 각 trail 조사: active면 InstanceCount += 1 하고, ActiveList에 trailId를 기록
    TrailHeader H = g_Header[id];
    if (H.active == 1 && H.count > 1) // 의미있는 trail만
    {
        uint writeIndex;
        // InstanceCount(오프셋 4)에 원자적 더하기 → 이전 값을 writeIndex로 사용
        g_DrawArgs.InterlockedAdd(4, 1, writeIndex);
        g_ActiveList[writeIndex] = id;
    }
}
