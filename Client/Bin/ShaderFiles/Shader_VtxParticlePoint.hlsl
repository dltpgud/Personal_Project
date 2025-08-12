#include "Engine_Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;
vector			g_vCamPosition;

float2 g_textureSize;
float2 g_frameSize;
int g_framesPerRow; 
int g_currentFrame;
struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vPSize : PSIZE;

	row_major float4x4 TransformMatrix : WORLD;

	float2 vLifeTime : TEXCOORD0;
};


struct VS_OUT
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;	
	float2 vLifeTime : TEXCOORD0;
    row_major float4x4 TransformMatrix : WORLD;
};

VS_OUT VS_MAIN(  VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	

	vector			vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

	vPosition = mul(vPosition, g_WorldMatrix);

	Out.vPosition = vPosition;
	Out.vPSize = In.vPSize;	
	Out.vLifeTime = In.vLifeTime;
    Out.TransformMatrix = In.TransformMatrix;
	return Out;
}

struct GS_IN
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
	float2 vLifeTime : TEXCOORD0;
    row_major float4x4 TransformMatrix : WORLD;
};

struct GS_OUT
{
	float4 vPosition : SV_POSITION;	
	float2 vTexcoord : TEXCOORD0;
	float2 vLifeTime : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT			Out[4];

	vector		vLook = g_vCamPosition - In[0].vPosition;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * length(In[0].TransformMatrix._11_11_11)*0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * length(In[0].TransformMatrix._21_22_23)*0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = In[0].vPosition + vector(vRight, 0.f) + vector(vUp, 0.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vLifeTime = In[0].vLifeTime;

	Out[1].vPosition = In[0].vPosition - vector(vRight, 0.f) + vector(vUp, 0.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);
	Out[1].vLifeTime = In[0].vLifeTime;

	Out[2].vPosition = In[0].vPosition - vector(vRight, 0.f) - vector(vUp, 0.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vLifeTime = In[0].vLifeTime;

	Out[3].vPosition = In[0].vPosition + vector(vRight, 0.f) - vector(vUp, 0.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vLifeTime = In[0].vLifeTime;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float2 vLifeTime : TEXCOORD1;
};


struct PS_OUT
{
	
	vector vColor : SV_TARGET0;	

};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (In.vLifeTime.x <= In.vLifeTime.y)
		discard;
	
    float2 frameUVSize = g_frameSize / g_textureSize;
	
    int frameX = g_currentFrame % g_framesPerRow; 
    int frameY = g_currentFrame / g_framesPerRow; 
    float2 frameStartUV = float2(frameX, frameY) * frameUVSize;

    float2 finalUV = frameStartUV + In.vTexcoord * frameUVSize;

    Out.vColor = g_Texture.Sample(PointSampler, finalUV);;
	if (Out.vColor.a == 0.f)
		discard;
    Out.vColor.rgb *= 0.4f;

	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
