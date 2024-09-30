
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vLightDir;
float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

texture2D g_DiffuseTexture;
float4 g_vMtrlAmbient = float4(0.4f, 0.4f, 0.4f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;

sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler PointSampler = sampler_state
{
	filter = MIN_MAG_MIP_POINT;
};

struct VS_IN
{
	float3 vPosition : POSITION;	
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;	
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN( /* 내가 그릴려고 했던 정점을 받아오는거다*/ VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	
	
	vector			vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);	

	Out.vPosition = vPosition;	
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexcoord = In.vTexcoord;	
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);  /*픽셀의 월드 좌표 구하기*/
	
	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};


struct PS_OUT
{
	/* 변수에 대한 시멘틱을 정의한다. */
	vector vColor : SV_TARGET0;	

};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * 30.f);
	
	
	/*빛의 역방향을 정규화하고 법선을 정규화하고  이둘을 내적하면 범위가 정해짐, 최소는 0 거기에 환경광을 더하면 픽셀 색이 정해진다. 법선벡터로 색을 정한다.*/
    float4 vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient);

    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));   /*반사각 구하기*/
    float4 vLook = In.vWorldPos - g_vCamPosition;  /*카메라의 룩벡터*/

    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);   /*정반사 구하기,스펙큘러의 세기는 코사인 세타, 즉, 곡선으로 떨어짐으로 이걸 최대한 제곱하면 정확한 스펙큘러를 구할 수 있음*/
	
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade) +
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;;
/*최종 픽셀 색은 디퓨즈에 법선을 이용한 환경광을 곱하고 스펙큘러들을 곱해서 최종 픽셀 색을 정한다-*/
	
	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
