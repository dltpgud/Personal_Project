 #include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D maskTexture;
float threshold;
float g_hp, g_hp_pluse;
texture2D g_Texture, g_Texture0, g_Texture1, g_Texture2;
bool g_Hit;


struct VS_IN_SAMPLE
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct VS_IN 
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = float2(1.0 - In.vTexcoord.x, In.vTexcoord.y);
    Out.vProjPos = vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION; 
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos :  TEXCOORD1;   
};


struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_HP_Bar_BackGround(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    vector Diffuse0 = g_Texture0.Sample(LinearSampler, In.vTexcoord);
    
    vector Black = { 1.f, 1.f, 1.f, 0.f };
    
    Diffuse0 -= Black;
    vector WHite = { 0.1f, 0.1f, 0.1f, 0.1f };
    
    if (true == g_Hit)
    {
        Diffuse0 += WHite;
    }
    Out.vColor = Diffuse0;

    if (Out.vColor.a == 0.f)  
        discard;

    
    return Out;
}

PS_OUT PS_MAIN_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector Red    = { 0.f, 1.f, 1.f, 0.f };
    vector yellow = { 0.f, 0.f, 1.f, 0.f };
    vector WHite = { 0.1f, 0.1f, 0.1f, 0.1f };
    vector Diffuse0 = g_Texture0.Sample(LinearSampler, In.vTexcoord);
    vector Diffuse1 = g_Texture1.Sample(LinearSampler, In.vTexcoord);
  
    Diffuse0 -= Red;
    Diffuse1 -= yellow;

    
    if(true == g_Hit)
    {
        Diffuse0 += WHite;
        Diffuse1 += WHite;
    }
    
    if (In.vTexcoord.x < g_hp )
        Out.vColor = Diffuse0;
    
   else if (In.vTexcoord.x < g_hp_pluse)
        Out.vColor = Diffuse1;

 
    
    
    if (Out.vColor.a == 0.f)   
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_Dissolve(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float maskValue = maskTexture.Sample(LinearSampler, In.vTexcoord).r;

    if (maskValue < threshold)
    {
        return Out;
    }
    else
    {
        Out.vColor = 0;
        return Out;

    }

}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Clockwise);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HP_Bar_BackGround();
    }

    pass DefaultPass1
    {
        SetRasterizerState(RS_Clockwise);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f,0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HP();
	}

    pass DefaultPass2
    {
        SetRasterizerState(RS_Clockwise);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Dissolve();
    }

}
