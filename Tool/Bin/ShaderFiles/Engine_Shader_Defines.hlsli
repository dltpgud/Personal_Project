
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler LinearSamplerClamp = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};
SamplerComparisonState ShadowSampler
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
    ComparisonFunc = LESS_EQUAL;
};




sampler PointSampler = sampler_state
{
    filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};
SamplerState PointSamplerClamp : register(s3)
{
    Filter = MIN_MAG_MIP_POINT; // Point filtering
    AddressU = clamp; // Clamp to edge
    AddressV = clamp;
    AddressW = clamp;
    ComparisonFunc = ALWAYS; // �׳� ���� �ؽ�ó ���� ��
};

RasterizerState RS_Clockwise
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = true;
};

RasterizerState RS_NONCULL
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Sky
{
    FillMode = Solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_Debug
{
    FillMode = WireFrame;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};
DepthStencilState DSS_DefaultNoWrite
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    SrcBlend[0] = Src_Alpha;
    DestBlend[0] = Inv_Src_Alpha;
    BlendOp[0] = Add;
};

BlendState BS_Light
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

RasterizerState RS_Shadow
{
    FillMode = Solid; // ���̾������� X
    CullMode = Back; // ������ �ﰢ�� ����
    FrontCounterClockwise = false; // �ð�/�ݽð� ���߱� (���� ��ǥ�迡 �°�)
    DepthBias = 100; // �׸��� Acne ����
    SlopeScaledDepthBias = 1.0;
    DepthClipEnable = true;
};
DepthStencilState DSS_Shadow
{
    DepthEnable = true; // ���� �׽�Ʈ ��
    DepthWriteMask = all; // ���� �����
    DepthFunc = less_equal; // z-test
};
BlendState BS_Shadow
{
    BlendEnable[0] = false; // ���� ���ʿ�
};
