
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_LightViewMatrixInv, g_LightProjMatrixInv;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
texture2D g_Texture;
texture2D g_OutLineTexture;

vector g_vLightDir;
vector g_vLightDirS = {-0.2f, -1.f, -0.5f, 0.f};

vector g_vLightPos;
float g_fLightRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

texture2D g_NormalTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;

texture2D g_ShadeTexture;
texture2D g_DiffuseTexture;
texture2D g_LightDepthTexture;

texture2D g_EmissiveTexture;
texture2D g_RimTexture;

texture2D g_FinalTexture;
texture2D g_BlurTexture;
texture2D g_BrightnessTexTure;

texture2D g_vMtrlAmbient; 
vector g_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

vector g_vCamPosition;

float2 g_WinDowSize;
float g_fCamFar;

// 텍스쳐에서 한 픽셀의 간격
float dX;
float dY;
float Bloom_Weights[5] = { 0.0545, 0.2442, 0.4026, 0.2442 , 0.0545 };
float2 g_shadowMapSize;

float4 Compute_WorldPos_byCamera(float2 vTexcoord)
{
    float4 vWorldPos = 0.f;

    vector vDepthDesc = g_DepthTexture.Sample(PointSamplerClamp, vTexcoord);
    float fViewZ = vDepthDesc.y * g_fCamFar;
	
    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    return vWorldPos;
}


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
   PS_OUT_LIGHT Out;
  
  /* 빛 정보와 노말 정보를 이용해서 명암을 계산하여 리턴한다. */
   vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
   vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

   // 빛이 오는 방향과 법선을 내적해 빛이 표면에 얼마나 닿는지 수치화해 음영을 구함.
   // 음수 값이 나오면 음영이 반대로 들어감으로 최소값은 0으로 설정
   float fShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);
  
   // Diffuse 와 Ambient 조명을 고려한 최종 조도를 구한다.
   vector vAmbiet = g_vMtrlAmbient.Sample(PointSampler, In.vTexcoord);
   Out.vShade = g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * vAmbiet));
    
   // 픽셀들의 월드 좌표를 복원한다.
   float4 vWorldPos = Compute_WorldPos_byCamera(In.vTexcoord);
    
   float4 vLook = vWorldPos - g_vCamPosition;
   
   //광원 방향을 기준으로 표면 법선에 반사된 방향
   float4 vReflect = reflect(normalize(g_vLightDir), vNormal);
   //정반사 계수 계산
   // 내적 값이 1에 가까우면 하이라이트가 강해짐  
   float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);
   //광원의 스펙큘러 색과 머티리얼의 스펙큘러 반응도를 곱해 반사광 색상을 정한다.
   Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular * 0.8f;
    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);

    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    float4 vWorldPos = Compute_WorldPos_byCamera(In.vTexcoord);

    vector vLightDir = vWorldPos - g_vLightPos;

    // 점조명의 영역을 정하자
    float fDistance = length(vLightDir);

    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    float fShade = max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f);

    vector vAmbiet = g_vMtrlAmbient.Sample(PointSampler, In.vTexcoord);
    Out.vShade = (g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * vAmbiet))) * fAtt;
	
    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(vLightDir), vNormal);

    float fSpecular = (pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f)) * fAtt;
    
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular *0.8f;

    return Out;
}

float Compute_OutLine(float2 vTexcoord)
{
    // 화면에서 좌,좌위,좌아래,우,우위,우아래,위,아래로 한 픽셀식 움직이고, Projpos.w / g_fCamFar 저장된 값을 g_fCamFar 와 곱해 실제 카메라 거리로 복원
    float depthMid    = g_OutLineTexture.Sample(LinearSampler, vTexcoord).y * g_fCamFar;
    float depthLeft   = g_OutLineTexture.Sample(LinearSampler, vTexcoord + float2(-1.f / g_WinDowSize.x, 0.f)).y * g_fCamFar;
    float depthRight  = g_OutLineTexture.Sample(LinearSampler, vTexcoord + float2(1.f / g_WinDowSize.x, 0.f)).y * g_fCamFar;
    float depthUp     = g_OutLineTexture.Sample(LinearSampler, vTexcoord + float2(0.f, -1.f / g_WinDowSize.y)).y * g_fCamFar;
    float depthDown   = g_OutLineTexture.Sample(LinearSampler, vTexcoord + float2(0.f, 1.f / g_WinDowSize.y)).y * g_fCamFar;
    float depthUL     = g_OutLineTexture.Sample(LinearSampler, vTexcoord + float2(-1.f / g_WinDowSize.x, -1.f / g_WinDowSize.y)).y * g_fCamFar;
    float depthUR     = g_OutLineTexture.Sample(LinearSampler, vTexcoord + float2(1.f / g_WinDowSize.x, -1.f / g_WinDowSize.y)).y * g_fCamFar;
    float depthDL     = g_OutLineTexture.Sample(LinearSampler, vTexcoord + float2(-1.f / g_WinDowSize.x, 1.f / g_WinDowSize.y)).y * g_fCamFar;
    float depthDR     = g_OutLineTexture.Sample(LinearSampler, vTexcoord + float2(1.f / g_WinDowSize.x, 1.f / g_WinDowSize.y)).y * g_fCamFar;

    // 차이 계산
    float depthDiffH  = abs(depthLeft - depthRight);
    float depthDiffV  = abs(depthUp - depthDown);
    float depthDiffD1 = abs(depthUL - depthDR);
    float depthDiffD2 = abs(depthUR - depthDL);

    // 거리 비례 임계값
    float depthThreshold = sqrt(depthMid) * g_OutLineTexture.Sample(LinearSampler, vTexcoord).z;

    // 외곽 판정
    bool isEdge = (depthDiffH > depthThreshold) || (depthDiffV > depthThreshold) || (depthDiffD1 > depthThreshold) || (depthDiffD2 > depthThreshold);

    float OutLine = 1.f;
    if (isEdge)
    {
        // 평균 (8방향 모두 포함)
        float avgNeighbor = (depthLeft + depthRight + depthUp + depthDown + depthUL + depthUR + depthDL + depthDR) /8;

        if (abs(depthMid - avgNeighbor) >= depthThreshold)
            OutLine = 0.f; // 외곽선 그리기
    }

    return OutLine;
}

// 라이트 공간 투영 + UV/Depth 계산
void ProjectToLight(float4 worldPos, out float2 uv, out float depth, out bool valid)
{
    float4 lp = mul(worldPos, g_LightViewMatrix);
    lp = mul(lp, g_LightProjMatrix);

    // D3D: NDC z = z/w ∈ [0,1]  (★ 0.5+0.5 하면 안 됨)
    float invw = rcp(lp.w);
    float nx = lp.x * invw; // -1..1
    float ny = lp.y * invw; // -1..1
    float nz = lp.z * invw; //  0..1  (D3D)

    // 텍스처 좌표 변환 (y 반전)
    uv = float2(nx * 0.5f + 0.5f, -ny * 0.5f + 0.5f);
    depth = saturate(nz);

    // 클립 (경계 밖은 섀도우 0으로 처리)
    valid = all(uv >= 0.0f) && all(uv <= 1.0f) && (lp.w > 0.0f);
}

float ComputeReceiverBias(float3 n, float3 l, float3 v)
{
    // 기저 편향 + 경사 기반(슬로프) 편향
    const float baseBias = 0.0004f; // 필요시 0.0003~0.002 범위에서 튜닝
    const float slopeBias = (1.0f - saturate(dot(n, -l))) * 0.0040f;
    const float viewBias = (1.0f - abs(dot(n, v))) * 0.0020f; // 뷰 방향 기반 추가 보정
    return baseBias + slopeBias + viewBias;
}

// 3x3 PCF (SampleCmpLevelZero 사용)
float PCF_Shadow(float2 uv, float depth, float2 shadowMapSize)
{
    float2 texel = 1.0f / shadowMapSize;

    float sum = 0.0f;
    [unroll]
    for (int y = -1; y <= 1; ++y)
    {
        [unroll]
        for (int x = -1; x <= 1; ++x)
        {
            float2 o = float2(x, y) * texel;
            // SampleCmpLevelZero: 비교값(depth)을 주면 샘플 내부에서 비교 수행
            sum += g_LightDepthTexture.SampleCmpLevelZero(ShadowCmpSampler, uv + o, depth);
        }
    }
    return sum / 9.0f; // 1이면 완전 밝음(그림자 X), 0이면 어둠(그림자 100%)
}


PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse  = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vShade    = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    vector vRim      = g_RimTexture.Sample(LinearSampler, In.vTexcoord);
    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);    
    float fOutLine   = Compute_OutLine(In.vTexcoord);
  
    if (vDiffuse.a == 0.f)
        discard;

     vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
     float fCurIsSpecular = vDepthDesc.z;
     
     if (fCurIsSpecular == 0.f)
         vSpecular = 0.f;
   
    
    vector vPositionWS = Compute_WorldPos_byCamera(In.vTexcoord);

// 라이트 투영
    float2 lightUV;
    float lightDepth;
    bool inLight = false;
    ProjectToLight(vPositionWS, lightUV, lightDepth, inLight);

// 기본값: 그림자 없음
    float shadowFactor = 1.0f;

// 경계 안쪽일 때만 샘플
    if (inLight)
    {
    // 노멀/라이트로 수신면 편향 계산
        float3 n = normalize(g_NormalTexture.Sample(PointSampler, In.vTexcoord).xyz * 2.0f - 1.0f);
        float3 l = normalize(g_vLightDirS.xyz);
        float3 v = normalize(g_vCamPosition.xyz - vPositionWS.xyz);

    // 편향은 비교 깊이에 "마이너스"로 적용 (그림자가 너무 뜨는 현상 방지)
        float depthWithBias = saturate(lightDepth - ComputeReceiverBias(n, l,v));

    // SampleCmp 기반 3x3 PCF (반환값: 1=밝음, 0=어두움)
        float pcf = PCF_Shadow(lightUV, depthWithBias, g_shadowMapSize);

    // shadowFactor는 "빛의 스케일"이므로 pcf 그대로 사용
        shadowFactor = pcf;
    }

    Out.vColor = vDiffuse * vShade * fOutLine + vRim + vSpecular + vEmissive;

    Out.vColor.rgb *= lerp(0.8f, 1.0f, shadowFactor);  
  
    return Out;
}

PS_OUT PS_MAIN_PURE(PS_IN In)
{ 
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexcoord);
    
    Out.vColor = vDiffuse;
    
    return Out;
}

// 블러 X
PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    
    int i;
    for (i = 0; i < 5; i++)
    {
        vDiffuse += Bloom_Weights[i] * g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexcoord + float2(dX, 0.0) * float(i - 2));
    }
    
    Out.vColor = vDiffuse;

    return Out;
}

// 블러 Y
PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    
    int i;
    for (i = 0; i < 5; i++)
    {
        vDiffuse += Bloom_Weights[i] * g_DiffuseTexture.Sample(LinearSamplerClamp, In.vTexcoord + float2(0.0, dY) * float(i - 2));
    }
    
    Out.vColor = vDiffuse;

    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Final
    {
        SetRasterizerState(RS_Shadow);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

    pass Pure
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PURE();
    }
   
    pass BLUR_X 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass BLUR_Y 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Light, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }
}
