 #include "Engine_Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4			g_vLightDir;
float4			g_vLightDiffuse;
float4			g_vLightAmbient;
float4			g_vLightSpecular;

texture2D		g_DiffuseTexture;
texture2D       g_EmissiveTexture;

float4			g_vCamPosition;

float4 g_RGB;
float4 g_RGBEnd;
float g_TimeSum;
float4 g_DoorEmissiveColor;
float g_fCamFar;
bool g_bDoorEmissive;
texture2D g_maskTexture;

struct VS_INST
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vPos : TEXCOORD4;

};

struct VS_OUTINST
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;

    
};

VS_OUTINST VSINST_MAIN(VS_INST In)
{
    VS_OUTINST Out = (VS_OUTINST) 0;
    
    float4x4 WorldMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vPos); 
    
    vector vPosition = mul(float4(In.vPosition, 1.f), WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;

    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), WorldMatrix));
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);
    Out.vTexcoord = In.vTexcoord;
        
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), WorldMatrix);
    Out.vProjPos = vPosition;
    
    return Out;
}

struct PS_OUT_LIGHTDEPTH
{
    float vLightDepth : SV_TARGET0;
};

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
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;

    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.01f, 0.f);
    Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
    Out.vAmbient = vector(1.5f, 1.5f, 1.5f, 1.5f);
    return Out;
}


PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;
    Out.vLightDepth = (In.vProjPos.z / In.vProjPos.w);

    return Out;
}


PS_OUT PS_NONOUTLINE(PS_IN In)
{
PS_OUT Out = (PS_OUT) 0;
	
vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;

    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.01f, 0.f);
    Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
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
   
        VertexShader = compile vs_5_0 VSINST_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN(); 

    }
   
    pass DefaultPass1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VSINST_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NONOUTLINE(); 

    }

    pass LightDepth
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VSINST_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

    
}
