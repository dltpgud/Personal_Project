 #include "Engine_Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4			g_vLightDir;
float4			g_vLightDiffuse;
float4			g_vLightAmbient;
float4			g_vLightSpecular;
float4			g_RimColor;
float			g_RimPow;

texture2D		g_DiffuseTexture;
float4			g_vMtrlAmbient	= float4(0.4f, 0.4f, 0.4f, 1.f);
float4			g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4			g_vCamPosition;


float4x4		g_BoneMatrices[512];   /*�� ��Ʈ���� ����*/
bool g_TagetBool;
bool g_TagetDeadBool;
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
	/* �� �޽ÿ� ������ �ִ� ������ �ε��� */
	uint4  vBlendIndex : BLENDINDEX;
	float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;	
	float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(  VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;
	
	/*�ִϸ��̼� ���� ���� �޽��鿡�� ������ �ο��Ͽ� �����ϵ��� �����*/
	float			fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);  

	
	/*�� ��Ʈ���� ������ �ֱ�*/
	matrix			BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
	
	

	/*�� ��Ʈ������ �޽��� ��ġ�� ������ ���� ���ؼ� ������ ��ġ�� ��� �ְ� ������ ��ġ�� ����ֱ�. */
	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

	matrix		matWV, matWVP;
	
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	vPosition = mul(vPosition, matWVP);

	Out.vPosition = vPosition;	
    Out.vNormal = mul(vNormal, g_WorldMatrix);/*���� ����� ���� ������ ��ġ�� ��� ����ε� �������� ������ ���� �ϱ�.*/
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


	if (vMtrlDiffuse.a <= 0.3f)
		discard;


	float4		vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient);

	float4		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	float4		vLook = In.vWorldPos - g_vCamPosition;

	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade) + 
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	return Out;
}

PS_OUT PS_MAIN_MONSTER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (true == g_TagetDeadBool)
    {
        vector Black = { 0.3f, 0.3f, 0.3f, 0.1f };
        vMtrlDiffuse -= Black;
    }
	
    if (vMtrlDiffuse.a <= 0.3f)
        discard;


    float4 vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient);

    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    float4 vLook = In.vWorldPos - g_vCamPosition;

    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);

	
    float rim = { 0.f };
	
	if (true == g_TagetBool)
    {

        rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
        rim = pow(1 - rim, g_RimPow);
    }
	
  //   Out.vColor = (g_vLightDiffuse  * vMtrlDiffuse) * saturate(vShade) +
	//	          (g_vLightSpecular * g_vMtrlSpecular) * fSpecular + (rim * rimColor);
    
	
	
    float NdotL = max(0, dot(normalize(In.vNormal), normalize(g_vLightDir) * -1));

    // ī�� ȿ���� ���� ���� �Ӱ谪
    float3 finalColor;
    if (NdotL > 0.2)
    {
        finalColor = vMtrlDiffuse.rgb * float3(1.0, 1.0, 1.0); // ���̶���Ʈ ����
    }
    else
    {
        finalColor = vMtrlDiffuse.rgb * float3(0.9, 0.9, 0.9); // ���� ����
    }    
	
    Out.vColor = float4(finalColor * g_vLightDiffuse, vMtrlDiffuse.a) * saturate(vShade) +
    (g_vLightSpecular * g_vMtrlSpecular) * fSpecular + (rim * g_RimColor); // ���� ���� ��ȯ
	
	
	
    return Out;
}



PS_OUT PS_MA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	

    float4 vShade = max(dot(normalize(g_vLightDir) * -1, normalize(In.vNormal)), 0.0f) + (g_vLightAmbient * g_vMtrlAmbient);
    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    float4 vLook = In.vWorldPos - g_vCamPosition;
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
	
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

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
        finalColor = vMtrlDiffuse.rgb * float3(0.9, 0.9, 0.9); // ���� ����
    }
	
		// ī�� ��Ÿ�� ����ŧ�� �ܰ� ����

    float threshold1 = 0.7; // ù ��° �Ӱ谪 (��ο� ����)
    float threshold2 = 0.8; // �� ��° �Ӱ谪 (���� ����)

// �Ӱ谪�� ���� ����ŧ�� ������ ����
    float cartoonSpecular = 0.0;
    if (fSpecular > threshold2)
    {
        cartoonSpecular = 1.0; // ���� ���̶���Ʈ
    }
    else if (fSpecular > threshold1)
    {
        cartoonSpecular = 0.8; // �߰� ���̶���Ʈ
    }


    Out.vColor = float4(finalColor * g_vLightDiffuse, vMtrlDiffuse.a) * saturate(vShade) +
    (g_vLightSpecular * g_vMtrlSpecular) * cartoonSpecular; // ���� ���� ��ȯ
    
    
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
		PixelShader = compile ps_5_0 PS_MA();
	}

    pass DefaultPass1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_MONSTER();
    }
}
