cbuffer SkinCB : register(b0)
{
    uint g_NumVerts;   float3 _pad0; // 16B
    float4x4 g_World; // 64B
}

StructuredBuffer<float3> g_BindPos : register(t0); // 바인드포즈 위치
StructuredBuffer<uint4> g_BoneIdx : register(t1); // 각 정점의 본 인덱스(최대 4)
StructuredBuffer<float4> g_BoneWgt : register(t2); // 각 정점의 본 가중치(최대 4)
StructuredBuffer<float4x4> g_BoneMats : register(t3); // 본 행렬(스키닝용), 보통 팔레트

RWStructuredBuffer<float3> g_OutWorldPos : register(u0); // 결과: 월드 좌표 포지션

[numthreads(128, 1, 1)]
void CS_Skin(uint3 tid : SV_DispatchThreadID)
{
    uint i = tid.x;
    if (i >= g_NumVerts)
        return;

    uint4 bi = g_BoneIdx[i];
    float4 bw = g_BoneWgt[i];

    // 선형 블렌드 스키닝(4본)
    float4x4 S =
        g_BoneMats[bi.x] * bw.x +
        g_BoneMats[bi.y] * bw.y +
        g_BoneMats[bi.z] * bw.z +
        g_BoneMats[bi.w] * bw.w;

    float4 pL = float4(g_BindPos[i], 1);
    float4 pW = mul(mul(pL, S), g_World); // 본 변환 후 오브젝트 월드 적용
    g_OutWorldPos[i] = pW.xyz;
}
