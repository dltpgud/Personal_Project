#include "Engine_Shader_Defines.hlsli"

//==================================================================================
// Constant Buffers
//==================================================================================
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float2 g_WinDowSize;
float g_fCamFar;
float _pad0;

matrix g_ViewMatrixInv, g_ProjMatrixInv;
vector g_vCamPosition;

// G-Buffer inputs
Texture2D g_DepthTexture : register(t10);
Texture2D g_NormalTexture : register(t11);

// Decal atlas
Texture2DArray g_DecalArray : register(t12);

//==================================================================================
// GPU-generated instance data (from compute shader)
//==================================================================================
struct DecalInstanceData
{
    row_major float4x4 WorldInv; // 64바이트
    float3 DecalPos; // 12
    float _padA; // 4  -> align 16

    float3 DecalDir; // 12
    float _padB; // 4

    float3 HalfSize; // 12
    float LifeTime; // 4

    float DecalTime; // 4
    int TexIndex; // 4
    int DecalType; // 4
    int bNormal; // 4
}; // == 128 bytes

// CS가 만든 인스턴스 데이터 (VS 전용 StructuredBuffer)
StructuredBuffer<DecalInstanceData> g_InstanceDataVS : register(t9);

//==================================================================================
// VS / PS I/O
//==================================================================================
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 Texcoord : TEXCOORD0;
    uint InstID : SV_InstanceID;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD0;

    row_major float4x4 WorldInv : TEXCOORD1;
    float3 DecalPos : TEXCOORD5;
    float3 DecalDir : TEXCOORD6;
    float3 HalfSize : TEXCOORD7;
    float LifeTime : TEXCOORD8;
    float DecalTime : TEXCOORD9;
    int TexIndex : TEXCOORD10;
    int bNormal : TEXCOORD11;
    int DecalType : TEXCOORD12;
};

VS_OUTPUT VS_Decal(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT) 0;
    DecalInstanceData inst = g_InstanceDataVS[In.InstID];

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);
    Out.Position = mul(float4(In.Position, 1.f), matWVP);

    Out.Texcoord = In.Texcoord;
    Out.WorldInv = inst.WorldInv;
    Out.DecalPos = inst.DecalPos;
    Out.DecalDir = inst.DecalDir;
    Out.HalfSize = inst.HalfSize;
    Out.LifeTime = inst.LifeTime;
    Out.DecalTime = inst.DecalTime;
    Out.TexIndex = inst.TexIndex;
    Out.bNormal = inst.bNormal;
    Out.DecalType = inst.DecalType;
    return Out;
}

//-------------------------------------------
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    row_major float4x4 WorldInv : TEXCOORD1;
    float3 DecalPos : TEXCOORD5;
    float3 DecalDir : TEXCOORD6;
    float3 HalfSize : TEXCOORD7;
    float LifeTime : TEXCOORD8;
    float DecalTime : TEXCOORD9;
    int TexIndex : TEXCOORD10;
    int bNormal : TEXCOORD11;
    int DecalType : TEXCOORD12;
};

struct PS_OUT
{
    float4 vDecal : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
};

//-------------------------------------------
float4 ComputeDecalEffect(float3 baseColor, float decalTime, float lifeTime)
{
    float lifeRatio = saturate(decalTime / lifeTime);

    // 🔹 Fade-out 커브 (느리게 → 빠르게 사라짐)
    // 0~1 범위를 부드러운 감쇠 곡선으로 변환
    float fadeOut = pow(1.0 - lifeRatio, 2.2); // 수치가 클수록 천천히 사라짐

    // 🔹 Glow (처음에 밝았다가 점점 어두워짐)
    float glowPhase = smoothstep(0.0, 0.4, 1.0 - lifeRatio); // 초반 Glow 강도
    float3 glowColor = baseColor * (1.0 + glowPhase * 3.0); // Glow는 색 강조

    // 🔹 최종 색상과 알파
    float3 finalColor = lerp(baseColor, glowColor, glowPhase);
    return float4(finalColor, fadeOut);
}


float4 Compute_WorldPos_byCamera(float2 vTexcoord)
{
    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    float fViewZ = vDepthDesc.y * g_fCamFar;

    float4 vWorldPos;
    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    return vWorldPos;
}

//-------------------------------------------
PS_OUT PS_Decal(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 screenUV = In.vPosition.xy / g_WinDowSize;
    float4 worldPos = Compute_WorldPos_byCamera(screenUV);

    float3 N = normalize(In.DecalDir);
    float3 up = (abs(N.y) < 0.999f) ? float3(0, 1, 0) : float3(1, 0, 0);
    float3 T = normalize(cross(up, N));
    float3 B = cross(N, T); // normalize 생략 가능 (T,B,N 직교계)

    float3x3 TBN = float3x3(T, B, N);

    float3 surfNormal = normalize(g_NormalTexture.SampleLevel(NoMipSampler, screenUV, 0).xyz * 2.f - 1.f);

    float4 localPos;

    if (In.DecalType == 0)
    { //BOX
        localPos = mul(worldPos, In.WorldInv);
      
        if (any(abs(localPos.xyz) > 1.f))
            discard;
        
        if (dot(normalize(surfNormal), N) < 0.1f)
            discard;
    }
    else
    {//SSD
        float3 rel = (worldPos.xyz - In.DecalPos) / In.HalfSize; 

        localPos = float4(mul(rel, transpose(TBN)), 1.f);

        if (any(abs(localPos.xy) > 1.f))
            discard;
    }

    float2 uv = localPos.xy * 0.5f + 0.5f;
    float4 decalSample = g_DecalArray.Sample(NoMipSampler, float3(uv, In.TexIndex));

    float edgeFadeVol = saturate(1.0f - max(abs(localPos.x), max(abs(localPos.y), abs(localPos.z))));
    float edgeFadePlanar = saturate(1.0f - max(abs(localPos.x), abs(localPos.y)));
    float edgeFade = (In.DecalType == 0) ? edgeFadeVol : edgeFadePlanar;

    // =========================
    // 생존도 페이드 (시간 경과 기반)
    // =========================
    // t = 현재까지 지난 비율 (0~1)
    float t = saturate(In.DecalTime / max(In.LifeTime, 0.0001f));

    // lifeFade = 1 → 막 생성됨
    // lifeFade = 0 → 거의 죽을 시간
    float lifeFade = 1.0f - t;

  
    float finalAlpha = decalSample.a * edgeFade * lifeFade;

    if (finalAlpha < 0.01f)
        discard;

    float4 finalRGB = ComputeDecalEffect(decalSample.rgb, In.DecalTime, In.LifeTime);

    if (In.bNormal != 0)
    {
        float3 nTS = g_DecalArray.Sample(NoMipSampler, float3(uv, 0)).xyz * 2.f - 1.f;
        nTS.z = sqrt(saturate(1 - dot(nTS.xy, nTS.xy)));

        float3 decalNormalWS = normalize(mul(nTS, TBN));

        // GBuffer normal 형식에 맞게 encode
        Out.vNormal = float4(decalNormalWS * 0.5f + 0.5f, 0.f);
    }
    else
    {
        Out.vNormal = float4(0, 0, 0, 0);
    }

    float fadeAlpha = saturate(finalRGB.a * decalSample.a);
    if (fadeAlpha < 0.02f)
        discard;

    
    Out.vDecal = float4(finalRGB.rgb, finalAlpha);

    return Out;
}


technique11 DecalTech
{
    pass Decal
    {
        SetRasterizerState(RS_Decal);
        SetDepthStencilState(DSS_DefaultNoWrite, 0);
        SetBlendState(BS_Decal, float4(0, 0, 0, 0), 0xffffffff);
        VertexShader = compile vs_5_0 VS_Decal();
        PixelShader = compile ps_5_0 PS_Decal();
    }
}
