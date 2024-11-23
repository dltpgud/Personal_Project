#include "Engine_Shader_Defines.hlsli"
/* 0 1 2 0 2 3 */

/* ���������� : ������Ʈ ���̺� */
/* ���� ���� ���� �����ϴ� ��� �Լ����� ���������� ����� �� �ִ�. ������ �Ұ� */
/* �ܺ�������Ʈ���� ���̴� �������� Ư�� �����͸� ������ �ޱ����� �޸𸮰����� �ǹ�. */

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
vector g_vCamPosition;


struct VS_IN
{
    float3 vPosition : POSITION;

};


struct VS_OUT
{
    float4 vPosition : POSITION;

};

VS_OUT VS_MAIN( /* ���� �׸����� �ߴ� ������ �޾ƿ��°Ŵ�*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = float4(In.vPosition, 1.f);

	/* ��¥ �����ϰ� ���ϱ⸸ �����Ѵ�. */
    vPosition = mul(vPosition, g_WorldMatrix);

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
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz));
    float3 vUp = normalize(cross(vLook.xyz, vRight)) ;

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


/* ������ ��� ������ ��ġ������ w������ xyzw�� ��� ������. ���������̽����� ��ȯ. */
/* ��ġ���͸� ����Ʈ�� ��ȯ�Ѵ�. (��������ǥ�� ��ȯ) */
/* �����Ͷ����� (����� ���� �������� ������ ���������Ͽ� �ȼ��� �����ȴ�.) */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;

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



	/* g_Texture��� �ؽ��������� ������ �ؽ���忡 ��ġ�� �ȼ��� ���� LinearSampler����� ���´�. */
    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);

    if (Out.vColor.a == 0.f)
        discard;




    return Out;
}

/* �������� ���� ������ ����Ÿ�ٿ� �׷��ش�. */

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
