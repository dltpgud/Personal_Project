#include "Engine_Shader_Defines.hlsli"

cbuffer CB_DECAL_FRAME : register(b0)
{
    float g_DeltaTime;
    uint g_SpawnCount;
    uint g_MaxDecals;
    float pad0;
};

struct DECAL_SPAWN_REQ
{
    float3 Pos;
    uint Valid;

    float3 Dir;
    float Size;

    float Depth;
    float LifeTime;
    float DeltaScale;
    float PadA;

    int TexIndex;
    int DecalType;
    int bNormal;
    int PadB;
    float4x4 WorldInv;
};

struct GPU_DecalHeader
{
    float3 Pos;
    float Life;
    float3 Dir;
    float DecalTime;
    float3 Size;
    float DeltaScale;
    int TexIndex;
    int Type;
    int Active;
    int bNormal;
};

StructuredBuffer<DECAL_SPAWN_REQ> g_SpawnReq : register(t0);
RWStructuredBuffer<GPU_DecalHeader> g_Decals : register(u0);
RWStructuredBuffer<uint> g_LiveList : register(u1);

[numthreads(256, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    uint idx = id.x;
    if (idx >= g_MaxDecals)
        return;

    GPU_DecalHeader h = g_Decals[idx];

    if (h.Active == 1)
    {
        h.DecalTime += g_DeltaTime;

        if (h.DecalTime >= h.Life)
        {
            h.Active = 0;
        }
    }

    if (h.Active == 0 && g_SpawnCount > 0)
    {
        uint spawnIdx;
        InterlockedAdd(g_LiveList[1000], 1, spawnIdx);

        if (spawnIdx < g_SpawnCount)
        {
            DECAL_SPAWN_REQ req = g_SpawnReq[spawnIdx];
            if (req.Valid == 1)
            {
                h.Pos = req.Pos;
                h.Dir = normalize(req.Dir);
                h.Size = float3(req.Size, req.Size, req.Depth);
                h.bNormal = req.bNormal;
                h.Life = max(req.LifeTime, 0.1f);
                h.DecalTime = 0.0f;
                h.DeltaScale = req.DeltaScale;
                h.TexIndex = req.TexIndex;
                h.Type = req.DecalType;
                h.Active = 1;
            }
        }
    }

    if (h.Active == 1)
    {
        uint outIdx;
        InterlockedAdd(g_LiveList[0], 1, outIdx);
        g_LiveList[outIdx + 1] = idx;
    }

    g_Decals[idx] = h;
}
