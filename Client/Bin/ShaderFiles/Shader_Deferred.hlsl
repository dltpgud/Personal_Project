
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_LightViewMatrixInv, g_LightProjMatrixInv;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

Texture2D g_SSAOTexture;
Texture2D g_VFXTexture;
texture2D g_Texture;
Texture2D g_BloomTexture;
vector g_vLightDir;
vector g_vLightPos;
float  g_fLightRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

texture2D g_EffectTexture;
texture2D g_NormalTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_ShadeTexture;
texture2D g_DiffuseTexture;
texture2D g_LightDepthTexture;
texture2D g_DecalNormalTexture;
texture2D g_DecalTexture;


texture2D g_AccumTexture;
texture2D g_AccumNormalTexture;
texture2D g_RevealageTexture;

texture2D g_EmissiveTexture;
texture2D g_RimTexture;

texture2D g_FinalTexture;
texture2D g_BlurTexture;
texture2D g_BrightnessTexTure;

texture2D g_vMtrlAmbient;
vector g_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

vector g_vCamPosition;
  bool g_SSAOEnable; 
float2 g_WinDowSize;
float g_fCamFar;
texture2D g_FogMaskTex;
float  g_fFogTime;
// === Fog params ===
// 안개 색
float3 g_FogColorNear = float3(0.96, 0.58, 0.40); // 근거리 (붉은 사막톤)
float3 g_FogColorFar = float3(0.92, 0.72, 0.55); // 원거리 (밝은 황토톤)

// 거리 기반 범위
float g_FogStart = 15.0f; // 안개 시작 거리 (카메라로부터)
float g_FogEnd = 300.0f; // 안개 끝 (이 뒤는 완전히 안개 속)
float g_FogDensity = 0.45f; // 전체 강도 (0.3~0.6 사이 튜닝)


// 텍스쳐에서 한 픽셀의 간격
float dX;
float dY;
float Bloom_Weights[5] = { 0.0545, 0.2442, 0.4026, 0.2442, 0.0545 };
float2 g_shadowMapSize;


float4 Compute_WorldPos_byCamera(float2 vTexcoord)
{
    float4 vWorldPos = 0.f;

    vector vDepthDesc = g_DepthTexture.Sample(PointSamplerClamp, vTexcoord);

    float fViewZ = vDepthDesc.y * g_fCamFar;
	
    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    return vWorldPos;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;

    // --- 노멀 계산 ---
    float3 baseNormal = normalize(g_NormalTexture.Sample(LinearSamplerClamp, In.vTexcoord).xyz * 2 - 1);
    float3 decalNormal = g_DecalNormalTexture.Sample(LinearSamplerClamp, In.vTexcoord).xyz * 2 - 1;
    float decalWeight = g_DecalTexture.Sample(LinearSamplerClamp, In.vTexcoord).a;
    float3 finalNormal;
    if (all(decalNormal.xyz <= 0))
        finalNormal = baseNormal;
    else
    {
        finalNormal = normalize(lerp(baseNormal, decalNormal, decalWeight));
    }
    
    float3 L = normalize(-g_vLightDir.xyz);
    float NdotL = saturate(dot(finalNormal, L));

    // === 툰단계 제거, 대신 라이트 랩으로 부드러운 명암 ===
    float lightWrap = 0.25f; // 0.0~0.4 (값을 높이면 그림자 경계가 부드러워짐)
    float wrapped = saturate((NdotL + lightWrap) / (1.0f + lightWrap));

    // SSAO 읽기
    float fSSAO = 1.0f;
    if (g_SSAOEnable)
    {
        fSSAO = saturate(g_SSAOTexture.Sample(LinearSamplerClamp, In.vTexcoord).r); // 재선언 X
        fSSAO = pow(fSSAO, 0.85f);
    }
    // 어두운 영역에서만 AO 강화
    float aoMix = lerp(1.0f, fSSAO, 1.0f - wrapped);

    // 조명 색상
    float3 ambient = g_vLightAmbient.rgb * g_vMtrlAmbient.Sample(LinearSamplerClamp, In.vTexcoord).rgb;
    float3 diffuse = g_vLightDiffuse.rgb * wrapped;

    // 스펙큘러 (살짝 강화)
    float3 V = normalize(g_vCamPosition.xyz - Compute_WorldPos_byCamera(In.vTexcoord).xyz);
    float3 H = normalize(L + V);
    float spec = pow(saturate(dot(finalNormal, H)), 64.0f);
    spec *= smoothstep(0.3f, 0.7f, wrapped);

    // 최종 조명 계산
    Out.vShade.rgb = (ambient + diffuse * aoMix);
    Out.vSpecular.rgb = g_vLightSpecular.rgb * g_vMtrlSpecular.rgb * spec * 0.7f;
    Out.vShade.a = 1.0f;
    return Out;
}


PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;

    // --- 노멀 계산 ---
    float3 baseNormal = normalize(g_NormalTexture.Sample(LinearSamplerClamp, In.vTexcoord).xyz * 2 - 1);
    float3 decalNormal = g_DecalNormalTexture.Sample(LinearSamplerClamp, In.vTexcoord).xyz * 2 - 1;
    float decalWeight = g_DecalTexture.Sample(LinearSamplerClamp, In.vTexcoord).a;
    float3 finalNormal;
    if (all(decalNormal.xyz <= 0))
        finalNormal = baseNormal;
    else
    {
        finalNormal = normalize(lerp(baseNormal, decalNormal, decalWeight));
    }

    float4 vWorldPos = Compute_WorldPos_byCamera(In.vTexcoord);
    float3 L = g_vLightPos.xyz - vWorldPos.xyz;

    // 거리와 감쇠
    float fDistance = length(L);
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    L = normalize(L);

    // --- 기본 조명 ---
    float NdotL = saturate(dot(finalNormal, L));

    // 부드러운 명암 (라이트 랩)
    float lightWrap = 0.25f;
    float wrapped = saturate((NdotL + lightWrap) / (1.0f + lightWrap));

    // --- SSAO 적용 ---
    float fSSAO = 1.0f;
    if (g_SSAOEnable)
    {
        fSSAO = saturate(g_SSAOTexture.Sample(LinearSamplerClamp, In.vTexcoord).r);
        fSSAO = pow(fSSAO, 0.85f);
    }

    // 어두운 영역에서만 AO 강화
    float aoMix = lerp(1.0f, fSSAO, 1.0f - wrapped);

    // --- Ambient / Diffuse ---
    float3 ambient = g_vLightAmbient.rgb * g_vMtrlAmbient.Sample(LinearSamplerClamp, In.vTexcoord).rgb;

    // --- Specular ---
    float3 V = normalize(vWorldPos.xyz - g_vCamPosition.xyz);
    float3 H = normalize(L + V);
    float spec = pow(saturate(dot(finalNormal, H)), 30.0f) * fAtt;
  

    // --- 최종 결과 ---
    Out.vShade.rgb = (g_vLightDiffuse.rgb * saturate(aoMix + (g_vLightAmbient.rgb * ambient))) * fAtt;
    Out.vSpecular.rgb = g_vLightSpecular.rgb * g_vMtrlSpecular.rgb * spec * 0.8f ;

    return Out;
}


float Compute_OutLine(float2 vTexcoord)
{
    float OutLine = 1.f;

    vector NormalSample = g_NormalTexture.Sample(LinearSamplerClamp, vTexcoord);
    float depthCenter = g_DepthTexture.Sample(LinearSamplerClamp, vTexcoord).r;

    if ( depthCenter <= 0.f)
        return OutLine;
    
    float2 dx = float2(1.f / g_WinDowSize.x, 0.f);
    float2 dy = float2(0.f, 1.f / g_WinDowSize.y);

    // 인접 노멀/깊이 샘플
    float3 nMid = normalize(NormalSample.xyz * 2.f - 1.f);
    float3 nLeft = normalize(g_NormalTexture.Sample(LinearSamplerClamp, vTexcoord - dx).xyz * 2.f - 1.f);
    float3 nRight = normalize(g_NormalTexture.Sample(LinearSamplerClamp, vTexcoord + dx).xyz * 2.f - 1.f);
    float3 nUp = normalize(g_NormalTexture.Sample(LinearSamplerClamp, vTexcoord - dy).xyz * 2.f - 1.f);
    float3 nDown = normalize(g_NormalTexture.Sample(LinearSamplerClamp, vTexcoord + dy).xyz * 2.f - 1.f);

    float dLeft = g_DepthTexture.Sample(LinearSamplerClamp, vTexcoord - dx).r;
    float dRight = g_DepthTexture.Sample(LinearSamplerClamp, vTexcoord + dx).r;
    float dUp = g_DepthTexture.Sample(LinearSamplerClamp, vTexcoord - dy).r;
    float dDown = g_DepthTexture.Sample(LinearSamplerClamp, vTexcoord + dy).r;

    const float depthEdgeLimit = 0.0015f;
    bool depthEdge = ( abs(depthCenter - dLeft) > depthEdgeLimit ||  abs(depthCenter - dRight) > depthEdgeLimit
    || abs(depthCenter - dUp) > depthEdgeLimit || abs(depthCenter - dDown) > depthEdgeLimit );
 
    float dotL = dot(nMid, nLeft);
    float dotR = dot(nMid, nRight);
    float dotU = dot(nMid, nUp);
    float dotD = dot(nMid, nDown);

    float normalDiff = 1.f - min(min(dotL, dotR), min(dotU, dotD));

    float3 viewDir = float3(0.f, 0.f, 1.f); // 화면 정면 기준
    float ndotv = abs(dot(nMid, viewDir));
    float normalThreshold = lerp(0.01f, 0.1f, ndotv); 

    if (normalDiff > normalThreshold || depthEdge)
        OutLine = 0.f;

    return OutLine;
}


void ProjectToLight(float4 worldPos, out float2 uv, out float depth, out bool valid)
{
    float4 lp = mul(worldPos, g_LightViewMatrix);
    lp = mul(lp, g_LightProjMatrix);

    // D3D: NDC z = z/w ∈ [0,1]  (0.5+0.5 하면 안 됨)
    float invw = rcp(lp.w);
    float nx = lp.x * invw; // -1..1
    float ny = lp.y * invw; // -1..1
    float nz = lp.z * invw; //  0..1  (D3D)

    // 텍스처 좌표 변환 (y 반전)
    uv = float2(nx * 0.5f + 0.5f, -ny * 0.5f + 0.5f);
    depth = saturate(nz);

    // 클립 (경계 밖은 섀도우 0으로 처리)
    valid = all(uv >= 0.0f) && all(uv <= 1.0f) && (lp.w > 0.0f);
}

float ComputeReceiverBias(float3 n, float3 l, float3 v)
{
    // 기저 편향 + 경사 기반(슬로프) 편향
    const float baseBias = 0.0005f; // 필요시 0.0003~0.002 범위에서 튜닝
    const float slopeBias = (1.0f - saturate(dot(n, -l))) * 0.001f;
    const float viewBias = (1.0f - abs(dot(n, v))) * 0.001f; // 뷰 방향 기반 추가 보정
    return baseBias + slopeBias + viewBias;
}

// 3x3 PCF (SampleCmpLevelZero 사용)
float PCF_Shadow(float2 uv, float depth, float2 shadowMapSize)
{
    float2 texel = 1.0f / shadowMapSize;

    float sum = 0.0f;
    [unroll]
    for (int y = -1; y <= 1; ++y)
    {
        [unroll]
        for (int x = -1; x <= 1; ++x)
        {
            float2 o = float2(x, y) * texel;
            // SampleCmpLevelZero: 비교값(depth)을 주면 샘플 내부에서 비교 수행
            sum += g_LightDepthTexture.SampleCmpLevelZero(ShadowCmpSampler, uv + o, depth);
        }
    }
    return sum / 9.0f; // 1이면 완전 밝음(그림자 X), 0이면 어둠(그림자 100%)
}

// 0..1 깊이 안개 팩터 (시작~끝 사이에서 선형)
float FogFactorDepth(float viewZ)
{
    float t = saturate((viewZ - g_FogStart) / max(1e-4, (g_FogEnd - g_FogStart)));
    // 밀도(지수형)을 살짝 섞어서 더 자연스럽게
    float expFog = 1.0f - exp(-t * g_FogDensity * 2.0f);
    return saturate(lerp(t, expFog, 0.5f));
}

float3 GammaCorrection(float3 color, float gamma)
{
    return pow(color, 1.0f / gamma); // 스칼라 값을 벡터의 각 요소에 적용
}

PS_OUT PS_MAIN_LIGHT_COMBINE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 
     // ==== 기본 텍스처 샘플 ====
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    float fOutLine = Compute_OutLine(In.vTexcoord);
   
    
    // ==== 데칼이랑 외곽선 반영 ====
    vector vDecal = g_DecalTexture.Sample(LinearSampler, In.vTexcoord);
    
    vDiffuse.rgb = lerp(vDiffuse.rgb, vDecal.rgb, vDecal.a);
     
    if (vDiffuse.a == 0.f)
        discard;
 
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fCurIsSpec = vDepthDesc.z;
    if (fCurIsSpec == 0.f)
        vSpecular = 0.f;
 
    // ==== 월드 / 뷰 좌표 복원 ====
    float4 vPositionWS = Compute_WorldPos_byCamera(In.vTexcoord);
    float viewZ = vDepthDesc.y * g_fCamFar;
 
    // ==== 섀도우 계산 ====
    float2 lightUV;
    float lightDepth;
    bool inLight = false;
    ProjectToLight(vPositionWS, lightUV, lightDepth, inLight);
 
    float shadowFactor = 1.0f;
    if (inLight)
    {
        float3 n = normalize(g_NormalTexture.Sample(PointSampler, In.vTexcoord).xyz * 2.0f - 1.0f);
        float3 l = normalize(g_vLightDir.xyz);
        float3 v = normalize(g_vCamPosition.xyz - vPositionWS.xyz);
        float depthWithBias = lightDepth - ComputeReceiverBias(n, l, v);
        shadowFactor = PCF_Shadow(lightUV, depthWithBias, g_shadowMapSize);
    }
    
    // ==== 조명 합성 ====
    float3 baseLit = (vDiffuse * fOutLine * vShade + vSpecular).rgb;
    baseLit *= lerp(0.8f, 1.0f, shadowFactor);
     
    //====감마 설정 ===
     baseLit.rgb = GammaCorrection(baseLit.rgb, 0.8f);
    Out.vColor.rgb = baseLit;
    return Out;
}

PS_OUT PS_MAIN_Final(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vBloom = g_BloomTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vFinalDesc = g_FinalTexture.Sample(PointSampler, In.vTexcoord);
    vector vRim = g_RimTexture.Sample(LinearSampler, In.vTexcoord);
    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vEffect = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);

    // === Bloom_Occlusion====
    float bloomDepth = vBloom.a;
    float sceneDepth = vDepthDesc.g;
    float diff = bloomDepth - sceneDepth;
    if (diff > 0.5f)
    {
        vBloom.rgb = 0.0f;
    }
    
    vFinalDesc = vFinalDesc + vEffect + vRim + vBloom;
    float3 emissive = vEmissive.rgb;
    
     // ==== 안개 계산 ====
     // 깊이 기반
    float4 vPositionWS = Compute_WorldPos_byCamera(In.vTexcoord);
    float viewZ = vDepthDesc.y * g_fCamFar;
   
    float fd = FogFactorDepth(viewZ);
    float fogT = saturate(fd * g_FogDensity);
     // 거리 따라 색상 그라데이션
    float3 fogColor = lerp(g_FogColorNear, g_FogColorFar, fd);
     
     // ==== 안개 블렌딩 ====
     // 에미시브는 약하게만 감쇠
    float3 foggedBase = lerp(fogColor, vFinalDesc.rgb, 1.0f - fogT);
    float emissiveFog = lerp(1.0f, 1.0f - fogT, 0.3f);
    float3 finalRGB = foggedBase + emissive * emissiveFog;
 
    Out.vColor = float4(finalRGB, 1.f);
    return Out;
}

PS_OUT PS_MAIN_PURE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexcoord);
    
    Out.vColor = vDiffuse;
    
    return Out;
}

// 블러 X
PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    
    int i;
    for (i = 0; i < 5; i++)
    {
        vDiffuse += Bloom_Weights[i] * g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexcoord + float2(dX, 0.0) * float(i - 2));
    }
    
    Out.vColor = vDiffuse;

    return Out;
}

// 블러 Y
PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    
    int i;
    for (i = 0; i < 5; i++)
    {
        vDiffuse += Bloom_Weights[i] * g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexcoord + float2(0.0, dY) * float(i - 2));
    }
    
    Out.vColor = vDiffuse;

    return Out;
}

struct PS_DECAL
{
    float4 vDecal : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    
};


PS_DECAL PS_COMPOSITE_BLEND(PS_IN In)
{
    PS_DECAL Out = (PS_DECAL)0;

    float4 accumColor  = g_AccumTexture.Sample(LinearSampler, In.vTexcoord);
    float4 accumNormal = g_AccumNormalTexture.Sample(LinearSampler, In.vTexcoord);
    float revealage    = g_RevealageTexture.Sample(LinearSampler, In.vTexcoord).r;

    float colorWeight  = max(accumColor.a, 1e-5f);
    float normalWeight = max(accumNormal.a, 1e-5f);

    float3 finalColor  = accumColor.rgb / colorWeight;
    float3 finalNormal = normalize(accumNormal.rgb / normalWeight);

    Out.vDecal  = float4(finalColor, saturate(revealage));
    Out.vNormal = float4(finalNormal * 0.5f + 0.5f, 1.0f);

    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DefaultNoWrite, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass LIGHT_COMBINE //3
    {
        SetRasterizerState(RS_Shadow);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_COMBINE();
    }

    pass Pure //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PURE();
    }
   
    pass BLUR_X //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass BLUR_Y //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }


    pass Final //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Final();
    }

    pass CompositeBlend8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COMPOSITE_BLEND();
    }
}
