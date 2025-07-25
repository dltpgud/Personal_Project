 #include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture, g_Texture1;

float4 g_vCamPosition;

float2 g_PSize;

float4 g_RgbStart;
float4 g_RgbEnd;

bool g_Hit;
float g_hp, g_hp_pluse;
float threshold;

struct VS_IN  
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
 
};

struct VS_OUT   
{
  	float4 vPosition : POSITION;/*SV는 시스템 값 시멘틱,,*/
  
};


VS_OUT VS_MAIN( /* 내가 그릴려고 했던 정점을 받아오는거다*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition = vPosition;
    
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4];

    vector vLook = g_vCamPosition - In[0].vPosition;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * g_PSize.x;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * g_PSize.y;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = In[0].vPosition + vector(vRight, 0.f) + vector(vUp, 0.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);

    Out[1].vPosition = In[0].vPosition - vector(vRight, 0.f) + vector(vUp, 0.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);

    Out[2].vPosition = In[0].vPosition - vector(vRight, 0.f) - vector(vUp, 0.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);

    Out[3].vPosition = In[0].vPosition + vector(vRight, 0.f) - vector(vUp, 0.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);

    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();

    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
}



struct PS_IN
{
    float4 vPosition : SV_POSITION;  
	float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    
    // 림라이트와 아웃라인이 텍스쳐 불투명도가 없음에도 뚫려서 보이는 증상을 방지하기 위해..시맨틱들 정의..
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
   
    
  vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord).r;
    
   if (vMtrlDiffuse.a == 0.f)  
    discard;

    float3 color = lerp(g_RgbStart.rgb, g_RgbEnd.rgb, vMtrlDiffuse.rgb);
    
   vMtrlDiffuse = float4(color, 1);
    

    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}


PS_OUT PS_PLAYERBULLET(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
 
    if (vMtrlDiffuse.r == 0.f && vMtrlDiffuse.g == 0.f && vMtrlDiffuse.b == 0.f)
        discard;
    
   
    vMtrlDiffuse.a = vMtrlDiffuse.r;
    
      float3 color = lerp(g_RgbStart.rgb, g_RgbEnd.rgb, vMtrlDiffuse.rgb);
       
    Out.vDiffuse = float4(color, vMtrlDiffuse.a);
    
    if (Out.vDiffuse.a <= 0.01f)
        discard;
    
    

    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}


PS_OUT PS_PLAYERBULLETDEAD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float3 color = lerp(g_RgbStart.rgb, g_RgbEnd.rgb, vMtrlDiffuse.rgb);
       
    Out.vDiffuse = float4(color, vMtrlDiffuse.a);
    
    if (Out.vDiffuse.a <= 0.f)
        discard;
    
    

    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT PS_MAIN_HP_Bar_BackGround(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    vector Diffuse0 = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    vector Black = { 1.f, 1.f, 1.f, 0.f };
    
    Diffuse0 -= Black;
    vector WHite = { 0.1f, 0.1f, 0.1f, 0.1f };
    
    if (true == g_Hit)
    {
        Diffuse0 += WHite;
    }
    Out.vDiffuse = Diffuse0;

    if (Out.vDiffuse.a == 0.f)  
        discard;

    
    return Out;
}

PS_OUT PS_MAIN_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector Red = { 0.f, 1.f, 1.f, 0.f };
    vector yellow = { 0.f, 0.f, 1.f, 0.f };
    vector WHite = { 0.1f, 0.1f, 0.1f, 0.1f };
    vector Diffuse0 = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector Diffuse1 = g_Texture1.Sample(LinearSampler, In.vTexcoord);
  
    Diffuse0 -= Red;
    Diffuse1 -= yellow;

    
    if (true == g_Hit)
    {
        Diffuse0 += WHite;
        Diffuse1 += WHite;
    }
    
    if (In.vTexcoord.x < g_hp)
        Out.vDiffuse = Diffuse0;
    else if (In.vTexcoord.x < g_hp_pluse)
        Out.vDiffuse = Diffuse1;

    if (Out.vDiffuse.a == 0.f)   
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_Dissolve(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float maskValue = g_Texture1.Sample(LinearSampler, In.vTexcoord).r;

    if (maskValue < threshold)
    {
        return Out;
    }
    else
    {
        Out.vDiffuse = 0;
        return Out;
    }
}

technique11 DefaultTechnique
{ 
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
 
    pass DefaultPass1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_PLAYERBULLET();
    }


    pass DefaultPass2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_PLAYERBULLETDEAD();
    }

    pass DefaultPass3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_HP_Bar_BackGround();
    }

    pass DefaultPass4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_HP();
    }

    pass DefaultPass5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_Dissolve();
    }
}
