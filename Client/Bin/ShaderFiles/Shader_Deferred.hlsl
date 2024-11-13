
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
texture2D g_Texture;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

texture2D g_NormalTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;

texture2D g_ShadeTexture;
texture2D g_DiffuseTexture;

vector g_vMtrlAmbient = { 1.f, 1.f, 1.f, 1.f };
vector g_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

vector g_vCamPosition;

float4 g_vMonsterRimColor, g_vNPCRimColor;
float g_fMonsterRimPow, g_fNPCRimPow;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct PS_OUT
{
    vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};


PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;

	/* 빛 정보와 노말 정보를 이용해서 명암을 계산하여 리턴하낟. */
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 500.f;
	/* 0 ~ 1 -> -1 ~ 1 */
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    float fShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);

    Out.vShade = g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient));

    float4 vWorldPos;

	/* 투영스페이스 상의 완벽한 픽셀의 위치를 구했다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 튜ㅜ영행렬 / w */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    //
    
	/* 뷰스페이스 상의 완벽한 픽셀의 위치를 구했다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 튜ㅜ영행렬 / w */
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스로 이동하자. */
	/* 월드 페이스 상의 완벽한 픽셀의 위치를 구했다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(g_vLightDir), vNormal);

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}



PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;

	/* 빛 정보와 노말 정보를 이용해서 명암을 계산하여 리턴하낟. */
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 500.f;
	/* 0 ~ 1 -> -1 ~ 1 */
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    float4 vWorldPos;

	/* 투영스페이스 상의 완벽한 픽셀의 위치를 구했다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 튜ㅜ영행렬 / w */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 완벽한 픽셀의 위치를 구했다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 튜ㅜ영행렬 / w */
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스로 이동하자. */
	/* 월드 페이스 상의 완벽한 픽셀의 위치를 구했다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


    vector vLightDir = vWorldPos - g_vLightPos;

    float fDistance = length(vLightDir);

    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    float fShade = max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f);

    Out.vShade = (g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))) * fAtt;
	
    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(vLightDir), vNormal);

    float fSpecular = (pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f)) * fAtt;
    
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}
       
PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDiffuse.a == 0.f)
        discard;
    
   
   // float fCurRimDesc = vDepthDesc.z;
   // int iCurID = floor(fCurRimDesc);
   // float fCurState = fCurRimDesc - iCurID;
   // float rim = { 0.f };
   // float4 RimEnd = { 0.f, 0.f, 0.f, 0.f };
   // if (iCurID == 1 && fCurState >= 0.08f)
  

    Out.vColor = vDiffuse * vShade + vSpecular ;

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
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }
}
