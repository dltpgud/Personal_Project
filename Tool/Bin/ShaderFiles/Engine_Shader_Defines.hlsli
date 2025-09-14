
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
    ComparisonFunc = ALWAYS; // 그냥 깊이 텍스처 읽을 때
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
    FillMode = Solid;
    CullMode = Back; // 필요에 따라 None으로도 가능
    FrontCounterClockwise = false;

    DepthBias = 50; // 픽셀 단위 오프셋 (값은 GPU에 따라 조정 필요)
    SlopeScaledDepthBias = 1.0f; // 기울기 따라 추가 오프셋
    DepthBiasClamp = 0.0f; // 오프셋 최대 제한 (보통 0 = 무제한)
};

SamplerComparisonState ShadowCmpSampler : register(s7)
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
    ComparisonFunc = LESS; 
};

