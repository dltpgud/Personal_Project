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


float4x4		g_BoneMatrices[512];   /*뼈 메트릭스 개수*/
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
	/* 이 메시에 영향을 주는 뼈들의 인덱스 */
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
	
	/*애니메이션 뼈가 없는 메쉬들에게 성분을 부여하여 존재하도록 만들기*/
	float			fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);  

	
	/*뼈 매트릭스 설정해 주기*/
	matrix			BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
	
	

	/*뼈 매트릭스와 메쉬의 위치와 법선을 각각 곱해서 뼈들의 위치를 잡아 주고 법선의 위치도 잡아주기. */
	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

	matrix		matWV, matWVP;
	
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	vPosition = mul(vPosition, matWVP);

	Out.vPosition = vPosition;	
    Out.vNormal = mul(vNormal, g_WorldMatrix);/*월드 행렬을 곱해 법선의 위치를 잡아 제대로된 정점들의 조명연산 들어가게 하기.*/
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
	/* 변수에 대한 시멘틱을 정의한다. */
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

    // 카툰 효과를 위한 색상 임계값
    float3 finalColor;
    if (NdotL > 0.2)
    {
        finalColor = vMtrlDiffuse.rgb * float3(1.0, 1.0, 1.0); // 하이라이트 색상
    }
    else
    {
        finalColor = vMtrlDiffuse.rgb * float3(0.9, 0.9, 0.9); // 음영 색상
    }    
	
    Out.vColor = float4(finalColor * g_vLightDiffuse, vMtrlDiffuse.a) * saturate(vShade) +
    (g_vLightSpecular * g_vMtrlSpecular) * fSpecular + (rim * g_RimColor); // 최종 색상 반환
	
	
	
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

    // 조명 계산
    float NdotL = max(0, dot(normalize(In.vNormal), normalize(g_vLightDir) * -1));

    // 카툰 효과를 위한 색상 임계값
    float3 finalColor;
    if (NdotL > 0.2)
    {
        finalColor = vMtrlDiffuse.rgb * float3(1.0, 1.0, 1.0); // 하이라이트 색상
    }
    else
    {
        finalColor = vMtrlDiffuse.rgb * float3(0.9, 0.9, 0.9); // 음영 색상
    }
	
		// 카툰 스타일 스펙큘러 단계 조정

    float threshold1 = 0.7; // 첫 번째 임계값 (어두운 영역)
    float threshold2 = 0.8; // 두 번째 임계값 (밝은 영역)

// 임계값에 따라 스펙큘러 강도를 조정
    float cartoonSpecular = 0.0;
    if (fSpecular > threshold2)
    {
        cartoonSpecular = 1.0; // 밝은 하이라이트
    }
    else if (fSpecular > threshold1)
    {
        cartoonSpecular = 0.8; // 중간 하이라이트
    }


    Out.vColor = float4(finalColor * g_vLightDiffuse, vMtrlDiffuse.a) * saturate(vShade) +
    (g_vLightSpecular * g_vMtrlSpecular) * cartoonSpecular; // 최종 색상 반환
    
    
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
