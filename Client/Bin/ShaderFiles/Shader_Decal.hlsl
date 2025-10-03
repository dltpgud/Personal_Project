#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float2 g_WinDowSize;
float g_fCamFar;
texture2D g_DepthTexture, g_NormalTexture, g_DecalArray;
//Texture2DArray g_DecalArray;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
vector g_vCamPosition;
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 Texcoord : TEXCOORD0;

    // Instance Data (slot 1)
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vPos : TEXCOORD4;
    float3 Tangent : TEXCOORD5;
    float3 Binormal : TEXCOORD6;
    float3 Normal : TEXCOORD7;
    float3 DecalPos : TEXCOORD8;
    float3 DecalDir : TEXCOORD9;
    float3 HalfSize : TEXCOORD10;
    float LifeTime : TEXCOORD11;
    float DecalTime : TEXCOORD12;
    int TexIndex : TEXCOORD13;
    int bNormal : TEXCOORD14;
    int DecalType : TEXCOORD15;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD0;

    // Pass-through instance data
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vPos : TEXCOORD4;
    float3 Tangent : TEXCOORD5;
    float3 Binormal : TEXCOORD6;
    float3 Normal : TEXCOORD7;
    float3 DecalPos : TEXCOORD8;
    float3 DecalDir : TEXCOORD9;
    float3 HalfSize : TEXCOORD10;
    float LifeTime : TEXCOORD11;
    float DecalTime : TEXCOORD12;
    int TexIndex : TEXCOORD13;
    int bNormal : TEXCOORD14;
    int DecalType : TEXCOORD15;
};

VS_OUTPUT VS_Decal(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT) 0;

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.Position = mul(float4(In.Position, 1.f), matWVP);
    Out.Texcoord = In.Texcoord;

    // Pass instance data
    Out.vRight = In.vRight;
    Out.vUp = In.vUp;
    Out.vLook = In.vLook;
    Out.vPos = In.vPos;
    Out.Tangent = In.Tangent;
    Out.Binormal = In.Binormal;
    Out.Normal = In.Normal;
    Out.DecalPos = In.DecalPos;
    Out.DecalDir = In.DecalDir;
    Out.HalfSize = In.HalfSize;
    Out.LifeTime = In.LifeTime;
    Out.DecalTime = In.DecalTime;
    Out.TexIndex = In.TexIndex;
    Out.bNormal = In.bNormal;
    Out.DecalType = In.DecalType;

    return Out;
}
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vPos : TEXCOORD4;
    float3 Tangent : TEXCOORD5;
    float3 Binormal : TEXCOORD6;
    float3 Normal : TEXCOORD7;
    float3 DecalPos : TEXCOORD8;
    float3 DecalDir : TEXCOORD9;
    float3 HalfSize : TEXCOORD10;
    float LifeTime : TEXCOORD11;
    float DecalTime : TEXCOORD12;
    int TexIndex : TEXCOORD13;
    int bNormal : TEXCOORD14;
    int DecalType : TEXCOORD15;
};

struct PS_OUT
{
    float4 vDecal : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    
};

float4 ComputeDecalEffect(float3 decal, float decalTime, float lifeTime)
{
    float glowPhase = saturate(1.0 - decalTime * 1.0f);
    float3 glowColor = decal * 4.0;
    float3 colorWithGlow = lerp(decal, glowColor, glowPhase);

    float3 grayColor = float3(0.25, 0.25, 0.25);
    float3 Color = (glowPhase > 0.01f) ? colorWithGlow : grayColor;
    float fade = saturate(lifeTime);

    return float4(Color, fade);
}

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


PS_OUT PS_Decal(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 screenUV = In.vPosition.xy / g_WinDowSize;
    float4 worldPos = Compute_WorldPos_byCamera(screenUV);
   
    float3 surfNormal = normalize(g_NormalTexture.Sample(PointSamplerClamp, screenUV).xyz * 2.0f - 1.0f);

    if (dot(surfNormal, In.DecalDir) > -0.3f)   // -1이면 정면, 0이면 수직
        discard;
    
    float EXDeapth = g_DepthTexture.Sample(PointSamplerClamp, screenUV).w;
    if (EXDeapth > 0.5)
        discard;
    
    float3 N = normalize(In.DecalDir); // 데칼 방향을 기준 노멀로
    float3 up = abs(N.y) < 0.999f ? float3(0, 1, 0) : float3(1, 0, 0);
    float3 T = normalize(cross(up, N));
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);

    float3 rel = (worldPos.xyz - In.DecalPos) / In.HalfSize;
    float3 localPos = mul(rel, transpose(TBN));

    if (any(abs(localPos.xy) > 1.0f))
        discard;

    float2 uv = localPos.xy * 0.5f + 0.5f;
    float4 decal = g_DecalArray.Sample(PointSamplerClamp, uv);
    if (decal.a < 0.01f)
        discard;
    
    
// 1. 씬 깊이 (linear, g 채널에 저장되어 있음)
    float sceneLinear = g_DepthTexture.Sample(PointSamplerClamp, screenUV).g;

// 2. worldPos -> view space 변환
    float4 viewPos = mul(float4(worldPos.xyz, 1.0f), g_ViewMatrix);
    float pixelLinear = viewPos.z / g_fCamFar; // 카메라 전방(z축) 거리 기준 (0~1)

// 3. 비교
    if (pixelLinear > sceneLinear + 0.001f)
        discard;



   // if (true == g_bDecalNormal)
   // {
   //     float3 nTS = g_DecalNormalAtlas.Sample(PointSamplerClamp, uv).xyz * 2 - 1;
   //     nTS.z = sqrt(saturate(1 - dot(nTS.xy, nTS.xy)));
   //     float3 decalNormal = normalize(mul(nTS, TBN));
   //
   //     float3 blendedNormal = normalize(lerp(surfNormal, decalNormal, decal.a));
   //     Out.vNormal = float4(blendedNormal * 0.5f + 0.5f, 0.f);
   // }
   // else
        Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
    
    float4 finalColor = ComputeDecalEffect(decal.rgb, In.DecalTime, In.LifeTime);

    Out.vDecal = float4(finalColor.rgb, decal.a * finalColor.a);

    return Out;
}
technique11 DecalTech
{
    pass Decal_BoXProj //8  
    {
        SetRasterizerState(RS_Decal);
        SetDepthStencilState(DSS_Decal, 0);
        SetBlendState(BS_Decal, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_Decal();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Decal();
    }
}
