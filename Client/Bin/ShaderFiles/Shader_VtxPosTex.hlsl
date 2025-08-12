 #include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_hp, g_hp_pluse;
texture2D g_Texture, g_Texture0, g_Texture1, g_Texture2;


int g_Discard;
float4 g_Alpha;
float4 g_RGB;
float4 g_RGBEnd;
float g_TimeSum;

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
	float4 vProjPos :  TEXCOORD1; 
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    

     Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a == 0.f) 
        discard;
  
    return Out;
}

PS_OUT PS_MAIN_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.vTexcoord.x < g_hp )
        Out.vColor = g_Texture1.Sample(LinearSampler, In.vTexcoord);
    else if (In.vTexcoord.x < g_hp_pluse)
        Out.vColor = g_Texture2.Sample(LinearSampler, In.vTexcoord);
    else 
        Out.vColor = g_Texture0.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a == 0.f)   
        discard;
    
    return Out;
}


PS_OUT PS_MAIN_BOSSHP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 Black = { 0.f, 0.f, 1.f, 0.f };
    float3 Blue = { 0.f, 0.f, 1.f };
    float3 Majenta = { 1.f, 0.f, 1.f };
    float3 Yellow = { 1.f, 1.f, 0.f };
    float4 Diffuse0 = g_Texture.Sample(LinearSampler, In.vTexcoord);
  
    if (all(Diffuse0.xyz == Blue))
       Diffuse0 -= Black;

   else if (Diffuse0.x > 0.f && Diffuse0.z != 0.f)
        Diffuse0 -= float4(1.f, 1.f, 1.f, 0.f);
   else if (Diffuse0.x >= 0.f && Diffuse0.z == 0.f)
        Diffuse0 += float4(1.f, 1.f, 1.f, 0.f);

    if (In.vTexcoord.x < g_hp && all(Diffuse0.xyz == float3(0.f, 0.f, 0.f)))
        Diffuse0 += float4(1.f, 0.f, 0.f, 1.f);
    else if (In.vTexcoord.x < g_hp_pluse && all(Diffuse0.xyz == float3(0.f, 0.f, 0.f)))
        Diffuse0 += float4(1.f, 1.f, 1.f, 1.f);
    
    Out.vColor = Diffuse0;      

    if (Out.vColor.a == 0.f)   
            discard;
    
    return Out;
}

PS_OUT PS_MAIN_PLAYERSPRINT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 

   float4 Image = g_Texture.Sample(LinearSampler, In.vTexcoord);

    
  if(g_Discard == 0 )
  {
      if (Image.r != 1.f)
      {
          if (Image.g >=0.f) 
              discard;
          if (Image.b >=0.f) 
              discard;
      }
      
      if (Image.a != 0.f)
            Image.a = g_Alpha.x;
    }
 
     if (g_Discard == 1)
   {
       if (Image.g != 1.f)
       {
           if (Image.r>=0.f) 
               discard;
           if (Image.b>=0.f) 
               discard;
       }
       if (Image.a != 0.f)
            Image.a =  g_Alpha.y;
    }
   
     if (g_Discard == 2)
     {
         if (Image.b != 1.f)
         {
             if (Image.r >=0.f) 
                 discard;
             if (Image.g>=0.f) 
                 discard;
         }
         if (Image.a != 0.f)
            Image.a = g_Alpha.z;
    }
         
    
    Image.r = 0.7f;
    Image.g = 0.7f;
    Image.b = 0.7f;
    Out.vColor = Image;

     if (Out.vColor.a == 0.f)   
            discard;
    
    return Out;
}

PS_OUT PS_MAIN_PLAYERHEALTH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
  
    float4 Texture = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    
    if (Texture.r >= 0.f)
    {
    
        Texture.r = 0.f;
        Texture.g = 1.f;
        Texture.b = 0.f;
    }

    Out.vColor = Texture;

    if (Out.vColor.a == 0.f) 
        discard;
   
    return Out;
}


PS_OUT PS_MAIN_Prrr(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    
    float4 Texture = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Texture.r == 0.f && Texture.g == 0.f&& Texture.b == 0.f)
        discard;
        
    if (Texture.r >= 0.4f)
    {
        Texture.r = 0.f;
        Texture.g = 0.f;
        Texture.b = 0.f;
    }
    if (Texture.g >= 0.6f)
    {
        Texture.r = g_RGB.x;
        Texture.g = g_RGB.y;
        Texture.b = g_RGB.z;
        Texture.a = g_RGB.w;

    }
    
    
    
    Out.vColor = Texture;

        if (Out.vColor.a == 0.f)   
            discard;
    
        return Out;
    }


PS_OUT PS_MAIN_Shooting(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 Texture = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Texture.r == 0.f && Texture.g == 0.f && Texture.b == 0.f)
        discard;
   
   if (Texture.b >= 0.f & Texture.r <= 0.5f)
   {
       Texture.a = Texture.b; 
   }
   
   float3 color = lerp(g_RGB.rgb, g_RGBEnd.rgb, Texture.rgb);
   
   Out.vColor = float4(color, g_RGB.w);

   if (Out.vColor.a == 0.f)   
       discard;
    
    return Out;
}


PS_OUT PS_MAIN_Pade(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 Texture = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = Texture;
    Out.vColor.a *= g_TimeSum;
    
    return Out;
}
technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass DefaultPass1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f,0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HP();
	}

    pass DefaultPass2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BOSSHP();
    }

    pass DefaultPass3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PLAYERSPRINT();
    }

    pass DefaultPass4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PLAYERHEALTH();
    }
    pass DefaultPass5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Prrr();
    }

    pass DefaultPass6
    {
        SetRasterizerState(RS_Clockwise);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Shooting();
    }
    pass DefaultPass7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Pade();
    }
}
