 #include "Engine_Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4			g_vLightDir;
float4			g_vLightDiffuse;
float4			g_vLightAmbient;
float4			g_vLightSpecular;
texture2D       g_NormalTexture;
float4			g_RimColor;
float			g_RimPow;


texture2D		g_DiffuseTexture;
texture2D       g_EmissiveTexture;

float4			g_vMtrlAmbient	= float4(0.4f, 0.4f, 0.4f, 1.f);
float4			g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4			g_vCamPosition;

float4x4		g_BoneMatrices[512];   /*뼈 메트릭스 개수*/
bool g_TagetBool;
bool g_DoorBool;
bool g_TagetDeadBool;
bool g_bEmissive;
float g_EmissivePower;
float3 g_EmissiveColor;

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
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
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
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix)); /*월드 행렬을 곱해 법선의 위치를 잡아 제대로된 정점들의 조명연산 들어가게 하기.*/
	Out.vTexcoord = In.vTexcoord;	
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = vPosition;
	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};


struct PS_OUT
{
	/* 변수에 대한 시멘틱을 정의한다. */
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vPickDepth : SV_TARGET3;
    vector vRim : SV_TARGET4;
    vector vEmissive : SV_TARGET5;
    vector vOutLine : SV_TARGET6;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);	

	if (vMtrlDiffuse.a <= 0.3f)
		discard;

//	float4		vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient);
//
//	float4		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
//	float4		vLook = In.vWorldPos - g_vCamPosition;
//
//	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
//
//	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade) + 
//		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;    

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 1.f);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
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


    float rim = { 0.f };
    if (true == g_TagetBool)
    {
        rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
        rim = pow(1 - rim, g_RimPow);
    }

   
    //float NdotL = max(0, dot(normalize(In.vNormal), normalize(g_vLightDir) * -1));
    //
    //// 카툰 효과를 위한 색상 임계값
    //float3 finalColor;
    //if (NdotL > 0.2)
    //{
    //    finalColor = vMtrlDiffuse.rgb * float3(1.0, 1.0, 1.0); // 하이라이트 색상
    //}
    //else
    //{
    //    finalColor = vMtrlDiffuse.rgb * float3(0.9, 0.9, 0.9); // 음영 색상
    //}    

    Out.vDiffuse = vMtrlDiffuse ;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 1.f);
    Out.vRim = (rim * g_RimColor);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
    return Out;
}

PS_OUT PS_MAIN_BOSSMONSTER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMtrlEmissive;
    if (true == g_TagetDeadBool)
    {
        vector Black = { 0.3f, 0.3f, 0.3f, 0.1f };
        vMtrlDiffuse -= Black;
    }
	
    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float rim = { 0.f };
    if (true == g_TagetBool)
    {
        rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
        rim     = pow(1 - rim, g_RimPow);
    }

    if (true == g_bEmissive)
    {
        vMtrlEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
        float3 colorStart = float3(0.f, 0.f, 0.f); 
        float3 colorEnd = g_EmissiveColor.rgb;
	
        float3 color = lerp(colorStart, colorEnd, vMtrlEmissive.rgb);
        
        vMtrlEmissive = float4(color, 0.f) * g_EmissivePower;

    }
    else
        vMtrlEmissive = 0.f;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vEmissive = vMtrlEmissive;
	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 1.f);
    Out.vRim = (rim * g_RimColor);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
    return Out;
}

PS_OUT PS_WEAPON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    vector vMtrlEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vDiffuse = vMtrlDiffuse ;
    Out.vEmissive = vMtrlEmissive * g_EmissivePower;
	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 1.f);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
    return Out;
}



PS_OUT PS_Door(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float rim = { 0.f };
    if (true == g_DoorBool)
    {
        rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
        rim = pow(1 - rim, g_RimPow);
    }

    Out.vDiffuse = vMtrlDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 1.f);
    Out.vRim = (rim * g_RimColor);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
    
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
        PixelShader = compile ps_5_0 PS_MAIN_MONSTER();
    }


    pass DefaultPass2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BOSSMONSTER();
    }

    pass DefaultPass3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WEAPON();
    }

    pass DefaultPass4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Door();
    }

}
