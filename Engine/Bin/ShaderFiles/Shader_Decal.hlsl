#include "Engine_Shader_Defines.hlsli"
  
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float2 g_WinDowSize;
float g_fCamFar;
texture2D g_DepthTexture, g_NormalTexture;
Texture2DArray g_DecalArray;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
vector g_vCamPosition;

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 Texcoord : TEXCOORD0;
    
    row_major float4x4 WorldInv : TEXCOORD1;
    float3 DecalPos : TEXCOORD5;
    float3 DecalDir : TEXCOORD6;
    float3 HalfSize : TEXCOORD7;
    float LifeTime : TEXCOORD8;
    float DecalTime : TEXCOORD9;
    int TexIndex : TEXCOORD10;
    int bNormal : TEXCOORD11;
    int DecalType : TEXCOORD12;
    int ProtoIndex : TEXCOORD13;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD0;
    
    row_major float4x4 WorldInv : TEXCOORD1;
    float3 DecalPos : TEXCOORD5;
    float3 DecalDir : TEXCOORD6;
    float3 HalfSize : TEXCOORD7;
    float LifeTime : TEXCOORD8;
    float DecalTime : TEXCOORD9;
    int TexIndex : TEXCOORD10;
    int bNormal : TEXCOORD11;
    int DecalType : TEXCOORD12;
    int ProtoIndex : TEXCOORD13;
};

VS_OUTPUT VS_Decal(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT) 0;

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.Position = mul(float4(In.Position, 1.f), matWVP);
    Out.Texcoord = In.Texcoord;

    Out.WorldInv = In.WorldInv;
    Out.DecalPos = In.DecalPos;
    Out.DecalDir = In.DecalDir;
    Out.HalfSize = In.HalfSize;
    Out.LifeTime = In.LifeTime;
    Out.DecalTime = In.DecalTime;
    Out.TexIndex = In.TexIndex;
    Out.bNormal = In.bNormal;
    Out.DecalType = In.DecalType;
    Out.ProtoIndex = In.ProtoIndex;
    return Out;
}
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    row_major float4x4 WorldInv : TEXCOORD1;
    float3 DecalPos : TEXCOORD5;
    float3 DecalDir : TEXCOORD6;
    float3 HalfSize : TEXCOORD7;
    float LifeTime : TEXCOORD8;
    float DecalTime : TEXCOORD9;
    int TexIndex : TEXCOORD10;
    int bNormal : TEXCOORD11;
    int DecalType : TEXCOORD12;
    int ProtoIndex : TEXCOORD13;
};

struct PS_OUT
{
    float4 vDecal : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    
};

float4 ComputeDecalEffect(float3 decal, float decalTime, float lifeTime)
{
    float t = saturate(decalTime / lifeTime);
    float slowFactor = log2(1.0 + 1.0 - t); // 느리게 줄어듦
    float glowPhase = saturate(slowFactor);

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

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
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
     
    float EXDeapth = g_DepthTexture.Sample(NoMipSampler, screenUV).w;
    if (EXDeapth > 0.5)
        discard;
    
    float3 N = normalize(In.DecalDir); 
    float3 up = abs(N.y) < 0.999f ? float3(0, 1, 0) : float3(1, 0, 0);
    float3 T = normalize(cross(up, N));
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);
    float3 surfNormal = normalize(g_NormalTexture.SampleLevel(NoMipSampler, screenUV, 0).xyz * 2.0f - 1.0f);

    
    bool kill = false;
    float2 uv;
    float4 localPos;
    if(In.DecalType == 0 )  //BOX
    {
        localPos = mul(worldPos, In.WorldInv);
  
        if (any(abs(localPos.xyz) > 1.f))
            discard;
        
       if (dot(normalize(surfNormal), normalize(In.DecalDir)) < 0.1f)
            discard;
              
    }
    else
    { //SSD
        float3 rel = (worldPos.xyz - In.DecalPos) / In.HalfSize;
        localPos = float4(mul(rel, transpose(TBN)), 1);
        if (any(abs(localPos.xy) > 1.0f))
            discard;
    }
            
    uv = localPos.xy * 0.5f + 0.5f;
    
    float4 decal = g_DecalArray.Sample(NoMipSampler, float3(uv, In.ProtoIndex));

    float edgeFade = saturate(1.0f - max(abs(localPos.x), max(abs(localPos.y), abs(localPos.z))));
    edgeFade = pow(edgeFade, 1.0f);
    decal.a *= edgeFade;
    
    if (decal.a < 0.01f)
        discard;
 
    if (true == In.bNormal)
    {
        int protoNormalSlice = In.ProtoIndex - In.TexIndex - 1;
        
        float3 nTS = g_DecalArray.Sample(NoMipSampler, float3(uv, protoNormalSlice)).xyz * 2 - 1;
        nTS.z = sqrt(saturate(1 - dot(nTS.xy, nTS.xy)));
        float3 decalNormal = normalize(mul(nTS, TBN));
        float3 blendedNormal = normalize(lerp(surfNormal, decalNormal, decal.a));
        Out.vNormal = float4(decalNormal * 0.5f + 0.5f, 0.f);
    }
    else
      Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
    
    float4 finalColor = ComputeDecalEffect(decal.rgb, In.DecalTime, In.LifeTime);

    Out.vDecal = float4(finalColor.rgb, decal.a * finalColor.a);

    return Out;
}


technique11 DecalTech
{
    pass Decal 
    {
        SetRasterizerState(RS_Decal);
        SetDepthStencilState(DSS_DefaultNoWrite, 0);
        SetBlendState(BS_Decal, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_Decal();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Decal();
    }
}
