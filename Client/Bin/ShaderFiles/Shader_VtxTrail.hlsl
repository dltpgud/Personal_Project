 #include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;

float4 g_vCamPosition;

struct VS_IN_TRAILE
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

struct VS_OUT_TRAILE
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

VS_OUT_TRAILE VS_Traile(VS_IN_TRAILE In)
{
    VS_OUT_TRAILE Out;

    float4 worldPos = mul(float4(In.pos, 1.f), g_WorldMatrix);
    float4 viewPos = mul(worldPos, g_ViewMatrix);
    Out.pos = mul(viewPos, g_ProjMatrix);

    Out.uv = In.uv;
    Out.color = In.color;

    return Out;
}

struct PS_IN_TRAILE
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRim : SV_TARGET3;
    vector vEmissive : SV_TARGET4;
    vector vOutLine : SV_TARGET5;
};

PS_OUT PS_TRAILE(PS_IN_TRAILE In)
{
    PS_OUT Out = (PS_OUT) 0;
    // 텍스처 색상 샘플링
    float4 texColor = g_Texture.Sample(LinearSampler, In.uv);

    // 정점 컬러(알파 포함)와 곱하기
    Out.vDiffuse = texColor * In.color;;
    return Out;
}
technique11 DefaultTechnique
{
    pass DefaultPass0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthRead, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Traile();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAILE();
    }
}
