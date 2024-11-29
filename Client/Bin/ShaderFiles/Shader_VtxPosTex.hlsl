 #include "Engine_Shader_Defines.hlsli"
/* 0 1 2 0 2 3 */

/* 전역변수들 : 컨스턴트 테이블 */
/* 같은 파일 내에 존재하는 모든 함수에서 전역변수를 사용할 수 있다. 대입은 불가 */
/* 외부프로젝트에서 쉐이더 전역으로 특정 데이터를 던지고 받기위한 메모리공간의 의미. */
/* 전역변수는 다른 쉐이더파일에 같은 타입과 이름으로 선언된 변수가 있다라면 메모리공간을 공유한다. */
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


struct VS_IN    /*입력 매개변수*/
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
	/* 자료형　변수명　：　속성. */
    /*입력 서명을 형성하며，커스팀 정점 구조체의 자료 멤버들에 대응된다, 속성(시멘틱)은 정점 세이더 입력 매개변수들에 대응시키는 역할*/
};

struct VS_OUT   /*출력 매개변수, 참조나 포인터가 없음으로 함수의 반환은 구조체는 out이 지정된 출력 매개변수를 이용해야 한다*/
{
  	float4 vPosition : SV_POSITION;/*SV는 시스템 값 시멘틱,,*/
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos :  TEXCOORD1; 
    
    /*여기서 시멘틱은 은 정점 셰이더의 출력을 파이프라인의 다음 단계(기하 세이더 또는 픽셀 셰이더)의 해당 입력에 대응시키는 역할을 한다 */
};


/* Vertex Shader Entry Function */
/* 1. 정점의 기본 변환(로컬상에 정의된 정점을 월드, 뷰, 투영) 을 수행한다. */
/* 2. 정점의 구성 정보를 인위적으로 수정 변경이 가능하다. */
// VS_OUT VS_MAIN(float3 vPosition : POSITION, float2 vTexcoord : TEXCOORD0)

VS_OUT VS_MAIN( /* 내가 그릴려고 했던 정점을 받아오는거다*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* float3 * float4x4 좌변의 w를 1로 채워서 곱하기연산을 수행한다. */
	/* w로 xyzw를 모두 나눈다. */
	/*D3DXVec3TransformCoord();*/

	/* 진짜 순수하게 곱하기만 수행한다. mul은 w나누기를 실행하지 않는다*/
    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = vPosition;
    
    return Out;
}


/* 이후　HLSL이 자동으로 지원하는 부분*/
/* 세개의 결과 정점의 위치벡터의 w값으로 xyzw를 모두 나눈다. 투영스페이스로의 변환. */
/* 위치벡터를 뷰포트로 변환한다. (윈도우좌표로 변환) */
/* 래스터라이즈 (결과로 나온 정점들의 정보를 선형보간하여 픽셀이 생성된다.) */

struct PS_IN
{
    float4 vPosition : SV_POSITION;  /*픽셀 쉐이더에서 픽셀위치 변경 불가, 색 변경은 가능*/
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos :  TEXCOORD1;     /*쉐이더 루프에 안건드릴려고 시멘틱은 포지션이 아니라 다른 것으로..*/
  
};


struct PS_OUT
{
	/* 변수에 대한 시멘틱을 정의한다. */
    vector vColor : SV_TARGET0;

};

/* 픽셀 쉐이더 */
/* 픽셀의 최종적인 렌더링 색상을 결정하낟. */
/* 픽셀당 픽셀쉐이더를 하나씩 모두 수행한다. */
// vector PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    

     Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a == 0.f)  /*DX11에서는 알파테스트(알파값으로 반투명은 고려하지 않고 그릴지 말지 판단해 놓는 것)가 없음으로 쉐이더로 조건*/
        discard;
    
    //  float2 vProjPos = In.vProjPos.xy / In.vProjPos.w;     /*뷰 포트 좌표를 투영 좌표로 전환이 가능*/
    
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
/* 지정해준 색을 지정한 렌더타겟에 그려준다. */



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

    if (Out.vColor.a == 0.f)  /*DX11에서는 알파테스트(알파값으로 반투명은 고려하지 않고 그릴지 말지 판단해 놓는 것)가 없음으로 쉐이더로 조건*/
        discard;
    
    //  float2 vProjPos = In.vProjPos.xy / In.vProjPos.w;     /*뷰 포트 좌표를 투영 좌표로 전환이 가능*/
    
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
{ /*Technique은 특정 렌더링 작업을 정의하는 셰이더 코드 블록*/
  /* Pass는 셰이더 실행의 단위로, 정점 셰이더, 픽셀 셰이더 등의 그래픽 파이프라인 단계에서 실행될 셰이더들을 정의함*/
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
