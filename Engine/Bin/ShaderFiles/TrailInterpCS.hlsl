
static const uint VERT_STRIDE = 40u; // float3 pos(12) + float2 uv(8) + float4 color(16) + uint trailId(4)
#define SUBDIV 8u

cbuffer CS_PERFRAME : register(b0)
{
    uint mode; 
    uint maxTrails;
    uint maxPointsPerTrail;
    float lifeTime;

    float3 g_CamPosWS; 
    float _pad0;
};

struct TrailPoint
{
    float3 pos;
    float life;
    float4 color;
};

struct TrailHeader
{
    float width;
    float3 _pad0; 
    uint active;
    uint head;
    uint count;
    uint first;
    uint frameIndex;
    uint generation; 
    uint2 _pad1; 
};

StructuredBuffer<TrailHeader> g_Header : register(t0); 
StructuredBuffer<TrailPoint> g_Points : register(t1); 
RWByteAddressBuffer g_Out : register(u0); 
RWByteAddressBuffer g_DrawArgs : register(u1); 

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

uint TrailIndex(uint head, uint count, uint stride, uint j)
{
    uint start = (head + stride - count) % stride;
    return (start + j) % stride;
}

[numthreads(64, 1, 1)]
void CSMain(uint3 gid : SV_DispatchThreadID)
{
    const uint trailId = gid.x;
    if (trailId >= maxTrails)
        return;

    TrailHeader H = g_Header[trailId];

    if (H.active == 0 || H.count < 2)
    {
        return;
    }

    const uint stride = maxPointsPerTrail;
    const uint base = trailId * stride;

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

    if (liveSegs == 0)
    {
        return;
    }
    
    const uint localVerts = liveSegs * SUBDIV * 6u;

    uint baseVertex = 0;
    g_DrawArgs.InterlockedAdd(0, localVerts, baseVertex);

    uint writeCursor = baseVertex;

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

        float3 A = P0.pos;
        float3 B = P1.pos;

        if (mode == 1 && H.count >= 4)
        {
            uint iM1 = TrailIndex(H.head, H.count, stride, (j == 0 ? 0 : j - 1));
            uint i2 = TrailIndex(H.head, H.count, stride, min(H.count - 1, j + 2));

            TrailPoint Pm1 = g_Points[base + iM1];
            TrailPoint P2 = g_Points[base + i2];

            A = CatmullRom(Pm1.pos, P0.pos, P1.pos, P2.pos, 0.25f);
            B = CatmullRom(Pm1.pos, P0.pos, P1.pos, P2.pos, 0.75f);
        }

        [unroll]
        for (uint s = 0; s < SUBDIV; ++s)
        {
            float tA = (float) s / (float) SUBDIV;
            float tB = (float) (s + 1u) / (float) SUBDIV;
            
            float3 qA = lerp(A, B, tA);
            float3 qB = lerp(A, B, tB);
            float3 qMid = 0.5f * (qA + qB);

            float3 segDir = normalize(qB - qA);
            float3 viewDir = normalize(camPos - qMid);
            
            float3 right;
            if (!hasPrev)
            {
                right = normalize(cross(viewDir, segDir));
            }
            else
            {
                float3 rProj = prevRight - segDir * dot(prevRight, segDir);
                float len2 = dot(rProj, rProj);

                right = (len2 > 1e-8f)
                    ? normalize(rProj)
                    : normalize(cross(viewDir, segDir));

                if (dot(right, prevRight) < 0.0f)
                    right = -right;
            }
            
            float tMid = 0.5f * (tA + tB);
            float lifeLerp = lerp(P0.life, P1.life, tMid);
            float life01 = saturate(lifeLerp / max(lifeTime, 1e-5f));
            float halfW = 0.5f * H.width * life01;
            
            float3 L0, R0;
            if (!hasPrev)
            {
                L0 = qA - right * halfW;
                R0 = qA + right * halfW;
            }
            else
            {
                L0 = prevL;
                R0 = prevR;
            }

            float3 L1 = qB - right * halfW;
            float3 R1 = qB + right * halfW;

            float vA = ((float) j + tA) / (float) H.count;
            float vB = ((float) j + tB) / (float) H.count;
            
            float4 C0 = lerp(P0.color, P1.color, tA);
            C0.a = life01;
            float4 C1 = lerp(P0.color, P1.color, tB);
            C1.a = life01;
            
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

            prevL = L1;
            prevR = R1;
            prevRight = right;
            prevDir = segDir;
            hasPrev = true;
        }
    }
}
