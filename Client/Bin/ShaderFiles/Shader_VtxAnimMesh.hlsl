 #include "Engine_Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4			g_vLightDir;
float4			g_vLightDiffuse;
float4			g_vLightAmbient;
float4			g_vLightSpecular;
texture2D       g_NormalTexture;
float4			g_RimColor;
float			g_RimPow =1.f;


texture2D		g_DiffuseTexture;
texture2D       g_EmissiveTexture;
texture2D       g_DiffTexture;
float4			g_vMtrlAmbient	= float4(0.4f, 0.4f, 0.4f, 1.f);
float4			g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4			g_vCamPosition;
float           g_fCamFar;

float4x4		g_BoneMatrices[512];   

bool g_DoorBool;
bool g_TagetDeadBool;
bool g_bEmissive;
bool g_bNomal;
float g_EmissivePower;
float3 g_EmissiveColor;
bool g_bDoorEmissive;
float4 g_DoorEmissiveColor;
float4 g_DoorRimColor;

Texture2D g_maskTexture;
float g_threshold;


struct VS_IN
{
	float3 vPosition : POSITION;	
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;	
	float3 vTangent : TANGENT;
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
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * fWeightW;


    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    vPosition = mul(vPosition, matWVP);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);

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
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vRim : SV_TARGET3;
    vector vEmissive : SV_TARGET4;
    vector vOutLine : SV_TARGET5;
    vector vAmbient : SV_TARGET6;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);	

	if (vMtrlDiffuse.a <= 0.3f)
		discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vRim = 0.f;
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.02f, 0.f);
    Out.vAmbient = vector(2.f, 2.f, 2.f, 2.f);
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

    float maskValue = g_maskTexture.Sample(LinearSampler, In.vTexcoord).r;
    if (maskValue > g_threshold)   
    { 
        Out.vDiffuse = vMtrlDiffuse;
    }
    else
    {
        discard;
    }

    float rim = { 0.f };
    rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
    rim = pow(1 - rim, g_RimPow);
     
   vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
   vector vNormal = vector(vNormalDesc.xy * 2.f - 1.f, vNormalDesc.z, 0.f);
   vNormal.z = sqrt(1 - vNormal.x * vNormal.x - vNormal.y * vNormal.y);
   float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
   vNormal = vector(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    Out.vRim = rim * g_RimColor;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vAmbient = vector(1.5f, 1.5f, 1.5f, 1.5f);
    Out.vDiffuse = vMtrlDiffuse ;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 1.f, 0.f);;
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.01f, 0.f);
    return Out;
}

PS_OUT PS_MAIN_BOSSMONSTER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMtrlEmissive;
	
    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float rim = { 0.f };
    rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
    rim = pow(1 - rim, g_RimPow);
    

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
    
    Out.vEmissive =  vMtrlEmissive;
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 1.f, 0.f);
    Out.vRim = (rim * g_RimColor);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.01f, 0.f);
    Out.vAmbient = vector(2.f, 2.f, 2.f, 2.f);
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
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.019f, 0.f);
    Out.vRim = 0.f;
    Out.vAmbient = vector(2.f, 2.f, 2.f, 2.f);
    return Out;
}



PS_OUT PS_Door(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMatEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    vector vEmissive;
    vector vDiffuse;
    if (true == g_bDoorEmissive)
    {
        vDiffuse = float4(0.1f, 0.1f, 0.1f, 1.f);
        vEmissive = vMatEmissive * g_DoorEmissiveColor;
    }
    else
    {
        vDiffuse = vMtrlDiffuse;
        vEmissive = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    float rim = { 0.f };
    rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
    rim = pow(1 - rim, g_RimPow);

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vRim = (rim * g_DoorRimColor);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.01f, 0.f);
    Out.vEmissive = vEmissive;
    Out.vAmbient = vector(1.5f, 1.5f, 1.5f, 1.5f);
    return Out;
}

struct PS_OUT_LIGHTDEPTH
{
    float vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;
    Out.vLightDepth = (In.vProjPos.z / In.vProjPos.w);

    return Out;
}

PS_OUT PS_MAIN_NPC(PS_IN In)
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
    rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
    rim = pow(1 - rim, g_RimPow);
    
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 1.f, 0.f);
    Out.vRim = (rim * g_RimColor);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.01f, 0.f);
    Out.vAmbient = vector(2.f, 2.f, 2.f, 2.f);
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
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

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
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WEAPON();
    }

    pass DefaultPass4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Door();
    }


    pass DefaultPass5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

    pass DefaultPass6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NPC();
    }




}
