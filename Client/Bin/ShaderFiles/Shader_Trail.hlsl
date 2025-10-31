//------------------------------------------------------------------------------
// TrailRender.hlsl
// - GPU에서 생성된 TrailVertex를 렌더링
// - TrailHeader.frameIndex에 따라 각 트레일마다 고정된 프레임 영역 샘플링
//------------------------------------------------------------------------------
#include "Engine_Shader_Defines.hlsli"

//------------------------------------------------------------------------------
// Constant Buffer
//------------------------------------------------------------------------------
cbuffer PerFrame : register(b0)
{
    float4x4 g_View;
    float4x4 g_Proj;
    float3 g_CamPosWS;
    float _pad0;

    float2 g_TrailUVScale; // (x, y). y는 보통 타일링 스케일
    float2 _pad1;

    uint g_TotalFrames; 
    float3 _pad2;
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

float g_fCamFar;
//------------------------------------------------------------------------------
// Input / Output 구조체
//------------------------------------------------------------------------------
struct VS_IN
{
    float3 pos : POSITION; // 12B
    float2 uv : TEXCOORD0; //  8B
    float4 color : COLOR0; // 16B
    uint trailId : TEXCOORD1;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    uint instID : SV_InstanceID; //  트레일 ID 전달
};

//------------------------------------------------------------------------------
// Resources
//------------------------------------------------------------------------------
Texture2D g_TrailTex : register(t3);
StructuredBuffer<TrailHeader> g_Header : register(t4); //  각 트레일 헤더 (frameIndex 포함)

//------------------------------------------------------------------------------
// Vertex Shader
//------------------------------------------------------------------------------
VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out;

    float4 v = mul(float4(In.pos, 1.0f), g_View);
    Out.posH = mul(v, g_Proj);

    TrailHeader H = g_Header[In.trailId];

    Out.uv = In.uv * g_TrailUVScale; // 일반형: 타일링 유지
    Out.color = In.color;
    Out.instID = In.trailId; // 🔹 트레일 식별자 전달

    return Out;
}
struct PS_IN
{
    float4 posH : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    uint instID : SV_InstanceID;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRim : SV_TARGET3;
    vector vEmissive : SV_TARGET4;
    vector vAmbient : SV_TARGET5;
    vector vBloom : SV_TARGET6;
};

//------------------------------------------------------------------------------
// Pixel Shaders
//------------------------------------------------------------------------------
PS_OUT PS_TrailSpriteTexture(PS_IN In) : SV_Target
{
    PS_OUT Out = (PS_OUT) 0;
    //가로 프레임 
    TrailHeader H = g_Header[In.instID];

        uint frameIndex = H.frameIndex % max(g_TotalFrames, 1);

        float2 frameSize = float2(1.0f / (float) g_TotalFrames, 1.0f);
        float2 uv = In.uv * frameSize + float2(frameSize.x * frameIndex, 0.0f);

        float4 tex = g_TrailTex.Sample(LinearSampler, uv);
    
        if (tex.a <= 0.1f)
            discard;
    Out.vEmissive = tex * In.color;
    Out.vBloom = float4(tex.rgb * In.color.rgb, In.posH.w / g_fCamFar);
    
    return Out;
}

PS_OUT PS_TrailColor(PS_IN In) : SV_Target
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vEmissive = In.color;
    Out.vBloom = float4(In.color.rgb, In.posH.w / g_fCamFar);
    return Out;
}

PS_OUT PS_TrailTexture(PS_IN In) : SV_Target
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 tex = g_TrailTex.Sample(LinearSampler, In.uv);
    Out.vEmissive = In.color;
    Out.vBloom = float4(tex.rgb * In.color.rgb, In.posH.w / g_fCamFar);
    return Out;
}

//------------------------------------------------------------------------------
// Technique
//------------------------------------------------------------------------------
technique11 TrailRender
{
    // 텍스처 트레일 (frameIndex 기반)
    pass P0_TextureTrail
    {
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_TrailSpriteTexture();
    }

    // 컬러 트레일
    pass P1_CurveTrail
    {
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_TrailColor();
    }

    // 오리지널 텍스쳐 
    pass P3_TextureTrail
    {
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_DefaultNoWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        PixelShader = compile ps_5_0 PS_TrailTexture();
    }
}
