#include "Engine_Shader_Defines.hlsli"


matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE		g_Texture;


sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
};

sampler PointSampler = sampler_state
{
	filter = MIN_MAG_MIP_POINT;
};

struct VS_IN
{
	float3 vPosition : POSITION;	
	float3 vTexcoord : TEXCOORD0;	
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float3 vTexcoord : TEXCOORD0;
	float4 vProjPos :  TEXCOORD1;
};

/* Vertex Shader Entry Function */
/* 1. ������ �⺻ ��ȯ(���û� ���ǵ� ������ ����, ��, ����) �� �����Ѵ�. */
/* 2. ������ ���� ������ ���������� ���� ������ �����ϴ�. */
// VS_OUT VS_MAIN(float3 vPosition : POSITION, float2 vTexcoord : TEXCOORD0)
VS_OUT VS_MAIN( /* ���� �׸����� �ߴ� ������ �޾ƿ��°Ŵ�*/ VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;

	/* float3 * float4x4 �º��� w�� 1�� ä���� ���ϱ⿬���� �����Ѵ�. */
	/* w�� xyzw�� ��� ������. */
	/*D3DXVec3TransformCoord();*/

	/* ��¥ �����ϰ� ���ϱ⸸ �����Ѵ�. */
	vector			vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);	

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = vPosition;

	return Out;
}


/* ������ ��� ������ ��ġ������ w������ xyzw�� ��� ������. ���������̽����� ��ȯ. */
/* ��ġ���͸� ����Ʈ�� ��ȯ�Ѵ�. (��������ǥ�� ��ȯ) */
/* �����Ͷ����� (����� ���� �������� ������ ���������Ͽ� �ȼ��� �����ȴ�.) */

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float3 vTexcoord : TEXCOORD0;
	float4 vProjPos :  TEXCOORD1;
};


struct PS_OUT
{
	/* ������ ���� �ø�ƽ�� �����Ѵ�. */
	vector vColor : SV_TARGET0;	

};

/* �ȼ� ���̴� */
/* �ȼ��� �������� ������ ������ �����ϳ�. */
/* �ȼ��� �ȼ����̴��� �ϳ��� ��� �����Ѵ�. */
// vector PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* g_Texture��� �ؽ��������� ������ �ؽ���忡 ��ġ�� �ȼ��� ���� LinearSampler����� ���´�. */
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);


	return Out;
}


/* �������� ���� ������ ����Ÿ�ٿ� �׷��ش�. */

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Sky);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
