 #include "Engine_Shader_Defines.hlsli"
/* 0 1 2 0 2 3 */

/* ���������� : ������Ʈ ���̺� */
/* ���� ���� ���� �����ϴ� ��� �Լ����� ���������� ����� �� �ִ�. ������ �Ұ� */
/* �ܺ�������Ʈ���� ���̴� �������� Ư�� �����͸� ������ �ޱ����� �޸𸮰����� �ǹ�. */
/* ���������� �ٸ� ���̴����Ͽ� ���� Ÿ�԰� �̸����� ����� ������ �ִٶ�� �޸𸮰����� �����Ѵ�. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_hp, g_hp_pluse;
texture2D g_Texture, g_Texture0, g_Texture1, g_Texture2;


int g_Discard;
float4 g_Alpha;
float4 g_RGB;
float4 g_RGBEnd;


struct VS_IN_SAMPLE
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct VS_IN    /*�Է� �Ű�����*/
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
	/* �ڷ����������������Ӽ�. */
    /*�Է� ������ �����ϸ磬Ŀ���� ���� ����ü�� �ڷ� ����鿡 �����ȴ�, �Ӽ�(�ø�ƽ)�� ���� ���̴� �Է� �Ű������鿡 ������Ű�� ����*/
};

struct VS_OUT   /*��� �Ű�����, ������ �����Ͱ� �������� �Լ��� ��ȯ�� ����ü�� out�� ������ ��� �Ű������� �̿��ؾ� �Ѵ�*/
{
  	float4 vPosition : SV_POSITION;/*SV�� �ý��� �� �ø�ƽ,,*/
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos :  TEXCOORD1; 
    
    /*���⼭ �ø�ƽ�� �� ���� ���̴��� ����� ������������ ���� �ܰ�(���� ���̴� �Ǵ� �ȼ� ���̴�)�� �ش� �Է¿� ������Ű�� ������ �Ѵ� */
};


/* Vertex Shader Entry Function */
/* 1. ������ �⺻ ��ȯ(���û� ���ǵ� ������ ����, ��, ����) �� �����Ѵ�. */
/* 2. ������ ���� ������ ���������� ���� ������ �����ϴ�. */
// VS_OUT VS_MAIN(float3 vPosition : POSITION, float2 vTexcoord : TEXCOORD0)

VS_OUT VS_MAIN( /* ���� �׸����� �ߴ� ������ �޾ƿ��°Ŵ�*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* float3 * float4x4 �º��� w�� 1�� ä���� ���ϱ⿬���� �����Ѵ�. */
	/* w�� xyzw�� ��� ������. */
	/*D3DXVec3TransformCoord();*/

	/* ��¥ �����ϰ� ���ϱ⸸ �����Ѵ�. mul�� w�����⸦ �������� �ʴ´�*/
    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = vPosition;
    
    return Out;
}


/* ���ġ�HLSL�� �ڵ����� �����ϴ� �κ�*/
/* ������ ��� ������ ��ġ������ w������ xyzw�� ��� ������. ���������̽����� ��ȯ. */
/* ��ġ���͸� ����Ʈ�� ��ȯ�Ѵ�. (��������ǥ�� ��ȯ) */
/* �����Ͷ����� (����� ���� �������� ������ ���������Ͽ� �ȼ��� �����ȴ�.) */

struct PS_IN
{
    float4 vPosition : SV_POSITION;  /*�ȼ� ���̴����� �ȼ���ġ ���� �Ұ�, �� ������ ����*/
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos :  TEXCOORD1;     /*���̴� ������ �Ȱǵ帱���� �ø�ƽ�� �������� �ƴ϶� �ٸ� ������..*/
  
};


struct PS_OUT
{
	/* ������ ���� �ø�ƽ�� �����Ѵ�. */
    vector vColor : SV_TARGET0;

};

/* �ȼ� ���̴� */
/* �ȼ��� �������� ������ ������ �����ϳ�. */
/* �ȼ��� �ȼ����̴��� �ϳ��� ��� �����Ѵ�. */
// vector PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    

     Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a == 0.f)  /*DX11������ �����׽�Ʈ(���İ����� �������� ������� �ʰ� �׸��� ���� �Ǵ��� ���� ��)�� �������� ���̴��� ����*/
        discard;
    
    //  float2 vProjPos = In.vProjPos.xy / In.vProjPos.w;     /*�� ��Ʈ ��ǥ�� ���� ��ǥ�� ��ȯ�� ����*/
    
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
/* �������� ���� ������ ����Ÿ�ٿ� �׷��ش�. */



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

    if (Out.vColor.a == 0.f)  /*DX11������ �����׽�Ʈ(���İ����� �������� ������� �ʰ� �׸��� ���� �Ǵ��� ���� ��)�� �������� ���̴��� ����*/
        discard;
    
    //  float2 vProjPos = In.vProjPos.xy / In.vProjPos.w;     /*�� ��Ʈ ��ǥ�� ���� ��ǥ�� ��ȯ�� ����*/
    
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



technique11 DefaultTechnique
{ /*Technique�� Ư�� ������ �۾��� �����ϴ� ���̴� �ڵ� ���*/
  /* Pass�� ���̴� ������ ������, ���� ���̴�, �ȼ� ���̴� ���� �׷��� ���������� �ܰ迡�� ����� ���̴����� ������*/
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

}
