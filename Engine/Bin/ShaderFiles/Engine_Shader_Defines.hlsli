


RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Clockwise
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = true;
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
