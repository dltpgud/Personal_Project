#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float2 g_WinDowSize;
float g_fCamFar;
float _pad0;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
vector g_vCamPosition;

Texture2D g_DepthTexture : register(t10);
Texture2D g_NormalTexture : register(t11);
Texture2DArray g_DecalArray : register(t12);

struct DecalInstanceData
{
    row_major float4x4 WorldInv; 
    float3 DecalPos; 
    float _padA; 
    float3 DecalDir; 
    float _padB; 
    float3 HalfSize; 
    float LifeTime; 
    float DecalTime; 
    int TexIndex; 
    int DecalType; 
    int bNormal; 
}; 

StructuredBuffer<DecalInstanceData> g_InstanceDataVS : register(t9);

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

float4 ComputeDecalEffect(float3 baseColor, float decalTime, float lifeTime)
{
    float lifeRatio = saturate(decalTime / lifeTime);

    float fadeOut = pow(1.0 - lifeRatio, 2.2); // 수치가 클수록 천천히 사라짐
    
    float glowPhase = smoothstep(0.0, 0.4, 1.0 - lifeRatio); // 초반 Glow 강도
    float3 glowColor = baseColor * (1.0 + glowPhase * 3.0); // Glow는 색 강조
    
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

struct PS_OUT
{
    float4 vAccum : SV_Target0; // 색상+알파 누적
    float4 vAccumNormal : SV_Target1; // 노말 누적
    float vRevealage : SV_Target2; // 투명 누적
};

PS_OUT PS_Decal(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 screenUV = In.vPosition.xy / g_WinDowSize;
    float4 worldPos = Compute_WorldPos_byCamera(screenUV);

    float3 N = normalize(In.DecalDir);
    float3 up = (abs(N.y) < 0.999f) ? float3(0, 1, 0) : float3(1, 0, 0);
    float3 T = normalize(cross(up, N));
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);

    float3 surfNormal = normalize(g_NormalTexture.SampleLevel(NoMipSampler, screenUV, 0).xyz * 2.f - 1.f);

    float4 localPos;

    if (In.DecalType == 0)
    {
        localPos = mul(worldPos, In.WorldInv);
        if (any(abs(localPos.xyz) > 1.f))
            discard;
        
        
        if (dot(normalize(surfNormal), N) < 0.1f)
            discard;
    }
    else
    {
        float3 rel = (worldPos.xyz - In.DecalPos) / In.HalfSize;
        localPos = float4(mul(rel, transpose(TBN)), 1.f);
        if (any(abs(localPos.xy) > 1.f))
            discard;
    }

    float2 uv = localPos.xy * 0.5f + 0.5f;
    float4 decalSample = g_DecalArray.Sample(NoMipSampler, float3(uv, In.TexIndex));

    float t = saturate(In.DecalTime / max(In.LifeTime, 0.0001f));
    float lifeFade = 1.0f - t;


    float3 camToPixel = worldPos.xyz - g_vCamPosition.xyz;
    float depth = length(camToPixel) / g_fCamFar; // 0 ~ 1 범위 (Near~Far)
    depth = saturate(depth);

    float weightDepth = 1e-4 / (1e-4 + depth * depth * 1e-2);

    float weightalpha = decalSample.a * lifeFade * weightDepth;
    weightalpha = saturate(weightalpha * 2.0f);

    if (weightalpha < 0.01f)
        discard;
    
    float4 finalRGB = ComputeDecalEffect(decalSample.rgb, In.DecalTime, In.LifeTime);

    Out.vAccum = float4(finalRGB.rgb * weightalpha, weightalpha);

    if (In.bNormal != 0)
    {
        float3 nTS = g_DecalArray.Sample(NoMipSampler, float3(uv, 0)).xyz * 2.f - 1.f;
        nTS.z = sqrt(saturate(1 - dot(nTS.xy, nTS.xy)));
        float3 decalNormalWS = normalize(mul(nTS, TBN));
        float normalWeight = weightalpha * weightalpha;
        Out.vAccumNormal = float4(decalNormalWS * normalWeight, normalWeight);
    }
    else
    {
        Out.vAccumNormal = float4(0, 0, 0, 0);
    }

 
    Out.vRevealage += weightalpha;

    return Out;
}



technique11 Decal_WBOIT
{
    pass P0
    {
        SetRasterizerState(RS_Decal);
        SetDepthStencilState(DSS_DefaultNoWrite, 0);
        SetBlendState(BS_WBOIT, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Decal();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Decal();
    }
}