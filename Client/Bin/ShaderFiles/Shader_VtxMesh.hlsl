 #include "Engine_Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4			g_vLightDir;
float4			g_vLightDiffuse;
float4			g_vLightAmbient;
float4			g_vLightSpecular;
float g_fCamFar;
texture2D		g_DiffuseTexture;
texture2D       g_EmissiveTexture;

float4			g_vCamPosition;

float4 g_RGB;
float4 g_RGBEnd;
float g_TimeSum;
float4 g_DoorEmissiveColor;

bool g_bDoorEmissive;
texture2D g_maskTexture;


//float Time; // 시간 값
//float RingRadius; // 초기 반지름 R
//float GrowthRate;
//

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
    float4 vProjPos : TEXCOORD2;
    

};

VS_OUT VS_MAIN( /* 내가 그릴려고 했던 정점을 받아오는거다*/ VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	
	
    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);	

	Out.vPosition = vPosition;	
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
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
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = In.vNormal;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 1.f);
    Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
    return Out;
}


PS_OUT PS_NONOUTLINE(PS_IN In)
{
PS_OUT Out = (PS_OUT) 0;
	
vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 1.f);
    Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
    return Out;
}



PS_OUT PS_MAKATOON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 1.f);
   // float4 vShade = max(dot(normalize(g_vLightDir) * -1, normalize(In.vNormal)), 0.0f) + (g_vLightAmbient * g_vMtrlAmbient);
   // float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
   // float4 vLook = In.vWorldPos - g_vCamPosition;
   // float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
   //
   // float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   //
   // // 조명 계산
   // float NdotL = max(0, dot(normalize(In.vNormal), normalize(g_vLightDir)*-1));
   //
   // // 카툰 효과를 위한 색상 임계값
   // float3 finalColor;
   // if (NdotL > 0.2)
   // {
   //     finalColor = vMtrlDiffuse.rgb * float3(1.0, 1.0, 1.0); // 하이라이트 색상
   // }
   // else
   // {
   //     finalColor = vMtrlDiffuse.rgb * float3(0.95, 0.95, 0.95); // 음영 색상
   // }
   //
   // Out.vColor = float4(finalColor * g_vLightDiffuse, vMtrlDiffuse.a) * saturate(vShade);
   //// +(g_vLightSpecular * g_vMtrlSpecular) * fSpecular; // 최종 색상 반환
    Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
    return Out;
}


struct PS_OUT_LIGHTDEPTH
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT PS_MAKATOON_ICON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 1.f);
    
    //float4 vShade = max(dot(normalize(g_vLightDir) * -1, normalize(In.vNormal)), 0.0f) + (g_vLightAmbient * g_vMtrlAmbient);
    //float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    //float4 vLook = In.vWorldPos - g_vCamPosition;
    //float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
	//
    //float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //
    //
    //float4 rimColor = { 0.5f, 0.5f, 0.5f, 0.5f };
    //float rim = { 0.f };
	//
    //    rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
    //    rim = pow(1 - rim, 1000);
    //// 조명 계산
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
    //    finalColor = vMtrlDiffuse.rgb * float3(0.95, 0.95, 0.95); // 음영 색상
    //}
    //
    //Out.vColor = float4(finalColor * g_vLightDiffuse, vMtrlDiffuse.a) * saturate(vShade); //+(rim * rimColor); // 최종 색상 반환
    Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
    return Out;
}

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;
	
    Out.vLightDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

    return Out;
}

PS_OUT PS_FIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 uvOffset = g_TimeSum;
    
   float2 movedTexCoord = In.vTexcoord + uvOffset;
    
    float vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, movedTexCoord).r;
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, movedTexCoord);

 
    float4 color = lerp(g_RGB, g_RGBEnd, vDiffuse);

    float rim{};
    rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
    rim = pow(1 - rim, 1);
 
    vDiffuse.a = vDiffuse.r;
    float4 g_RimColor = { 1.f, 0.f, 0.f, 1.f };

    Out.vRim = vMtrlDiffuse + (rim * g_RimColor);
    Out.vDiffuse = float4(color.rgb, vMtrlDiffuse) + (rim * g_RimColor);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);
    return Out;
}

PS_OUT PS_ShockWaveFire(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float uvOffset = g_TimeSum;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(In.vTexcoord.x + uvOffset, In.vTexcoord.y));

    vMtrlDiffuse.a = vMtrlDiffuse.r;
    
    float3 Red = { 1.f, 0.f, 0.f };
    
    float3 color = lerp(Red, g_RGB.rgb, vMtrlDiffuse.rgb);
       
    Out.vDiffuse = float4(color, vMtrlDiffuse.a);
    
    if (Out.vDiffuse.a <= 0.01f)
        discard;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
        Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 1.f);

    return Out;
}

PS_OUT PS_Shock(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    


    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler,In.vTexcoord);

    vMtrlDiffuse.a = vMtrlDiffuse.r;
    
    float3 Red = { 1.f, 0.f, 0.f };
    
    float3 color = lerp(Red, g_RGB.rgb, vMtrlDiffuse.rgb);
       
    Out.vDiffuse = float4(color, vMtrlDiffuse.a);
    
    if (Out.vDiffuse.a <= 0.01f)
        discard;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 1.f);

    return Out;
}

PS_OUT PS_WALL(PS_IN In)
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
        vDiffuse.rgb = float3(0.1f, 0.1f, 0.1f);
        vEmissive = vMatEmissive * g_DoorEmissiveColor;
    }
    else
    {
        vDiffuse = vMtrlDiffuse;
        vEmissive = vector(0.f, 0.f, 0.f, 0.f);
    }
    


    Out.vDiffuse = vDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vOutLine = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 1.f);
    Out.vEmissive = vEmissive;
    return Out;
}


PS_OUT PS_LASER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
  
    float4 vDiffuse = g_maskTexture.Sample(LinearSampler, float2(In.vTexcoord)*g_TimeSum);

    float4 color = lerp(float4(1.f
    , 1.f, 1.f, 1.f),
    float4(1.f,1.f,0.f, 1.f),  
 
    vDiffuse);

    float rim;
    rim = saturate(dot(normalize(In.vNormal), normalize(g_vCamPosition - In.vWorldPos)));
    rim = pow(1 - rim, 1);
 
    float4(color.rgb, vDiffuse.a) + (rim * float4(1.f, 1.f, 1.f, 1.f));
    
    Out.vRim = float4(color.rgb, vDiffuse.a) + rim;
    Out.vDiffuse = float4(color.rgb, vDiffuse.a);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vEmissive = vDiffuse;
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
        PixelShader = compile ps_5_0 PS_NONOUTLINE();

    }

    pass LightDepth
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

    pass DefaultPass3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FIRE();

    }

    pass DefaultPass4
    {
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_DefaultNoWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ShockWaveFire();
    }
   
   
    pass DefaultPass5
    {
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_DefaultNoWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Shock();
    }


    pass DefaultPass6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WALL();

    }

    pass DefaultPass7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LASER();

    }
}
