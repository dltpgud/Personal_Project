#include "Engine_Shader_Defines.hlsli"
/* 0 1 2 0 2 3 */

/* ���������� : ������Ʈ ���̺� */
/* ���� ���� ���� �����ϴ� ��� �Լ����� ���������� ����� �� �ִ�. ������ �Ұ� */
/* �ܺ�������Ʈ���� ���̴� �������� Ư�� �����͸� ������ �ޱ����� �޸𸮰����� �ǹ�. */

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;


struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;

	float4 vRight : TEXCOORD1;
	float4 vUp : TEXCOORD2;
	float4 vLook : TEXCOORD3;
	float4 vTranslation : TEXCOORD4;

	float2 vLifeTime : TEXCOORD5;
};


struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;	
	float2 vLifeTime : TEXCOORD1;
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

	float4x4		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector			vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	/* ��¥ �����ϰ� ���ϱ⸸ �����Ѵ�. */
	vPosition = mul(vPosition, g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);	

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;	
	Out.vLifeTime = In.vLifeTime;

	return Out;
}


/* ������ ��� ������ ��ġ������ w������ xyzw�� ��� ������. ���������̽����� ��ȯ. */
/* ��ġ���͸� ����Ʈ�� ��ȯ�Ѵ�. (��������ǥ�� ��ȯ) */
/* �����Ͷ����� (����� ���� �������� ������ ���������Ͽ� �ȼ��� �����ȴ�.) */

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;	
	float2 vLifeTime : TEXCOORD1;
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

	if (In.vLifeTime.x <= In.vLifeTime.y)
		discard;

	/* g_Texture��� �ؽ��������� ������ �ؽ���忡 ��ġ�� �ȼ��� ���� LinearSampler����� ���´�. */
	Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);	

	if (Out.vColor.a == 0.f)
		discard;


	Out.vColor.a = saturate(In.vLifeTime.x - In.vLifeTime.y) * 2.f;
	Out.vColor = vector(1.f, Out.vColor.a, 0.f, 1.f);

	return Out;
}

/* �������� ���� ������ ����Ÿ�ٿ� �׷��ش�. */

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
