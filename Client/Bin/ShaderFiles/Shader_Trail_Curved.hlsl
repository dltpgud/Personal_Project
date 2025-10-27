#include "Engine_Shader_Defines.hlsli"

// ===============================================
// GPU 트레일용 상수 / 텍스처
// ===============================================
matrix g_ViewMatrix, g_ProjMatrix;

Texture2D g_TrailTexture; // 트레일용 텍스처 (중앙 밝고 양끝 투명)

float g_fCamFar;
float4 g_vCamPosition;
float g_TimeSum;

// ===============================================
// 버퍼 (ComputeShader_TrailUpdate 에서 생성됨)
// ===============================================
StructuredBuffer<float3> RenderBuffer : register(t0); // ForRender
StructuredBuffer<uint> AliveIndexBuf : register(t1); // 정렬된 인덱스

// ===============================================
// VS 입력/출력 구조체
// ===============================================
struct VS_IN
{
    uint vVertexID : SV_VertexID;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float fAlpha : TEXCOORD1;
};

// ===============================================
// Vertex Shader
// - AliveIndexBuf 에서 순서대로 점 읽어 화면 투영
// ===============================================
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    uint idx = AliveIndexBuf[In.vVertexID];
    float3 vViewPos = RenderBuffer[idx];

    // ViewSpace → ClipSpace
    float4 vProjPos = mul(float4(vViewPos, 1.f), g_ProjMatrix);
    Out.vPosition = vProjPos;

    // UV: 꼬리→헤드로 진행
    Out.vTexcoord = float2((float) In.vVertexID / 256.0f, 0.5f); // 256은 예시 (Trail_MaxPoint와 연동 가능)

    // 수명 기반 그라데이션 (꼬리 쪽 알파 감소)
    Out.fAlpha = saturate(1.0f - Out.vTexcoord.x);

    return Out;
}

// ===============================================
// Pixel Shader
// ===============================================
struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vEmissive : SV_TARGET4;
};

PS_OUT PS_MAIN(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 트레일 텍스처 샘플
    vector vTrail = g_TrailTexture.Sample(LinearSampler, In.vTexcoord);

    // 색상/광도
    float3 baseColor = float3(0.1f, 1.0f, 0.4f); // 밝은 초록색 빛줄기
    float3 emissiveColor = baseColor * vTrail.a * In.fAlpha * 2.0f;

    Out.vDiffuse = float4(baseColor * In.fAlpha, 0.5f);
    Out.vNormal = float4(0.5f, 0.5f, 1.f, 0.f);
    Out.vEmissive = float4(emissiveColor, 1.f);
    Out.vDepth = float4(In.vPosition.z / In.vPosition.w, In.vPosition.w / g_fCamFar, 0.f, 0.f);

    return Out;
}

// ===============================================
// 발광 강조 (예: 더 밝은 버전)
// ===============================================
PS_OUT PS_Glow(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

   // vector vTrail = g_TrailTexture.Sample(LinearSampler, In.vTexcoord);
    
    //if (all(vTrail.rgb) == 0)
    vector vTrail = float4(0.f, 1.f, 0.f, 0.f);
    
    float3 glow = float3(0.2f, 1.0f, 0.5f) * vTrail.a * In.fAlpha * 3.0f;

    Out.vDiffuse = float4(glow, 0.8f);
    Out.vNormal = float4(0.5f, 0.5f, 1.f, 0.f);
    Out.vEmissive = float4(glow, 1.f);
    Out.vDepth = float4(In.vPosition.z / In.vPosition.w, In.vPosition.w / g_fCamFar, 0.f, 0.f);
    return Out;
}

// ===============================================
// Technique
// ===============================================
technique11 TrailTechnique
{
    // ──────────────────────────────
    // 1. 기본 트레일 패스
    // ──────────────────────────────
    pass Trail_Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Decal, 0);
        SetBlendState(BS_Additive, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    // ──────────────────────────────
    // 2. 발광 강조 패스
    // ──────────────────────────────
    pass Trail_Glow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Decal, 0);
        SetBlendState(BS_Additive, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Glow();
    }
}
