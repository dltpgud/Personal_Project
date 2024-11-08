 #include "Engine_Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vLightDir;
float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

texture2D g_DiffuseTexture;

//texture2D g_DiffuseTexture[2];
//texture2D g_MaskTexture;
float4 g_vMtrlAmbient = float4(0.4f, 0.4f, 0.4f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;
float g_TimeSum; 
float2 g_ScrollSpeed;


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

VS_OUT VS_MAIN( /* ���� �׸����� �ߴ� ������ �޾ƿ��°Ŵ�*/ VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	
	
	vector			vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);	

	Out.vPosition = vPosition;	
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexcoord = In.vTexcoord;	
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);  /*�ȼ��� ���� ��ǥ ���ϱ�*/
	
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
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * 30.f);
	
    //vector vSourDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    //vector vDestDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);
    //vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
	//
    //vector vMtrlDiffuse = vDestDiffuse * vMask + vSourDiffuse * (1.f - vMask);
	
	/*���� �������� ����ȭ�ϰ� ������ ����ȭ�ϰ�  �̵��� �����ϸ� ������ ������, �ּҴ� 0 �ű⿡ ȯ�汤�� ���ϸ� �ȼ� ���� ��������. �������ͷ� ���� ���Ѵ�.*/
    float4 vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient);

    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));   /*�ݻ簢 ���ϱ�*/
    float4 vLook = In.vWorldPos - g_vCamPosition;  /*ī�޶��� �躤��*/

    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);   /*���ݻ� ���ϱ�,����ŧ���� ����� �ڻ��� ��Ÿ, ��, ����� ���������� �̰� �ִ��� �����ϸ� ��Ȯ�� ����ŧ���� ���� �� ����*/
	
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade) +
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;;
/*���� �ȼ� ���� ��ǻ� ������ �̿��� ȯ�汤�� ���ϰ� ����ŧ������ ���ؼ� ���� �ȼ� ���� ���Ѵ�-*/
	
	return Out;
}


PS_OUT PS_Fire(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 uvOffset = g_TimeSum;
    
    float2 movedTexCoord = In.vTexcoord + uvOffset;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, movedTexCoord).r;
	
    float4 vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient);

    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal)); 
    float4 vLook = In.vWorldPos - g_vCamPosition; 

    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f); 
	
    float3 colorStart = float3(1.f, 0.f, 0.f); // ����
    float3 colorEnd = float3(1.f,1.0, 0.0); // ���
	
    float3 color = lerp(colorStart, colorEnd, vMtrlDiffuse);
    
    
    Out.vColor = (g_vLightDiffuse * float4(color, 1.0)) * saturate(vShade) +
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
/*���� �ȼ� ���� ��ǻ� ������ �̿��� ȯ�汤�� ���ϰ� ����ŧ������ ���ؼ� ���� �ȼ� ���� ���Ѵ�-*/
	
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
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass DefaultPass2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Fire();
    }
}
