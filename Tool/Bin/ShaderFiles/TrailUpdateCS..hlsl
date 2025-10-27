//------------------------------------------------------------------------------
// TrailUpdateCS.hlsl (SM5.0)
// - 슬롯, 구조체, 상수버퍼 레이아웃은 CEffect_TrailStream::Update() 와 일치
// - SRV:  t0=g_In, t1=Header? (읽지 않음), t2=g_SpawnReq
// - UAV:  u0=g_Out, u1=g_Header
// - CB:   b0=CS_PERFRAME (dt/fade/lifetime/maxTrails/maxPointsPerTrail)
//------------------------------------------------------------------------------

cbuffer CS_PERFRAME : register(b0)
{
    float dt;
    float fadeSpeed;
    float lifeTime;
    uint maxTrails;

    uint maxPointsPerTrail;
    float3 _pad0;
}

struct TrailPoint
{
    float3 pos;
    float life;
    float4 color;
};

struct TrailHeader
{
    float width; // per trail width
    float3 _pad0;
    uint active; // 1:alive
    uint3 _pad1;
};

struct SpawnReq
{
    uint trailID;
    float3 headPos;
    float addLife; // 새 포인트 life
    float width; // 새 폭(선택)
    float4 color; // 새 포인트 색상 (a는 CS에서 life로 보정)
};

StructuredBuffer<TrailPoint> g_In : register(t0);
RWStructuredBuffer<TrailPoint> g_Out : register(u0);
RWStructuredBuffer<TrailHeader> g_Header : register(u1);
StructuredBuffer<SpawnReq> g_SpawnReq : register(t2);

[numthreads(64, 1, 1)]
void CSMain(uint3 gid : SV_DispatchThreadID)
{
    const uint trailId = gid.x;
    if (trailId >= maxTrails)
        return;

    const uint base = trailId * maxPointsPerTrail;

    // 1) life 감소 + alpha 보정
    [loop]
    for (uint i = 0; i < maxPointsPerTrail; ++i)
    {
        const uint idx = base + i;
        TrailPoint tp = g_In[idx];

        if (tp.life > 0.0f)
        {
            tp.life = max(0.0f, tp.life - dt * fadeSpeed);
            const float a = saturate(lifeTime > 0.0f ? (tp.life / lifeTime) : 0.0f);
            tp.color.a = a;
        }
        g_Out[idx] = tp;
    }

    // 2) 스폰 요청 적용 (간단히 trailId 매칭되는 첫 요청만 처리)
    [loop]
    for (uint si = 0; si < g_SpawnReq.Length; ++si)
    {
        SpawnReq sr = g_SpawnReq[si];
        if (sr.trailID != trailId)
            continue;

        // 꼬리부터 한 칸씩 밀기
        for (uint i = maxPointsPerTrail - 1; i > 0; --i)
        {
            const uint dst = base + i;
            const uint src = base + (i - 1);
            g_Out[dst] = g_Out[src];
        }

        // 새 헤드 삽입
        TrailPoint head;
        head.pos = sr.headPos;
        head.life = clamp(sr.addLife, 0.0f, lifeTime);
        head.color = sr.color;
        head.color.a = saturate(lifeTime > 0.0f ? (head.life / lifeTime) : 0.0f);

        g_Out[base + 0] = head;

        // 헤더 갱신
        TrailHeader th = g_Header[trailId];
        th.width = (sr.width > 0.0f) ? sr.width : th.width;
        th.active = 1;
        g_Header[trailId] = th;

        break; // 한 프레임 1요청만 처리(간단 모드)
    }
}
