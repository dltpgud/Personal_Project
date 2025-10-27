// ===== 데이터 레이아웃 (기존과 동일) =====
struct ForRender
{
    float3 vViewPosition;
};
struct ForUpdate
{
    float3 vWorldPos; // 포인트 위치 (월드)
    float3 vWorldUp; // 리본 폭 방향(월드) : Up/Dn 구분 시 크기/부호로 구분
    float2 vLifeTime; // (x=초기수명, y=남은수명)
    bool bUp; // Up(true)/Down(false) 페어
};

// UAV / SRV
RWStructuredBuffer<ForRender> RenderBuffer : register(u0);
RWStructuredBuffer<ForUpdate> UpdateBuffer : register(u1);
RWStructuredBuffer<uint> AliveIndexBuffer : register(u2);

// 트레일 상수 (b1)
cbuffer TrailConstantBuffer : register(b1)
{
    uint Trail_MaxPoint; // 전체 포인트 버퍼 용량(Up/Dn/보간 포함)
    uint Trail_NumBetween; // 보간 포인트 개수 (각 Up/Dn 사이)
    float2 Trail_Scale; // Up/Dn 폭 (x=Up, y=Down)
    float Trail_LifeTime; // 수명
    float3 TrailConstant_Pad;
}

// Emit 정보 (b2)
cbuffer EmitInfoBuffer : register(b2)
{
    row_major float4x4 matWorld; // 이번 프레임 트레일 기준 행렬(발사체/무기 헤드 등)
}

// ===== Up 축 선택 =====
// 필요 시 빌드 매크로로 하나만 켜세요 (_USE_X_UP/_USE_Y_UP/_USE_Z_UP)
#ifndef _USE_X_UP
#ifndef _USE_Y_UP
#ifndef _USE_Z_UP
#define _USE_Z_UP 1
#endif
#endif
#endif

// ===== Catmull-Rom 보간 함수 =====
float3 CatmullRom(float3 p0, float3 p1, float3 p2, float3 p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;
    return 0.5f * (
        (2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
    );
}

// ===== 안전 보조: 인덱스 Clamp =====
uint ClampIndex(uint idx, uint maxCount)
{
    return (idx >= maxCount) ? (maxCount - 1) : idx;
}

// ===== 메인: 1스레드 디스패치 =====
[numthreads(1, 1, 1)]
void CS_EMIT(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x != 0)
        return;

    // --- Up 벡터 계산 (월드 축 선택) ---
    float3 vWorldUpAxis;
#if defined(_USE_X_UP)
        vWorldUpAxis = normalize(matWorld._11_12_13);
#elif defined(_USE_Y_UP)
        vWorldUpAxis = normalize(matWorld._21_22_23);
#else
    vWorldUpAxis = normalize(matWorld._31_32_33);
#endif

    float3 vEmitPos = matWorld._41_42_43;

    // ===== 1) 새 기준점(Up/Dn) 두 개를 등록 =====
    // Alive 카운터를 증가시키면서 Index를 확보
    uint idxUp = AliveIndexBuffer.IncrementCounter();
    uint idxDn = AliveIndexBuffer.IncrementCounter();

    // 용량 초과 방지(Overrun 시 무시)
    if (idxDn >= Trail_MaxPoint)
    {
        // (선택) 여기서 바로 Counter를 줄일 수는 없으므로, 그냥 리턴
        // 실제 운영에서는 Reset 또는 Update 단계에서 자연 정리됨
        return;
    }

    // 인덱스 기록
    AliveIndexBuffer[idxUp] = idxUp;
    AliveIndexBuffer[idxDn] = idxDn;

    // Up/Dn 포인트 채우기
    UpdateBuffer[idxUp].vWorldPos = vEmitPos;
    UpdateBuffer[idxUp].vWorldUp = vWorldUpAxis * Trail_Scale.x;
    UpdateBuffer[idxUp].vLifeTime = float2(Trail_LifeTime, Trail_LifeTime);
    UpdateBuffer[idxUp].bUp = true;

    UpdateBuffer[idxDn].vWorldPos = vEmitPos;
    UpdateBuffer[idxDn].vWorldUp = vWorldUpAxis * Trail_Scale.y;
    UpdateBuffer[idxDn].vLifeTime = float2(Trail_LifeTime, Trail_LifeTime);
    UpdateBuffer[idxDn].bUp = false;

    // ===== 2) 이전 세그먼트와 곡선 보간 포인트 삽입 =====
    // 첫 Emit(=0,1) 이면 보간 생략
    if (idxUp >= 2 && Trail_NumBetween > 0)
    {
        // 직전 페어(Up=idxUp-2, Dn=idxDn-2) 라고 가정
        uint lastUp = idxUp - 2;
        uint lastDn = idxDn - 2;

        // --- Up 라인용 Catmull-Rom 컨트롤 포인트 ---
        uint upP0 = (lastUp >= 2) ? (lastUp - 2) : lastUp;
        uint upP1 = lastUp;
        uint upP2 = idxUp;
        uint upP3 = ClampIndex(idxUp + 2, Trail_MaxPoint);

        float3 up0 = UpdateBuffer[upP0].vWorldPos;
        float3 up1 = UpdateBuffer[upP1].vWorldPos;
        float3 up2 = UpdateBuffer[upP2].vWorldPos;
        float3 up3 = UpdateBuffer[upP3].vWorldPos;

        float upLen0 = length(UpdateBuffer[lastUp].vWorldUp);
        float upLen1 = length(UpdateBuffer[idxUp].vWorldUp);

        // --- Down 라인용 컨트롤 포인트 ---
        uint dnP0 = (lastDn >= 2) ? (lastDn - 2) : lastDn;
        uint dnP1 = lastDn;
        uint dnP2 = idxDn;
        uint dnP3 = ClampIndex(idxDn + 2, Trail_MaxPoint);

        float3 dn0 = UpdateBuffer[dnP0].vWorldPos;
        float3 dn1 = UpdateBuffer[dnP1].vWorldPos;
        float3 dn2 = UpdateBuffer[dnP2].vWorldPos;
        float3 dn3 = UpdateBuffer[dnP3].vWorldPos;

        float dnLen0 = length(UpdateBuffer[lastDn].vWorldUp);
        float dnLen1 = length(UpdateBuffer[idxDn].vWorldUp);

        // --- 보간 포인트 생성 ---
        [loop]
        for (uint i = 0; i < Trail_NumBetween; ++i)
        {
            float t = (float) (i + 1) / (float) (Trail_NumBetween + 1);

            // 용량 체크 (Up/Dn 각각 한 칸씩 필요)
            uint subUp = AliveIndexBuffer.IncrementCounter();
            if (subUp >= Trail_MaxPoint)
                break;
            uint subDn = AliveIndexBuffer.IncrementCounter();
            if (subDn >= Trail_MaxPoint)
                break;

            AliveIndexBuffer[subUp] = subUp;
            AliveIndexBuffer[subDn] = subDn;

            // 위치 = Catmull-Rom 곡선 상의 점
            float3 posUp = CatmullRom(up0, up1, up2, up3, t);
            float3 posDn = CatmullRom(dn0, dn1, dn2, dn3, t);

            // 폭 보간 (길이만 부드럽게 보간, 방향은 새 Up축 기준 정규화)
            float lenUp = lerp(upLen0, upLen1, t);
            float lenDn = lerp(dnLen0, dnLen1, t);

            UpdateBuffer[subUp].vWorldPos = posUp;
            UpdateBuffer[subUp].vWorldUp = normalize(lerp(UpdateBuffer[lastUp].vWorldUp, UpdateBuffer[idxUp].vWorldUp, t));
            UpdateBuffer[subUp].vWorldUp = normalize(UpdateBuffer[subUp].vWorldUp) * lenUp;
            UpdateBuffer[subUp].vLifeTime = float2(Trail_LifeTime, lerp(UpdateBuffer[lastUp].vLifeTime.y, Trail_LifeTime, t));
            UpdateBuffer[subUp].bUp = true;

            UpdateBuffer[subDn].vWorldPos = posDn;
            UpdateBuffer[subDn].vWorldUp = normalize(lerp(UpdateBuffer[lastDn].vWorldUp, UpdateBuffer[idxDn].vWorldUp, t));
            UpdateBuffer[subDn].vWorldUp = normalize(UpdateBuffer[subDn].vWorldUp) * lenDn;
            UpdateBuffer[subDn].vLifeTime = float2(Trail_LifeTime, lerp(UpdateBuffer[lastDn].vLifeTime.y, Trail_LifeTime, t));
            UpdateBuffer[subDn].bUp = false;
        }
    }
}
