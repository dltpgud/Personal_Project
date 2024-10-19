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
/* 1. 정점의 기본 변환(로컬상에 정의된 정점을 월드, 뷰, 투영) 을 수행한다. */
/* 2. 정점의 구성 정보를 인위적으로 수정 변경이 가능하다. */
// VS_OUT VS_MAIN(float3 vPosition : POSITION, float2 vTexcoord : TEXCOORD0)
VS_OUT VS_MAIN( /* 내가 그릴려고 했던 정점을 받아오는거다*/ VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;

	/* float3 * float4x4 좌변의 w를 1로 채워서 곱하기연산을 수행한다. */
	/* w로 xyzw를 모두 나눈다. */
	/*D3DXVec3TransformCoord();*/

	/* 진짜 순수하게 곱하기만 수행한다. */
	vector			vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);	

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = vPosition;

	return Out;
}


/* 세개의 결과 정점의 위치벡터의 w값으로 xyzw를 모두 나눈다. 투영스페이스로의 변환. */
/* 위치벡터를 뷰포트로 변환한다. (윈도우좌표로 변환) */
/* 래스터라이즈 (결과로 나온 정점들의 정보를 선형보간하여 픽셀이 생성된다.) */

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float3 vTexcoord : TEXCOORD0;
	float4 vProjPos :  TEXCOORD1;
};


struct PS_OUT
{
	/* 변수에 대한 시멘틱을 정의한다. */
	vector vColor : SV_TARGET0;	

};

/* 픽셀 쉐이더 */
/* 픽셀의 최종적인 렌더링 색상을 결정하낟. */
/* 픽셀당 픽셀쉐이더를 하나씩 모두 수행한다. */
// vector PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/* g_Texture라는 텍스에서부터 지정한 텍스쿠드에 위치한 픽셀의 색을 LinearSampler방식을 얻어온다. */
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);


	return Out;
}


/* 지정해준 색을 지정한 렌더타겟에 그려준다. */

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
