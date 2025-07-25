#include "Engine_Shader_Defines.hlsli"
/* 0 1 2 0 2 3 */

/* 전역변수들 : 컨스턴트 테이블 */
/* 같은 파일 내에 존재하는 모든 함수에서 전역변수를 사용할 수 있다. 대입은 불가 */
/* 외부프로젝트에서 쉐이더 전역으로 특정 데이터를 던지고 받기위한 메모리공간의 의미. */

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

VS_OUT VS_MAIN( /* 내가 그릴려고 했던 정점을 받아오는거다*/ VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	

	vector			vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

	/* 진짜 순수하게 곱하기만 수행한다. */
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
	float4 vPosition : SV_POSITION;	//시멘틱을 아웃풋으로 내보냄
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


/* 세개의 결과 정점의 위치벡터의 w값으로 xyzw를 모두 나눈다. 투영스페이스로의 변환. */
/* 위치벡터를 뷰포트로 변환한다. (윈도우좌표로 변환) */
/* 래스터라이즈 (결과로 나온 정점들의 정보를 선형보간하여 픽셀이 생성된다.) */

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float2 vLifeTime : TEXCOORD1;
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

	if (In.vLifeTime.x <= In.vLifeTime.y)
		discard;
	
    float2 frameUVSize = g_frameSize / g_textureSize;
	
    int frameX = g_currentFrame % g_framesPerRow; // 현재 프레임의 X 위치
    int frameY = g_currentFrame / g_framesPerRow; // 현재 프레임의 Y 위치
    float2 frameStartUV = float2(frameX, frameY) * frameUVSize;

    // 최종 UV 좌표 계산
    float2 finalUV = frameStartUV + In.vTexcoord * frameUVSize;

    Out.vColor = g_Texture.Sample(PointSampler, finalUV);;
	if (Out.vColor.a == 0.f)
		discard;
    Out.vColor.rgb *= 0.4f;

	return Out;
}

/* 지정해준 색을 지정한 렌더타겟에 그려준다. */

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
