#include "Engine_Shader_Defines.hlsli"


//--------------------------------------
// Constant buffer
//--------------------------------------
matrix g_ViewMatrix;
matrix g_ProjMatrix;
float3 g_vCamPos;

StructuredBuffer<float4> g_TrailPoints : register(t0);
StructuredBuffer<uint4> g_TrailHeaders : register(t1);

//--------------------------------------
// Vertex → Geometry → Pixel 구조체
//--------------------------------------
struct VS_IN
{
    float3 vPosition : POSITION;
    uint InstID : SV_InstanceID; // 인스턴스 ID (트레일 인덱스)
};

struct VS_OUT
{
    uint TrailID : TEXCOORD0; // GS 입력으로 전달
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float vFade : TEXCOORD1;
};

//--------------------------------------
// VS
//--------------------------------------
VS_OUT VS_Main(VS_IN In)
{
    VS_OUT Out;
    Out.TrailID = In.InstID; // DrawInstanced에서 인스턴스ID 전달
    return Out;
}

//--------------------------------------
// GS
//--------------------------------------
[maxvertexcount(64)]
void GS_Trail(point VS_OUT In[1], inout TriangleStream<PS_IN> triStream)
{
    uint trailID = In[0].TrailID;

    uint4 header = g_TrailHeaders[trailID];
    uint startIdx = header.x;
    uint count = header.y;

    if (count < 2)
        return;

    [loop]
    for (uint i = 0; i < count - 1; ++i)
    {
        float4 p0 = g_TrailPoints[startIdx + i];
        float4 p1 = g_TrailPoints[startIdx + i + 1];

        float3 dir = normalize(p1.xyz - p0.xyz);
        float3 camDir = normalize(g_vCamPos - p0.xyz);
        float3 right = normalize(cross(dir, camDir));
        float width = 0.05f;

        float4 v0 = mul(mul(float4(p0.xyz + right * width, 1), g_ViewMatrix), g_ProjMatrix);
        float4 v1 = mul(mul(float4(p0.xyz - right * width, 1), g_ViewMatrix), g_ProjMatrix);
        float4 v2 = mul(mul(float4(p1.xyz + right * width, 1), g_ViewMatrix), g_ProjMatrix);
        float4 v3 = mul(mul(float4(p1.xyz - right * width, 1), g_ViewMatrix), g_ProjMatrix);

        float2 uv0 = float2(0, (float) i / count);
        float2 uv1 = float2(1, (float) i / count);
        float2 uv2 = float2(0, (float) (i + 1) / count);
        float2 uv3 = float2(1, (float) (i + 1) / count);

        float fade0 = 1 - (float) i / count;
        float fade1 = 1 - (float) (i + 1) / count;

        PS_IN o0 = { v0, uv0, fade0 };
        PS_IN o1 = { v1, uv1, fade0 };
        PS_IN o2 = { v2, uv2, fade1 };
        PS_IN o3 = { v3, uv3, fade1 };

        triStream.Append(o0);
        triStream.Append(o1);
        triStream.Append(o2);

        triStream.Append(o2);
        triStream.Append(o1);
        triStream.Append(o3);
    }
}

//--------------------------------------
// PS
//--------------------------------------
float4 PS_Trail(PS_IN In) : SV_TARGET
{
    float3 head = float3(0.2, 1.0, 0.5);
    float3 tail = float3(0.0, 0.4, 0.1);
    float3 color = lerp(tail, head, In.vTexcoord.y);
    return float4(color * In.vFade, In.vFade);
}

//--------------------------------------
// Technique
//--------------------------------------
technique11 TrailRender
{
    pass TrailPass
    {
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_DefaultNoWrite, 0);
        SetBlendState(BS_Additive, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_Trail();
        PixelShader = compile ps_5_0 PS_Trail();
    }
}
