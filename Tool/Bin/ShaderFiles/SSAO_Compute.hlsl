//=====================================================================
// SSAO_Combined.hlsl (cs_5_0)
//  - Compute SSAO + BlurX + BlurY
//  - Cartoon style (RoboQuest-like) SSAO
//=====================================================================
#include "Engine_Shader_Defines.hlsli"

//=====================================================================
// Common Resources
//=====================================================================
Texture2D g_DepthTex : register(t0); // R=clipZ, G=viewZ/far
Texture2D g_NormalTex : register(t1); // (N*0.5+0.5)

// SSAO 계산 결과를 Blur X/Y에 입력용으로 사용
Texture2D g_AOInput : register(t2); // for Blur X
Texture2D g_AOBlurXIn : register(t3); // for Blur Y

// 출력용 UAV
RWTexture2D<float> g_AOOut : register(u0); // SSAO output
RWTexture2D<float> g_AOBlurX : register(u1); // Temp (X blur)
RWTexture2D<float> g_AOBlurY : register(u2); // Final AO result


//=====================================================================
// Constant Buffer
//=====================================================================
cbuffer SSAO_CB : register(b0)
{
    float4x4 g_View;
    float4x4 g_Proj;
    float4x4 g_ProjInv;
    float4 g_ProjParams; // (1/far, far, near, 0)
    uint g_Width;
    uint g_Height;
    float g_Radius;
    float g_Bias;
    float g_AOIntensity; // AO overall strength (0~1)
};

//=====================================================================
// Utilities
//=====================================================================
float2 Hash2(float2 p)
{
    p = frac(p * float2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return frac(float2(p.x * p.y, p.x + p.y));
}

float3 ReconstructViewPos(float2 uv, float depthClip, float viewZNorm)
{
    float4 clip = float4(uv * 2.0f - 1.0f, depthClip, 1.0f);
    float4 view = mul(clip, g_ProjInv);
    view.xyz /= view.w;

    float linearZ = viewZNorm * g_ProjParams.y;
    float s = linearZ / max(abs(view.z), 1e-4);
    view.xyz *= s;
    return view.xyz;
}

//=====================================================================
// SSAO Kernel
//=====================================================================
static const int KERNEL_COUNT = 16;
static const float3 KERNEL[KERNEL_COUNT] =
{
    float3(0.50, 0.20, 0.70), float3(-0.30, 0.40, 0.60),
    float3(0.20, -0.50, 0.40), float3(-0.60, -0.20, 0.30),
    float3(0.70, -0.10, 0.20), float3(0.10, 0.60, 0.30),
    float3(-0.40, 0.10, 0.50), float3(0.30, -0.30, 0.70),
    float3(-0.20, -0.60, 0.40), float3(0.60, 0.30, 0.10),
    float3(-0.50, 0.20, 0.40), float3(0.20, 0.30, 0.60),
    float3(0.40, -0.40, 0.20), float3(-0.30, 0.50, 0.10),
    float3(0.10, -0.20, 0.50), float3(-0.10, -0.10, 0.60)
};

//=====================================================================
// 1️⃣ SSAO Compute Pass
//=====================================================================
[numthreads(16, 16, 1)]
void CS_SSAO(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x >= g_Width || DTid.y >= g_Height)
        return;

    float2 uv = (DTid.xy + 0.5f) / float2(g_Width, g_Height);
    float2 depthRG = g_DepthTex.SampleLevel(PointSamplerClamp, uv, 0).rg;
    float depthClip = depthRG.x;
    float viewZNorm = depthRG.y;

    if (depthClip >= 1.0f - 1e-6)
    {
        g_AOOut[DTid.xy] = 1.0f;
        return;
    }

    float3 nWorld = g_NormalTex.SampleLevel(LinearSamplerClamp, uv, 0).xyz * 2.0f - 1.0f;
    float3 normalVS = normalize(mul(nWorld, (float3x3) g_View));
    float3 posVS = ReconstructViewPos(uv, depthClip, viewZNorm);

    float2 h = Hash2(uv * float2(g_Width, g_Height));
    float3 randVec = normalize(float3(h * 2.0f - 1.0f, 0.0f));
    float3 tangent = normalize(randVec - normalVS * dot(randVec, normalVS));
    float3 bitangent = cross(normalVS, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normalVS);

    float occlusion = 0.0f;
    [unroll]
    for (int i = 0; i < KERNEL_COUNT; ++i)
    {
        float scale = (i + 1) / (float) KERNEL_COUNT;
        float3 dir = normalize(KERNEL[i]);
        dir = mul(dir, TBN);
        float3 samplePosVS = posVS + dir * (g_Radius * (0.2 + 0.8 * scale));

        float4 proj = mul(float4(samplePosVS, 1.0f), g_Proj);
        proj.xy /= proj.w;
        float2 suv = proj.xy * 0.5f + 0.5f;
        if (any(suv < 0.0f) || any(suv > 1.0f))
            continue;

        float2 sDepthRG = g_DepthTex.SampleLevel(PointSamplerClamp, suv, 0).rg;
        float3 sVS = ReconstructViewPos(suv, sDepthRG.x, sDepthRG.y);

        float diff = sVS.z - samplePosVS.z;
        float rangeCheck = smoothstep(0.0f, 1.0f, g_Radius / (abs(diff) + 1e-4));
        occlusion += step(-g_Bias, diff) * rangeCheck * 0.6f;
    }

    float ao = 1.0f - (occlusion / (float) KERNEL_COUNT);
    ao = pow(ao, 1.5f); // brightness correction
    ao = floor(ao * 3.0f) / 3.0f; // quantization (toon-like)
    ao = lerp(1.0f, ao, g_AOIntensity); // AO intensity mix

    g_AOOut[DTid.xy] = ao;
}

//=====================================================================
// 2️⃣ Blur X Pass
//=====================================================================
[numthreads(16, 16, 1)]
void CS_BlurX(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x >= g_Width || DTid.y >= g_Height)
        return;

    float2 uv = (DTid.xy + 0.5f) / float2(g_Width, g_Height);
    float sum = 0.0, wsum = 0.0;

    [unroll]
    for (int i = -2; i <= 2; ++i)
    {
        float w = exp(-abs(i) * 0.8);
        float2 offset = float2(i, 0) / float2(g_Width, 1.0f);
        sum += g_AOInput.SampleLevel(LinearSamplerClamp, uv + offset, 0).r * w;
        wsum += w;
    }

    g_AOBlurX[DTid.xy] = sum / wsum;
}

//=====================================================================
// 3️⃣ Blur Y Pass
//=====================================================================
[numthreads(16, 16, 1)]
void CS_BlurY(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x >= g_Width || DTid.y >= g_Height)
        return;

    float2 uv = (DTid.xy + 0.5f) / float2(g_Width, g_Height);
    float sum = 0.0, wsum = 0.0;

    [unroll]
    for (int j = -2; j <= 2; ++j)
    {
        float w = exp(-abs(j) * 0.8);
        float2 offset = float2(0, j) / float2(1.0f, g_Height);
        sum += g_AOBlurXIn.SampleLevel(LinearSamplerClamp, uv + offset, 0).r * w;
        wsum += w;
    }

    g_AOBlurY[DTid.xy] = sum / wsum;
}
