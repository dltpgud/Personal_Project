 #include "Engine_Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4			g_vLightDir;
float4			g_vLightDiffuse;
float4			g_vLightAmbient;
float4			g_vLightSpecular;

texture2D		g_DiffuseTexture;
float4			g_vMtrlAmbient = float4(0.6f, 0.6f, 0.6f, 1.f);
float4			g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4			g_vCamPosition;


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
	float3 vTangent : TANGENT;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;	
	float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN( /* ���� �׸����� �ߴ� ������ �޾ƿ��°Ŵ�*/ VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	
	
    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);	

	Out.vPosition = vPosition;	
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexcoord = In.vTexcoord;	
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

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
	/* ������ ���� �ø�ƽ�� �����Ѵ�. */
	vector vColor : SV_TARGET0;	

};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	float4		vShade = max(dot(normalize(g_vLightDir) * -1, normalize(In.vNormal)), 0.0f) + (g_vLightAmbient * g_vMtrlAmbient);

	float4		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	float4		vLook = In.vWorldPos - g_vCamPosition;

	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade) + 
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	return Out;
}


PS_OUT PS_MAKATOON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	

    float4 vShade = max(dot(normalize(g_vLightDir) * -1, normalize(In.vNormal)), 0.0f) + (g_vLightAmbient * g_vMtrlAmbient);
    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    float4 vLook = In.vWorldPos - g_vCamPosition;
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
	
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    // ���� ���
    float NdotL = max(0, dot(normalize(In.vNormal), normalize(g_vLightDir)*-1));

    // ī�� ȿ���� ���� ���� �Ӱ谪
    float3 finalColor;
    if (NdotL > 0.2)
    {
        finalColor = vMtrlDiffuse.rgb * float3(1.0, 1.0, 1.0); // ���̶���Ʈ ����
    }
    else
    {
        finalColor = vMtrlDiffuse.rgb * float3(0.95, 0.95, 0.95); // ���� ����
    }

    Out.vColor = float4(finalColor * g_vLightDiffuse, vMtrlDiffuse.a) * saturate(vShade);
   // +(g_vLightSpecular * g_vMtrlSpecular) * fSpecular; // ���� ���� ��ȯ
    
    return Out;
}


PS_OUT PS_MAKATOON_ICON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	

    float4 vShade = max(dot(normalize(g_vLightDir) * -1, normalize(In.vNormal)), 0.0f) + (g_vLightAmbient * g_vMtrlAmbient);
    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    float4 vLook = In.vWorldPos - g_vCamPosition;
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
	
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    
    float4 rimColor = { 0.5f, 0.5f, 0.5f, 0.5f };
    float rim = { 0.f };
	
        rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
        rim = pow(1 - rim, 1000);
    // ���� ���
    float NdotL = max(0, dot(normalize(In.vNormal), normalize(g_vLightDir) * -1));

    // ī�� ȿ���� ���� ���� �Ӱ谪
    float3 finalColor;
    if (NdotL > 0.2)
    {
        finalColor = vMtrlDiffuse.rgb * float3(1.0, 1.0, 1.0); // ���̶���Ʈ ����
    }
    else
    {
        finalColor = vMtrlDiffuse.rgb * float3(0.95, 0.95, 0.95); // ���� ����
    }

    Out.vColor = float4(finalColor * g_vLightDiffuse, vMtrlDiffuse.a) * saturate(vShade); //+(rim * rimColor); // ���� ���� ��ȯ
    
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
        PixelShader = compile ps_5_0 PS_MAKATOON();
    }
   
    pass DefaultPass1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAKATOON_ICON();
    }
//
   // pass DefaultPass1
   // {
   //     SetRasterizerState(RS_Default);
   //     SetDepthStencilState(DSS_Default, 0);
   //     SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
//
   //     VertexShader = compile vs_5_0 VS_MAIN();
   //     PixelShader = compile ps_5_0 PS_MAIN();
   // }

}
