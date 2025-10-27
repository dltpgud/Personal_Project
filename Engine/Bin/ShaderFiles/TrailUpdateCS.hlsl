//------------------------------------------------------------------------------
// TrailUpdateCS.hlsl 
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

// 공통 상수 버퍼
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

// 구조체
struct TrailPoint
{
    float3 pos;
    float life;
    float4 color;
};

struct TrailHeader
{
    float width;
    float3 _pad0; // align to 16 bytes
    uint active;
    uint head;
    uint count;
    uint first;
    uint frameIndex;
    uint generation; // ✅ 세대 번호 (C++과 일치)
    uint2 _pad1; // align to 16 bytes
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
    uint generation; // ✅ 세대 번호 (C++과 일치)
};



cbuffer CS_PERFRAME_Init : register(b1)
{
    uint maxTrail; // 최대 트레일 수
    uint maxPointsTrail; // 최대 포인트 수
    uint trailInx; // 초기화할 트레일 인덱스
    float lTime; // 트레일의 생명 시간
    float3 padding;
};


// 리소스
StructuredBuffer<TrailPoint> g_In : register(t0);
StructuredBuffer<SpawnReq> g_Spawn : register(t1);
RWStructuredBuffer<TrailPoint> g_Out : register(u0);
RWStructuredBuffer<TrailHeader> g_Header : register(u1);
RWStructuredBuffer<TrailPoint> g_Out2 : register(u2);
// 링버퍼에 포인트를 푸시하는 함수
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

// 메인 커널
[numthreads(64, 1, 1)]
void CSMain(uint3 gid : SV_DispatchThreadID)
{
    uint trailId = gid.x;
    if (trailId >= maxTrails)
        return;

    uint base = trailId * maxPointsPerTrail;
    TrailHeader header = g_Header[trailId];

    // === (0) 스폰 요청 확인 ===
    SpawnReq sr = g_Spawn[trailId];


    // === (1) 기존 포인트 fade ===
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

        g_Out[idx] = tp; // ping-pong 복사
    }

    // === (2) 새 스폰 요청 처리 ===
    if (sr.valid == 1)
    {
        if (sr.width > 0.0f)
            header.width = sr.width;

        header.active = 1;
        header.frameIndex = sr.frameIndex;
        header.generation = sr.generation;
        if (sr.isSegment == 1)
        {
            // 세그먼트 포인트 추가
            TrailPoint p0 = { sr.headPos, lifeTime, sr.color };
            TrailPoint p1 = { sr.tailPos, lifeTime, sr.color };
            PushPoint(header, base, p0, maxPointsPerTrail);
            PushPoint(header, base, p1, maxPointsPerTrail);
            anyAlive = true;
        }
        else
        {
            // 스티칭을 위한 트레일 포인트 추가
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

    // === (3) 완전 소멸 시 초기화 ===
    if (!anyAlive)
    {
        // 헤더 초기화
        header.active = 0;
        header.count = 0;
        header.head = 0;
        header.first = 0;
        header.frameIndex = 0;

        // 트레일 포인트 초기화
        [loop]
        for (uint i = 0; i < maxPointsPerTrail; ++i)
        {
            TrailPoint tp;
            tp.pos = float3(0, 0, 0); // 위치 초기화
            tp.life = 0.0f; // 생명 초기화
            tp.color = float4(0, 0, 0, 0); // 색상 초기화 (투명)
            g_Out[base + i] = tp;
        }
    }

    // === (4) 헤더 기록 ===
    g_Header[trailId] = header;
}

