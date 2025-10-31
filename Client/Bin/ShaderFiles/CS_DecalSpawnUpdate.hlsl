#include "Engine_Shader_Defines.hlsli"

//============================================================
// Constant Buffer (C++과 동일)
//============================================================
cbuffer CB_DECAL_FRAME : register(b0)
{
    float g_DeltaTime;
    uint g_SpawnCount;
    uint g_MaxDecals;
    float pad0;
};

//============================================================
// 구조체 (C++과 반드시 일치)
//============================================================
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

//============================================================
// 리소스
//============================================================
// t0 : CPU → GPU 스폰 요청
StructuredBuffer<DECAL_SPAWN_REQ> g_SpawnReq : register(t0);

// u0 : 전체 데칼 슬롯
RWStructuredBuffer<GPU_DecalHeader> g_Decals : register(u0);

// u1 : LiveList (0: Count, 이후는 살아있는 슬롯 인덱스들)
// ※ LiveList[1000]을 "스폰 요청 소비 카운터"로 재활용함
RWStructuredBuffer<uint> g_LiveList : register(u1);

//============================================================
// Compute Shader (빈 슬롯 탐색 + 스폰 + 업데이트)
//============================================================
[numthreads(256, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    uint idx = id.x;
    if (idx >= g_MaxDecals)
        return;

    GPU_DecalHeader h = g_Decals[idx];

    //----------------------------------------------
    // 1️⃣ 살아있는 데칼은 시간 경과 처리
    //----------------------------------------------
    if (h.Active == 1)
    {
        h.DecalTime += g_DeltaTime;

        if (h.DecalTime >= h.Life)
        {
            // 수명 다하면 비활성화
            h.Active = 0;
        }
    }

    //----------------------------------------------
    // 2️⃣ 비어있는 슬롯이면, Spawn 요청 하나 소비
    //----------------------------------------------
    if (h.Active == 0 && g_SpawnCount > 0)
    {
        uint spawnIdx;
        InterlockedAdd(g_LiveList[1000], 1, spawnIdx); // 🔧 LiveList[1000]을 spawn counter로 사용

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

    //----------------------------------------------
    // 3️⃣ 아직 살아있는 데칼은 LiveList에 등록
    //----------------------------------------------
    if (h.Active == 1)
    {
        uint outIdx;
        InterlockedAdd(g_LiveList[0], 1, outIdx);
        g_LiveList[outIdx + 1] = idx;
    }

    //----------------------------------------------
    // 4️⃣ 다시 저장
    //----------------------------------------------
    g_Decals[idx] = h;
}
