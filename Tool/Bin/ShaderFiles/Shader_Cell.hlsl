 #include "Engine_Shader_Defines.hlsli"
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4			g_vColor = (float4)1.f;

struct VS_IN
{
	float3 vPosition : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	vector			vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
};


struct PS_OUT
{
	vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_vColor;

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
}
