
#include "Engine_Shader_Defines.hlsli"

RWStructuredBuffer<uint> g_LiveList : register(u0);

[numthreads(1, 1, 1)]
void CSMain(uint3 dispatchId : SV_DispatchThreadID)
{
    g_LiveList[0] = 0;
    g_LiveList[100] = 0;
}


