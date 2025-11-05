//=============================================================================
// ParticleRender.hlsl  (for CEffect_ParticleStream, POINTLIST + GS 버전)
//=============================================================================
#include "Engine_Shader_Defines.hlsli"

//--------------------------------------------------------
// 상수 버퍼
//--------------------------------------------------------
cbuffer CB_VIEWPROJ : register(b0)
{
    float4x4 g_View;
    float4x4 g_Proj;
    float3 g_CamPosWS;
    float g_fCamFar;
};

// 파티클 애니메이션 / 시트 설정값
cbuffer CB_PARTICLE_SPRITE_INFO : register(b1)
{
    uint g_Columns; // 가로 프레임 수
    uint g_Rows; // 세로 프레임 수
    uint g_TotalFrames; // 전체 프레임 수
    uint g_UseSpriteSheet; // 0 = 단일 텍스처, 1 = 시트 애니메이션
};

//--------------------------------------------------------
// 인스턴스 데이터 (CS_ParticleBuildInstance 가 생성한 결과)
//--------------------------------------------------------
struct GPU_ParticleInstance
{
    float3 vPosition;
    float fSize;
    float4 vColor;
    uint FrameIndex;
    float3 _Pad0;
};

StructuredBuffer<GPU_ParticleInstance> g_InstanceDataVS : register(t0);

// 파티클 텍스처 (단일 텍스처 또는 스프라이트 시트 1장)
Texture2D g_ParticleTexture : register(t1);

//--------------------------------------------------------
// 공통: frameIndex -> sprite sheet UV
//--------------------------------------------------------
float2 GetSpriteUV(float2 baseUV, uint frameIdx)
{
    // 단일 텍스처 모드면 그냥 baseUV 반환
    if (g_UseSpriteSheet == 0 || g_Columns == 0 || g_Rows == 0)
        return baseUV;

    uint totalFrames = max(g_Columns * g_Rows, 1);
    uint idx = frameIdx % totalFrames;

    uint col = idx % g_Columns;
    uint row = idx / g_Columns; // 위->아래로 진행한다고 가정

    // 각 프레임의 (0~1) 영역 크기
    float2 frameSize = float2(1.0f / (float) g_Columns,
                              1.0f / (float) g_Rows);

    // 최종 UV = 프레임 offset + 프레임 내부 uv
    float2 finalUV = baseUV * frameSize
                   + float2(frameSize.x * col,
                            frameSize.y * row);

    return finalUV;
}

struct VS_INPUT
{
    uint instanceID : SV_InstanceID;
};

struct VS_OUTPUT
{
    float3 vPosWS : POSITION_WS;
    float fSize : SIZE;
    float4 vColor : COLOR0;
    uint FrameIdx : FRAMEIDX;
};

VS_OUTPUT VS_MAIN(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT) 0;

    GPU_ParticleInstance inst = g_InstanceDataVS[In.instanceID];

    Out.vPosWS = inst.vPosition;
    Out.fSize = inst.fSize;
    Out.vColor = inst.vColor;
    Out.FrameIdx = inst.FrameIndex;

    return Out;
}

//--------------------------------------------------------
// GS 단계
//  - point(=particle center) -> billboard quad(4 vertices)
//  - 카메라 Right / Up 벡터로 4코너 생성
//--------------------------------------------------------
struct GS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
    float3 ViewPos : TEXCOORD1; // 뷰스페이스 위치 (Bloom 등 심도 falloff 용)
    uint FrameIdx : TEXCOORD2;
};

[maxvertexcount(4)]
void GS_Billboard(point VS_OUTPUT In[1], inout TriangleStream<GS_OUTPUT> triStream)
{
    GS_OUTPUT Out;

    // 카메라 방향축 (뷰행렬에서 추출)
    float3 camRight = normalize(float3(g_View._11, g_View._21, g_View._31));
    float3 camUp = normalize(float3(g_View._12, g_View._22, g_View._32));

    float3 centerWS = In[0].vPosWS;
    float halfSize = In[0].fSize * 0.5f;

    // 쿼드 코너 방향(+/- Right/Up)
    float3 cornerWS[4];
    cornerWS[0] = centerWS + (-camRight - camUp) * halfSize; // (-1,-1)
    cornerWS[1] = centerWS + (-camRight + camUp) * halfSize; // (-1,+1)
    cornerWS[2] = centerWS + (camRight - camUp) * halfSize; // (+1,-1)
    cornerWS[3] = centerWS + (camRight + camUp) * halfSize; // (+1,+1)

    float2 baseUV[4];
    baseUV[0] = float2(0.0f, 1.0f);
    baseUV[1] = float2(0.0f, 0.0f);
    baseUV[2] = float2(1.0f, 1.0f);
    baseUV[3] = float2(1.0f, 0.0f);

    // strip 형태로 4개 쏴주면 (0,1,2,3) 조합으로 삼각형 2장 나옴
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        float4 vPosV = mul(float4(cornerWS[i], 1.0f), g_View); // view space
        float4 vPosH = mul(vPosV, g_Proj); // clip space

        Out.PosH = vPosH;
        Out.UV = GetSpriteUV(baseUV[i], In[0].FrameIdx);
        Out.Color = In[0].vColor;
        Out.ViewPos = vPosV.xyz;
        Out.FrameIdx = In[0].FrameIdx;

        triStream.Append(Out);
    }

    triStream.RestartStrip();
}

//--------------------------------------------------------
// PS OUTPUT 정의
//  - 최종적으로 엔진에서 어디에 쓸건지에 따라 2가지 모드 중 선택
//--------------------------------------------------------

// 1) 디버그 단일 렌더타겟 모드 (SV_Target0 한 개만)
struct PS_OUTPUT_DEBUG
{
    float4 Color : SV_Target0;
};

// 2) 엔진 MRT 모드 (주석 풀면 사용 가능)
//struct PS_OUTPUT_MRT
//{
//    float4 vDiffuse  : SV_TARGET0;
//    float4 vNormal   : SV_TARGET1;
//    float4 vDepth    : SV_TARGET2;
//    float4 vRim      : SV_TARGET3;
//    float4 vEmissive : SV_TARGET4;
//    float4 vAmbient  : SV_TARGET5;
//    float4 vBloom    : SV_TARGET6;
//};

//--------------------------------------------------------
// PS_MAIN_BASE (디버그 버전)
//  - 텍스처 * 컬러
//  - 알파 컷
//--------------------------------------------------------
PS_OUTPUT_DEBUG PS_MAIN_BASE(GS_OUTPUT In)
{
    PS_OUTPUT_DEBUG Out = (PS_OUTPUT_DEBUG) 0;

    // 파티클 텍스처 샘플
    float4 tex = g_ParticleTexture.Sample(LinearSampler, In.UV);

    // 알파 테스트 (아주 얇은 가장자리 잘라내기)
    float alpha = tex.a * In.Color.a;
    if (alpha <= 0.1f)
        discard;

    float3 rgb = tex.rgb * In.Color.rgb;

    Out.Color = float4(rgb, alpha);

    // 디버그용: 만약 그냥 노란 사각형만 보고 싶으면 아래 줄 쓰고 위 코드 전부 주석 처리해.
    //Out.Color = float4(1,1,0,1);

    return Out;
}

//--------------------------------------------------------
// technique11
//--------------------------------------------------------
technique11 ParticleRender
{
    pass P0_Default
    {
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_Billboard();
        PixelShader = compile ps_5_0 PS_MAIN_BASE();
    }

    // P1_SpriteSheetAnim : 지금은 P0와 같지만 나중에 다른 블렌딩/라이팅 쓸 거면 여기 분리
    pass P1_SpriteSheetAnim
    {
        SetRasterizerState(RS_NONCULL);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0, 0, 0, 0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_Billboard();
        PixelShader = compile ps_5_0 PS_MAIN_BASE();
    }
}
