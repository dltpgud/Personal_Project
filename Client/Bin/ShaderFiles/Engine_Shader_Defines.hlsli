
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

SamplerState NoMipSampler
{
    Filter = MIN_MAG_MIP_LINEAR; // 또는 POINT
    AddressU = Clamp;
    AddressV = Clamp;
    MipLODBias = 0;
    MinLOD = 0;
    MaxLOD = 0; // ?? 밈맵 0레벨만 샘플링하도록 강제
};

SamplerComparisonState ShadowCmpSampler : register(s7)
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
    ComparisonFunc = LESS_EQUAL;
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

BlendState BS_WBOIT
{
    // RT 0 : AccumColor
    BlendEnable[0] = true;
    SrcBlend[0] = ONE;
    DestBlend[0] = ONE;
    BlendOp[0] = ADD;
    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ONE;
    BlendOpAlpha[0] = ADD;

    // RT 1 : AccumNormal
    BlendEnable[1] = true;
    SrcBlend[1] = ONE;
    DestBlend[1] = ONE;
    BlendOp[1] = ADD;
    SrcBlendAlpha[1] = ONE;
    DestBlendAlpha[1] = ONE;
    BlendOpAlpha[1] = ADD;

    // RT 2 : Revealage
    BlendEnable[2] = true;
    SrcBlend[2] = ONE;
    DestBlend[2] = ONE;
    BlendOp[2] = ADD;
    SrcBlendAlpha[2] = ONE;
    DestBlendAlpha[2] = ONE;
    BlendOpAlpha[2] = ADD;
};

RasterizerState RS_Shadow
{
    FillMode = Solid;
    CullMode = Back; // 필요에 따라 None으로도 가능
    FrontCounterClockwise = false;

    DepthBias = 100; // 픽셀 단위 오프셋 (값은 GPU에 따라 조정 필요)
    SlopeScaledDepthBias = 1.0f; // 기울기 따라 추가 오프셋
    DepthBiasClamp = 0.0f; // 오프셋 최대 제한 (보통 0 = 무제한)
};

RasterizerState RS_Decal
{
    FillMode = Solid;
    CullMode = front; 
    FrontCounterClockwise = false;

};

BlendState BS_Decal
{
    BlendEnable[0] = true;
    SrcBlend[0] = Src_Alpha;
    DestBlend[0] = Inv_Src_Alpha;
    BlendOp[0] = Add;

    SrcBlendAlpha[0] = One; // 알파 채널 유지
    DestBlendAlpha[0] = Inv_Src_Alpha;
    BlendOpAlpha[0] = Add;
};
BlendState BS_AlphaBlend_Effect
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;

    SrcBlendAlpha = Src_Alpha;
    DestBlendAlpha = Inv_Src_Alpha;
    BlendOpAlpha = Add;
    
};


