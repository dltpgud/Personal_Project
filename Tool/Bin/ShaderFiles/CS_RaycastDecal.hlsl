// RaycastDecal_CS.hlsl
cbuffer RayCB : register(b0)
{
    float3 g_RayOrigin;  float _pad0; // 16B
    float3 g_RayDir;     float _pad1; // 16B (정규화!)
    uint g_NumTris;     float3 _pad2; // 16B
    float3 g_DecalSize;  float _pad3; // 16B (xy=가로/세로, z=두께)
}

StructuredBuffer<float3> g_Positions : register(t0); // 월드 좌표 정점
StructuredBuffer<uint3> g_Indices : register(t1); // 삼각형 인덱스(3개씩)

RWStructuredBuffer<float4x4> g_DecalWorld : register(u0);
RWStructuredBuffer<float4x4> g_DecalWorldInv : register(u1);
RWStructuredBuffer<uint> g_MinTBits : register(u2);

static const float EPS = 1e-7f;

bool RayTri(float3 o, float3 d, float3 v0, float3 v1, float3 v2, out float t, out float3 n)
{
    float3 e1 = v1 - v0, e2 = v2 - v0;
    float3 p = cross(d, e2);
    float det = dot(e1, p);
    if (abs(det) < EPS)
    {
        t = 0;
        n = 0;
        return false;
    }
    float inv = rcp(det);

    float3 tv = o - v0;
    float u = dot(tv, p) * inv;
    if (u < 0 || u > 1)
    {
        t = 0;
        n = 0;
        return false;
    }
    float3 q = cross(tv, e1);
    float v = dot(d, q) * inv;
    if (v < 0 || u + v > 1)
    {
        t = 0;
        n = 0;
        return false;
    }

    t = dot(e2, q) * inv;
    if (t <= 0)
    {
        n = 0;
        return false;
    }

    n = normalize(cross(e1, e2));
    return true;
}

[numthreads(64, 1, 1)]
void CS_Raycast(uint3 id : SV_DispatchThreadID)
{
    uint tri = id.x;
    if (tri >= g_NumTris)
        return;

    uint3 idx = g_Indices[tri];
    float3 v0 = g_Positions[idx.x];
    float3 v1 = g_Positions[idx.y];
    float3 v2 = g_Positions[idx.z];

    float t;
    float3 N;
    if (!RayTri(g_RayOrigin, g_RayDir, v0, v1, v2, t, N))
        return;

    uint tBits = asuint(t);
    InterlockedMin(g_MinTBits[0], tBits);

    if (g_MinTBits[0] == tBits)
    {
        float3 P = g_RayOrigin + g_RayDir * t;
        if (dot(N, -g_RayDir) < 0)
            N = -N;

        float3 T = normalize(g_RayDir - N * dot(g_RayDir, N));
        float3 B = normalize(cross(N, T));
        float3 S = g_DecalSize;

        // World
        float4x4 M =
        {
            float4(T * S.x, 0),
            float4(B * S.y, 0),
            float4(N * S.z, 0),
            float4(P, 1)
        };

        // WorldInv (직교행렬 성질 이용)
        float3x3 R = float3x3(T, B, N);
        R[0] /= S.x;
        R[1] /= S.y;
        R[2] /= S.z;
        float3x3 Rinv = transpose(R);
        float3 Tinvinv = -mul(Rinv, P);

        float4x4 Minv =
        {
            float4(Rinv[0], 0),
            float4(Rinv[1], 0),
            float4(Rinv[2], 0),
            float4(Tinvinv, 1)
        };

        g_DecalWorld[0] = M;
        g_DecalWorldInv[0] = Minv;
    }
}
