#include "Engine_Shader_Defines.hlsli"

//===================================================================
// CPU에서 GPU_DecalHeader를 쓴 버퍼와 동일한 레이아웃이어야 함
//===================================================================
struct DecalHeader
{
    float3 Pos;
    float Life; // 총수명

    float3 Dir;
    float DecalTime; // 경과시간

    float3 Size; // (sizeX,sizeY,depth) = (Size,Size,Depth)
    float DeltaScale;

    int TexIndex;
    int Type;
    int Active;
    int bNormal;
};

struct DecalInstanceData
{
    row_major float4x4 WorldInv; // 64바이트
    float3 DecalPos; // 12
    float _padA; // 4  -> align 16

    float3 DecalDir; // 12
    float _padB; // 4

    float3 HalfSize; // 12
    float LifeTime; // 4

    float DecalTime; // 4
    int TexIndex; // 4
    int DecalType; // 4
    int bNormal; // 4
};


//===================================================================
// 리소스
//===================================================================
// t0 : g_LiveList = [count, slotIdx0, slotIdx1, ...]
StructuredBuffer<uint> g_LiveList : register(t0);

// t1 : g_Decals 전체 슬롯
StructuredBuffer<DecalHeader> g_Decals : register(t1);

// u0 : 최종 인스턴스 데이터 (VS에서 StructuredBuffer로 읽음)
RWStructuredBuffer<DecalInstanceData> g_InstanceOut : register(u0);

// u1 : DrawIndirectArgs. 우리가 InstanceCount만 늘린다.
//      DrawIndexedInstancedIndirect 인자 레이아웃:
//      [0] IndexCountPerInstance
//      [1] InstanceCount
//      [2] StartIndexLocation
//      [3] BaseVertexLocation
//      [4] StartInstanceLocation
//
// 여기서는 DX11 호환성 고려해서 ByteAddressBuffer 유지하되,
// InstanceCount는 바이트 오프셋 4에 위치.
RWByteAddressBuffer g_DrawArgs : register(u1);


//===================================================================
// 행렬 보조 함수
//===================================================================
float4x4 InverseMatrix(float4x4 m)
{
    float4x4 r;
    float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3];
    float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3];
    float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3];
    float a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3];

    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;

    float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    float invDet = 1.0 / det;

    r[0][0] = (a11 * b11 - a12 * b10 + a13 * b09) * invDet;
    r[0][1] = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
    r[0][2] = (a31 * b05 - a32 * b04 + a33 * b03) * invDet;
    r[0][3] = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;

    r[1][0] = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
    r[1][1] = (a00 * b11 - a02 * b08 + a03 * b07) * invDet;
    r[1][2] = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
    r[1][3] = (a20 * b05 - a22 * b02 + a23 * b01) * invDet;

    r[2][0] = (a10 * b10 - a11 * b08 + a13 * b06) * invDet;
    r[2][1] = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
    r[2][2] = (a30 * b04 - a31 * b02 + a33 * b00) * invDet;
    r[2][3] = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;

    r[3][0] = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
    r[3][1] = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
    r[3][2] = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
    r[3][3] = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;

    return r;
}

float4x4 ComputeWorldInv(float3 pos, float3 dir, float size, float depth)
{
    float3 N = normalize(dir);
    float3 up = (abs(N.y) < 0.999f) ? float3(0, 1, 0) : float3(1, 0, 0);
    float3 T = normalize(cross(up, N));
    float3 B = normalize(cross(N, T));
    pos += N * 0.001f;
    // size는 XY로, depth는 Z 쪽 스케일(=decal range)
    float4x4 world =
    {
        float4(T * size, 0),
        float4(B * size, 0),
        float4(N * depth, 0),
        float4(pos, 1)
    };

    return InverseMatrix(world);
}

//===================================================================
// Main
//===================================================================
[numthreads(256, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    uint tid = id.x;

    // 살아있는 데칼 개수 읽기
    uint totalAlive = g_LiveList[0]; // g_LiveList[0] = count
    if (tid >= totalAlive)
        return;

    // 내가 처리할 데칼 슬롯 인덱스
    uint decalSlot = g_LiveList[tid + 1];

    DecalHeader DH = g_Decals[decalSlot];
    if (DH.Active == 0)
        return; // 안전 체크 (안전망)

    // DrawArgs[1] (= InstanceCount)에 1 추가하고
    // 이전 값을 instanceIdx로 받는다.
    uint instanceIdx;
    // R32_UINT 배열 (uint[5])처럼 쓰고 있음:
    // offset 4바이트 위치가 InstanceCount
    g_DrawArgs.InterlockedAdd(4 /*byte offset*/, 1, instanceIdx);

    // 인스턴스 데이터 채우기
    DecalInstanceData inst = (DecalInstanceData) 0;
    inst.WorldInv = ComputeWorldInv(DH.Pos, DH.Dir, DH.Size.x, DH.Size.z);
    inst.DecalPos = DH.Pos;
    inst.DecalDir = DH.Dir;
    inst.HalfSize = float3(DH.Size.x, DH.Size.y, DH.Size.z * 0.5f); // 원한다면 z만 절반 등 자유롭게
    inst.LifeTime = DH.Life;
    inst.DecalTime = DH.DecalTime;
    inst.TexIndex = DH.TexIndex;
    inst.bNormal = DH.bNormal;
    inst.DecalType = DH.Type;

    g_InstanceOut[instanceIdx] = inst;
}
