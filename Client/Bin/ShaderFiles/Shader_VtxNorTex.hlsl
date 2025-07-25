     #include "Engine_Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;


texture2D g_DiffuseTexture;

//texture2D g_DiffuseTexture[2];
//texture2D g_MaskTexture;
float4 g_vMtrlAmbient = float4(0.4f, 0.4f, 0.4f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;
float g_TimeSum; 
float g_fCamFar;
int g_onEmissive;

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
    float4 vProjPos : TEXCOORD2;
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
    Out.vProjPos = Out.vPosition;
    
	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};


struct PS_OUT
{
	/* 변수에 대한 시멘틱을 정의한다. */
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vPickDepth : SV_TARGET3;
    vector vEmissive : SV_TARGET5;
    vector vOutLine : SV_TARGET6;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * 30.f);
	
 
    Out.vDiffuse = vector(vMtrlDiffuse.rgb, 1.f);

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	/* 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

    Out.vOutLine = vector(0.f, 0.f, 1.f, 0.f);
    
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 1.f);
/*최종 픽셀 색은 디퓨즈에 법선을 이용한 환경광을 곱하고 스펙큘러들을 곱해서 최종 픽셀 색을 정한다-*/
	
	return Out;
}


PS_OUT PS_Fire(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 uvOffset = g_TimeSum;
    
    float2 movedTexCoord = In.vTexcoord + uvOffset;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, movedTexCoord);
 
	
    float3 colorStart = float3(1.f, 0.f, 0.f); // 빨강
    float3 colorEnd = float3(1.f,0.80, 0.0); // 노랑
	
    float3 color = lerp(colorStart, colorEnd, vMtrlDiffuse.rgb);

    vMtrlDiffuse = float4(color, 1);
    
    
    Out.vDiffuse = vector(vMtrlDiffuse.rgb, 0.8f);

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	/* 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 1.f);
    return Out;
}

struct PS_OUT_LIGHTDEPTH
{
    vector vLightDepth : SV_TARGET0;
};



PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;
	
    Out.vLightDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

    return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass DefaultPass1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Fire();
    }


    pass DefaultPass2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }
}
