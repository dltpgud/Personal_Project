
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
texture2D g_Texture;
texture2D g_OutLineTexture;

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
texture2D g_LightDepthTexture;

texture2D g_EmissiveTexture;
texture2D g_RimTexture;

texture2D g_FinalTexture;
texture2D g_BlurTexture;
texture2D g_BrightnessTexTure;

vector g_vMtrlAmbient = { 1.f, 1.f, 1.f, 1.f };
vector g_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

vector g_vCamPosition;


// 텍스쳐에서 한 픽셀의 간격
float dX;
float dY;

 float Bloom_Weights[5] = { 0.0545, 0.2442, 0.4026, 0.2442 , 0.0545 };


float4 Compute_WorldPos(float2 vTexcoord)
{
    float4 vWorldPos = 0.f;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    float fViewZ = vDepthDesc.y * 500.f;
	
    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 완벽한 픽셀의 위치를 구했다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 튜ㅜ영행렬 / w */
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스로 이동하자. */
	/* 월드 페이스 상의 완벽한 픽셀의 위치를 구했다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    return vWorldPos;
}


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
    vector vRim = g_RimTexture.Sample(LinearSampler, In.vTexcoord);
    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord); 
    
    float depthLeft = g_OutLineTexture.Sample(LinearSampler, In.vTexcoord + float2(-(1.f / 1280.f), 0.f)).y * 500.f;
    float depthRight = g_OutLineTexture.Sample(LinearSampler, In.vTexcoord + float2((1.f / 1280.f), 0.f)).y * 500.f;
    float depthUp = g_OutLineTexture.Sample(LinearSampler, In.vTexcoord + float2(0.f, -(1.f / 720.f))).y * 500.f;
    float depthDown = g_OutLineTexture.Sample(LinearSampler, In.vTexcoord + float2(0.f, (1.f / 720.f))).y * 500.f;
    float depthMid = g_OutLineTexture.Sample(LinearSampler, In.vTexcoord).y;
    float depthON_OFF = g_OutLineTexture.Sample(LinearSampler, In.vTexcoord).z;
    float OutLine;
    if (depthMid * 15.f < abs(depthLeft - depthRight))
    {
        OutLine = (0.f);
    }
    else if (depthMid * 15.f < abs(depthUp - depthDown))
    {
        OutLine = (0.f);
    }
    else if (depthON_OFF >= 0.5f)
    {
        OutLine = (1.f);
    }
    else
        OutLine = 1.f;
        
    
    
    
    if (vDiffuse.a == 0.f)
        discard;
    
     vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
     float fCurIsSpecular = vDepthDesc.z;
     
     if (fCurIsSpecular == 0.f)
         vSpecular = 0.f;
   
    
   vector vPosition = Compute_WorldPos(In.vTexcoord);
   
   vPosition = mul(vPosition, g_LightViewMatrix);
   vPosition = mul(vPosition, g_LightProjMatrix);
   
   
   float2 vTexcoord = 0.f;
   
   vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
   vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
   
   vector vOldDepth = g_LightDepthTexture.Sample(LinearSampler, vTexcoord);
 
    Out.vColor = vDiffuse * vShade * OutLine + vRim + (vSpecular * 0.8f) + vEmissive;
            
    
    if (vPosition.w - 1.5f > vOldDepth.y * 500.f)
    {
        Out.vColor.rgb *= 0.9f;
   	
    }
    

    
        return Out;
    }



PS_OUT PS_MAIN_PURE(PS_IN In)
{ // (다운 샘플링 용도로 사용)
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexcoord);
    
    Out.vColor = vDiffuse;
    
    return Out;
}

// 블러 X
PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    
    int i;
    for (i = 0; i < 5; i++)
    {
        vDiffuse += Bloom_Weights[i] * g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexcoord + float2(dX, 0.0) * float(i - 2));
    }
    
    Out.vColor = vDiffuse;

    return Out;
}

// 블러 Y
PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    
    int i;
    for (i = 0; i < 5; i++)
    {
        vDiffuse += Bloom_Weights[i] * g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexcoord + float2(0.0, dY) * float(i - 2));
    }
    
    Out.vColor = vDiffuse;

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

    pass Pure
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PURE();
    }
   
    pass BLUR_X 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass BLUR_Y 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

}
