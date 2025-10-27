//------------------------------------------------------------------------------
// TrailInterpCS.hlsl (Final - Continuous + trailId support)
//------------------------------------------------------------------------------
// - DrawInstancedIndirect용 최종 트레일 빌보드 메쉬 생성
// - TrailUpdateCS가 작성한 헤더/포인트를 읽어서
//   camera-facing quad 스트립을 실제 vertex buffer(RWByteAddressBuffer)에 펼침
// - trailId를 각 버텍스에 기록해서 PS에서 g_Header[trailId] 접근 가능
//------------------------------------------------------------------------------

static const uint VERT_STRIDE = 40u; // float3 pos(12) + float2 uv(8) + float4 color(16) + uint trailId(4)
#define SUBDIV 8u

//=============================================================================
// per-dispatch constants
//=============================================================================
cbuffer CS_PERFRAME : register(b0)
{
    uint mode; // 0 = straight, 1 = catmull-rom-ish smoothing
    uint maxTrails;
    uint maxPointsPerTrail;
    float lifeTime;

    float3 g_CamPosWS; // camera world position
    float _pad0;
};

//=============================================================================
// GPU data layouts (must match C++ side / TrailUpdateCS side)
//=============================================================================
struct TrailPoint
{
    float3 pos;
    float life;
    float4 color;
};

struct TrailHeader
{
    float width;
    float3 _pad0; // align 16

    uint active;
    uint head;
    uint count;
    uint first;

    uint frameIndex;
    uint generation; // <-- 세대 (TrailUpdateCS와 동일하게 유지)
    uint2 _pad1; // align 16
};

//=============================================================================
// resources
//=============================================================================
StructuredBuffer<TrailHeader> g_Header : register(t0); // same header buffer CS_Update wrote
StructuredBuffer<TrailPoint> g_Points : register(t1); // ping-ponged trail point buffer (latest)
RWByteAddressBuffer g_Out : register(u0); // final vertex buffer (triangle list)
RWByteAddressBuffer g_DrawArgs : register(u1); // indirect draw args buffer

//=============================================================================
// Catmull-Rom helper
//=============================================================================
float3 CatmullRom(float3 p0, float3 p1, float3 p2, float3 p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;
    return 0.5f *
        ((2.0f * p1)
        + (-p0 + p2) * t
        + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2
        + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

//=============================================================================
// ring buffer index helper
//  head = next write cursor
//  count = how many valid points
//  stride = maxPointsPerTrail
//  j = 0..count-1 logical index
// returns: physical index in the circular buffer
//=============================================================================
uint TrailIndex(uint head, uint count, uint stride, uint j)
{
    uint start = (head + stride - count) % stride;
    return (start + j) % stride;
}

//=============================================================================
// main thread group
// Each thread processes one trail
//=============================================================================
[numthreads(64, 1, 1)]
void CSMain(uint3 gid : SV_DispatchThreadID)
{
    const uint trailId = gid.x;
    if (trailId >= maxTrails)
        return;

    TrailHeader H = g_Header[trailId];

    // 비활성 또는 포인트가 2 미만이면 (연결선 없음) -> 아무 것도 출력 안 함
    if (H.active == 0 || H.count < 2)
    {
        return;
    }

    const uint stride = maxPointsPerTrail;
    const uint base = trailId * stride;

    //--------------------------------------------------------------------------
    // Pass 1: 살아있는 세그먼트 개수 계산
    // 세그먼트 = (Pj, Pj+1) 쌍에서 최소 한 점이라도 life>0
    //--------------------------------------------------------------------------
    uint liveSegs = 0;
    [loop]
    for (uint j = 0; j < H.count - 1; ++j)
    {
        uint i0 = TrailIndex(H.head, H.count, stride, j);
        uint i1 = TrailIndex(H.head, H.count, stride, j + 1);

        TrailPoint P0 = g_Points[base + i0];
        TrailPoint P1 = g_Points[base + i1];

        if (max(P0.life, P1.life) > 0.0f)
        {
            ++liveSegs;
        }
    }

    // 전부 죽었으면 출력 안 함
    if (liveSegs == 0)
    {
        return;
    }

    // 각 세그먼트는 SUBDIV 개의 소구간, 각 소구간은 2삼각형=6버텍스
    const uint localVerts = liveSegs * SUBDIV * 6u;

    //--------------------------------------------------------------------------
    // DrawArgs[0] (vertexCountPerInstance) += localVerts, 동시에 이 trail의 startVertex 얻기
    // DrawArgs 구조(D3D11_DRAW_INSTANCED_INDIRECT_ARGS):
    // 0: VertexCountPerInstance
    // 1: InstanceCount
    // 2: StartVertexLocation
    // 3: StartInstanceLocation
    //
    // 우리는 CPU에서 0으로 초기화해두고 여기서 0만 atomic-add 한다.
    //--------------------------------------------------------------------------
    uint baseVertex = 0;
    g_DrawArgs.InterlockedAdd(0, localVerts, baseVertex);

    // baseVertex는 이 trail이 쓸 첫 번째 버텍스 인덱스 (글로벌 오프셋)
    uint writeCursor = baseVertex;

    //--------------------------------------------------------------------------
    // Pass 2: 실제 버텍스 생성
    // Parallel Transport-like frame to reduce twisting:
    // keep previous 'right' and smoothly update it with projection.
    // trailId를 모든 버텍스에 써서 픽셀 셰이더에서 g_Header[trailId] 참조 가능
    //--------------------------------------------------------------------------

    bool hasPrev = false;
    float3 prevL = 0.0f;
    float3 prevR = 0.0f;
    float3 prevRight = 0.0f;
    float3 prevDir = 0.0f;

    const float3 camPos = g_CamPosWS;

    [loop]
    for (uint j = 0; j < H.count - 1; ++j)
    {
        uint i0 = TrailIndex(H.head, H.count, stride, j);
        uint i1 = TrailIndex(H.head, H.count, stride, j + 1);

        TrailPoint P0 = g_Points[base + i0];
        TrailPoint P1 = g_Points[base + i1];

        if (max(P0.life, P1.life) <= 0.0f)
            continue;

        // A,B는 현재 세그먼트 양 끝의 기준점
        float3 A = P0.pos;
        float3 B = P1.pos;

        // Catmull-Rom 보간 모드일 때 약간 더 부드러운 구간을 뽑아온다
        if (mode == 1 && H.count >= 4)
        {
            uint iM1 = TrailIndex(H.head, H.count, stride, (j == 0 ? 0 : j - 1));
            uint i2 = TrailIndex(H.head, H.count, stride, min(H.count - 1, j + 2));

            TrailPoint Pm1 = g_Points[base + iM1];
            TrailPoint P2 = g_Points[base + i2];

            // 두 샘플 위치(0.25, 0.75)로 약간 부드럽게 재구성
            A = CatmullRom(Pm1.pos, P0.pos, P1.pos, P2.pos, 0.25f);
            B = CatmullRom(Pm1.pos, P0.pos, P1.pos, P2.pos, 0.75f);
        }

        // 세그먼트를 SUBDIV개로 쪼개서 카메라 빌보드된 리본 스트립 생성
        [unroll]
        for (uint s = 0; s < SUBDIV; ++s)
        {
            float tA = (float) s / (float) SUBDIV;
            float tB = (float) (s + 1u) / (float) SUBDIV;

            // 보간된 두 점
            float3 qA = lerp(A, B, tA);
            float3 qB = lerp(A, B, tB);
            float3 qMid = 0.5f * (qA + qB);

            float3 segDir = normalize(qB - qA);
            float3 viewDir = normalize(camPos - qMid);

            // parallel-transport-ish right 벡터 구하기
            float3 right;
            if (!hasPrev)
            {
                // 첫 세그먼트: 그냥 카메라 기준 빌보드
                right = normalize(cross(viewDir, segDir));
            }
            else
            {
                // 이전 right를 현재 segDir에 평행이동/직교화
                float3 rProj = prevRight - segDir * dot(prevRight, segDir);
                float len2 = dot(rProj, rProj);

                right = (len2 > 1e-8f)
                    ? normalize(rProj)
                    : normalize(cross(viewDir, segDir));

                // 뒤집혀 있으면 동일한 면 유지 위해 반전
                if (dot(right, prevRight) < 0.0f)
                    right = -right;
            }

            // 너비와 알파 감쇠
            float tMid = 0.5f * (tA + tB);
            float lifeLerp = lerp(P0.life, P1.life, tMid);
            float life01 = saturate(lifeLerp / max(lifeTime, 1e-5f));
            float halfW = 0.5f * H.width * life01;

            // 스트립 좌/우
            float3 L0, R0;
            if (!hasPrev)
            {
                L0 = qA - right * halfW;
                R0 = qA + right * halfW;
            }
            else
            {
                // 이전 세그먼트의 마지막 단면을 그대로 이어붙여서 틈 제거
                L0 = prevL;
                R0 = prevR;
            }

            float3 L1 = qB - right * halfW;
            float3 R1 = qB + right * halfW;

            // V좌표(길이 방향 UV) - 단면마다 증가
            float vA = ((float) j + tA) / (float) H.count;
            float vB = ((float) j + tB) / (float) H.count;

            // 색상 보간 + 알파는 life 기반
            float4 C0 = lerp(P0.color, P1.color, tA);
            C0.a = life01;
            float4 C1 = lerp(P0.color, P1.color, tB);
            C1.a = life01;

            // 이제 2개의 삼각형(=6버텍스)을 g_Out (ByteAddressBuffer)에 써 넣는다.
            // 각 버텍스 = 40 bytes stride:
            // 0-11  : float3 pos
            // 12-19 : float2 uv
            // 20-35 : float4 color
            // 36-39 : uint trailId

            uint b0 = (writeCursor + 0u) * VERT_STRIDE;
            uint b1 = (writeCursor + 1u) * VERT_STRIDE;
            uint b2 = (writeCursor + 2u) * VERT_STRIDE;
            uint b3 = (writeCursor + 3u) * VERT_STRIDE;
            uint b4 = (writeCursor + 4u) * VERT_STRIDE;
            uint b5 = (writeCursor + 5u) * VERT_STRIDE;

            // tri 0: L0, R0, L1
            g_Out.Store3(b0 + 0u, asuint(L0));
            g_Out.Store2(b0 + 12u, asuint(float2(0.0f, vA)));
            g_Out.Store4(b0 + 20u, asuint(C0));
            g_Out.Store(b0 + 36u, trailId);

            g_Out.Store3(b1 + 0u, asuint(R0));
            g_Out.Store2(b1 + 12u, asuint(float2(1.0f, vA)));
            g_Out.Store4(b1 + 20u, asuint(C0));
            g_Out.Store(b1 + 36u, trailId);

            g_Out.Store3(b2 + 0u, asuint(L1));
            g_Out.Store2(b2 + 12u, asuint(float2(0.0f, vB)));
            g_Out.Store4(b2 + 20u, asuint(C1));
            g_Out.Store(b2 + 36u, trailId);

            // tri 1: L1, R0, R1
            g_Out.Store3(b3 + 0u, asuint(L1));
            g_Out.Store2(b3 + 12u, asuint(float2(0.0f, vB)));
            g_Out.Store4(b3 + 20u, asuint(C1));
            g_Out.Store(b3 + 36u, trailId);

            g_Out.Store3(b4 + 0u, asuint(R0));
            g_Out.Store2(b4 + 12u, asuint(float2(1.0f, vA)));
            g_Out.Store4(b4 + 20u, asuint(C0));
            g_Out.Store(b4 + 36u, trailId);

            g_Out.Store3(b5 + 0u, asuint(R1));
            g_Out.Store2(b5 + 12u, asuint(float2(1.0f, vB)));
            g_Out.Store4(b5 + 20u, asuint(C1));
            g_Out.Store(b5 + 36u, trailId);

            writeCursor += 6u;

            // parallel transport 상태 갱신
            prevL = L1;
            prevR = R1;
            prevRight = right;
            prevDir = segDir;
            hasPrev = true;
        }
    }
}
