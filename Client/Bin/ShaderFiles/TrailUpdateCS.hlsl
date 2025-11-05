
cbuffer CS_PERFRAME : register(b0)
{
    float dt;
    float fadeSpeed;
    float lifeTime;
    uint maxTrails;

    uint maxPointsPerTrail;
    uint spawnCount;
    uint mode;
    float minStepDist;
    uint maxStitch;
    float2 _pad0;
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

struct SpawnReq
{
    uint valid;
    uint trailIndex;
    float3 headPos;
    float addLife;
    float width;
    float3 _padS0;
    float4 color;
    uint frameIndex;
    uint isSegment;
    float3 tailPos;
    uint generation; 
};

cbuffer CS_PERFRAME_Init : register(b1)
{
    uint maxTrail; 
    uint maxPointsTrail; 
    uint trailInx; 
    float lTime; 
    float3 padding;
};

StructuredBuffer<TrailPoint> g_In : register(t0);
StructuredBuffer<SpawnReq> g_Spawn : register(t1);
RWStructuredBuffer<TrailPoint> g_Out : register(u0);
RWStructuredBuffer<TrailHeader> g_Header : register(u1);
RWStructuredBuffer<TrailPoint> g_Out2 : register(u2);

void PushPoint(inout TrailHeader header, uint base, TrailPoint p, uint maxPts)
{
    uint writeIdx = header.head % maxPts;
    g_Out[base + writeIdx] = p;

    header.head = (header.head + 1) % maxPts;
    if (header.count < maxPts)
        header.count += 1;
    else
        header.first = (header.first + 1) % maxPts;
}

[numthreads(64, 1, 1)]
void CSMain(uint3 gid : SV_DispatchThreadID)
{
    uint trailId = gid.x;
    if (trailId >= maxTrails)
        return;

    uint base = trailId * maxPointsPerTrail;
    TrailHeader header = g_Header[trailId];

    SpawnReq sr = g_Spawn[trailId];
    
    bool anyAlive = false;
    [loop]
    for (uint i = 0; i < maxPointsPerTrail; ++i)
    {
        uint idx = base + i;
        TrailPoint tp = g_In[idx];

        if (tp.life > 0.0f)
        {
            tp.life = max(0.0f, tp.life - dt * fadeSpeed);
            tp.color.a = saturate(tp.life / max(lifeTime, 1e-5));
            if (tp.life > 0.0f)
                anyAlive = true;
        }

        g_Out[idx] = tp; 
    }
    
    if (sr.valid == 1)
    {
        if (sr.width > 0.0f)
            header.width = sr.width;

        header.active = 1;
        header.frameIndex = sr.frameIndex;
        header.generation = sr.generation;
        if (sr.isSegment == 1)
        {
            TrailPoint p0 = { sr.headPos, lifeTime, sr.color };
            TrailPoint p1 = { sr.tailPos, lifeTime, sr.color };
            PushPoint(header, base, p0, maxPointsPerTrail);
            PushPoint(header, base, p1, maxPointsPerTrail);
            anyAlive = true;
        }
        else
        {
            TrailPoint targetP;
            targetP.pos = sr.headPos;
            targetP.life = max(sr.addLife > 0 ? sr.addLife : lifeTime, 0.0f);
            targetP.color = sr.color;
            targetP.color.a = saturate(targetP.life / max(lifeTime, 1e-5));

            float3 prevPos = targetP.pos;
            if (header.count > 0)
            {
                uint prevIdx = (header.head + maxPointsPerTrail - 1) % maxPointsPerTrail;
                prevPos = g_In[base + prevIdx].pos;
            }

            float dist = distance(prevPos, targetP.pos);
            uint stitchCnt = (dist > minStepDist)
                ? min(max((uint) floor(dist / max(minStepDist, 1e-4)), 1u), maxStitch)
                : 1u;

            [loop]
            for (uint k = 1; k <= stitchCnt; ++k)
            {
                float t = (float) k / (float) stitchCnt;
                TrailPoint p = targetP;
                p.pos = lerp(prevPos, targetP.pos, t);
                PushPoint(header, base, p, maxPointsPerTrail);
            }

            anyAlive = true;
        }
    }
    
    if (!anyAlive)
    {
        header.active = 0;
        header.count = 0;
        header.head = 0;
        header.first = 0;
        header.frameIndex = 0;

        [loop]
        for (uint i = 0; i < maxPointsPerTrail; ++i)
        {
            TrailPoint tp;
            tp.pos = float3(0, 0, 0); 
            tp.life = 0.0f; 
            tp.color = float4(0, 0, 0, 0); 
            g_Out[base + i] = tp;
        }
    }

    g_Header[trailId] = header;
}

