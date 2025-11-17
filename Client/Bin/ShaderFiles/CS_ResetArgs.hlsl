#include "Engine_Shader_Defines.hlsli"

RWByteAddressBuffer g_DrawArgs : register(u0);

cbuffer CB_RESET_ARGS : register(b0)
{
    uint g_IndexCount; 
};

[numthreads(1, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    g_DrawArgs.Store(0, g_IndexCount);

    g_DrawArgs.Store(4, 0);

    g_DrawArgs.Store(8, 0);

    g_DrawArgs.Store(12, 0);

    g_DrawArgs.Store(16, 0);
}
