 #include "Engine_Shader_Defines.hlsli"
/* 0 1 2 0 2 3 */

/* ���������� : ������Ʈ ���̺� */
/* ���� ���� ���� �����ϴ� ��� �Լ����� ���������� ����� �� �ִ�. ������ �Ұ� */
/* �ܺ�������Ʈ���� ���̴� �������� Ư�� �����͸� ������ �ޱ����� �޸𸮰����� �ǹ�. */
/* ���������� �ٸ� ���̴����Ͽ� ���� Ÿ�԰� �̸����� ����� ������ �ִٶ�� �޸𸮰����� �����Ѵ�. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_hp, g_hp_pluse;
texture2D g_Texture, g_Texture0, g_Texture1, g_Texture2;

sampler LinearSampler = sampler_state  /*����� �ʱ�ȭ�� ���ÿ� �� ��*/
{
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
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

    if (In.vTexcoord.x < g_hp || In.vTexcoord.x == g_hp_pluse)
        Out.vColor = g_Texture1.Sample(LinearSampler, In.vTexcoord);
    else if (In.vTexcoord.x < g_hp_pluse)
        Out.vColor = g_Texture2.Sample(LinearSampler, In.vTexcoord);
    else 
        Out.vColor = g_Texture0.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a == 0.f)   
        discard;
    
    return Out;
}


/* �������� ���� ������ ����Ÿ�ٿ� �׷��ش�. */

technique11 DefaultTechnique
{ /*Technique�� Ư�� ������ �۾��� �����ϴ� ���̴� �ڵ� ���*/
  /* Pass�� ���̴� ������ ������, ���� ���̴�, �ȼ� ���̴� ���� �׷��� ���������� �ܰ迡�� ����� ���̴����� ������*/
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass DefaultPass1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f,0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_HP();
	}
  

}
