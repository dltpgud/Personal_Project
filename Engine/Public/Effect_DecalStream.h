#pragma once
#include "EffectStream.h"

BEGIN(Engine)
class ENGINE_DLL CEffect_DecalStream final : public CEffectStream
{
public:
    struct DECALSTREAM_DESC
    {
        UINT MaxDecals = 0;       
        UINT MaxSpawnPerFrame = 0; 
        _wstring FilePathFmt;   
        _uint TextureCount = 0;
    };

    explicit CEffect_DecalStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEffect_DecalStream() = default;

public:
    static CEffect_DecalStream* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);

    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update(_float fTimeDelta) override;
    virtual HRESULT Render(class CShader* pShader) override;
    virtual HRESULT Trigger_Effect(void* pArg, _float fTimeDelta) override;
    virtual void Free() override;

private:

    HRESULT createGPUStorageBuffers();
    HRESULT createSpawnUploadBuffer();

    HRESULT createCB();

    UINT UploadSpawnRequestsToGPU();
    HRESULT DispatchSpawnUpdateCS(float dt, UINT spawnCount);
    HRESULT DispatchBuildDrawCS();
    void    ResetDrawArgsOnCPU();
    void ClearLiveList_OnGPU();
   HRESULT BuildGlobalDecalArray(_wstring FilePathFmt, _uint TextureCount);

private:
   struct GPU_DecalHeader
   {
       _float3 Pos;
       _float Life;
       _float3 Dir;
       _float DecalTime;
       _float3 Size;
       _float DeltaScale;
       _int TexIndex;
       _int Type;
       _int Active;
       _int bNormal;
   };

   struct CB_DECAL_FRAME
   {
       _float g_DeltaTime;
       _uint g_SpawnCount;
       _uint g_MaxDecals;
       _float pad;
   };

   struct DECAL_SPAWN_REQ
   {
       _float3 Pos;
       _uint Valid;
       _float3 Dir;
       _float Size;
       _float Depth;
       _float LifeTime;
       _float DeltaScale;
       _float PadA;
       _int TexIndex;
       _int bNormal;
       _int PadB;

   };

   struct CONTINUOUS_STATE
   {
       _vector LastPos;
       _float LastTime;
       _bool Initialized = false;
   };

   struct GPU_DecalInstanceData
   {
       _float4x4 WorldInv; // row_major float4x4
       _float _padA;
       _float3 DecalDir;
       _float _padB;
       _float LifeTime;
       _float DecalTime;
       _int TexIndex;
       _int bNormal;
   };

private:
    ID3D11ComputeShader* m_pCS_SpawnUpdate = nullptr;
    ID3D11ComputeShader* m_pCS_BuildDrawData = nullptr;
    ID3D11ComputeShader* m_pCS_ClearLiveList = nullptr;
    ID3D11ComputeShader*  m_pCS_ResetArgs = nullptr;
    class CVIBuffer_Cube* m_pVIBuffer_Cube = nullptr;

    ID3D11Buffer* m_pDecalSlots = nullptr;
    ID3D11ShaderResourceView* m_pDecalSlotsSRV = nullptr;
    ID3D11UnorderedAccessView* m_pDecalSlotsUAV = nullptr;

    ID3D11Buffer* m_pLiveList = nullptr;
    ID3D11ShaderResourceView* m_pLiveListSRV = nullptr;
    ID3D11UnorderedAccessView* m_pLiveListUAV = nullptr;

    ID3D11Buffer* m_pInstanceBuffer[2] = {nullptr};
    ID3D11ShaderResourceView* m_pInstanceSRV[2] = {nullptr};
    ID3D11UnorderedAccessView* m_pInstanceUAV[2] = {nullptr};

    ID3D11Buffer* m_pSpawnUpload = nullptr;
    ID3D11ShaderResourceView* m_pSpawnUploadSRV = nullptr;

    ID3D11Buffer* m_pIndirectArgs[2] = {nullptr};
    ID3D11UnorderedAccessView* m_pIndirectArgsUAV[2] = {nullptr};

    ID3D11ShaderResourceView* m_pDecalArraySRV = nullptr;
    ID3D11Buffer* m_pCB_DecalFrame = nullptr;
    ID3D11Buffer* m_pCB_ResetArgs = nullptr;



private:
    UINT m_MaxDecals = 0;
    UINT m_MaxSpawnPerFrame = 0;
    const _float fkMinDistance = 0.5f;
    const _float kCooldownMs = 0.03f;
    _float m_TotalTime = 0.f;
    unordered_map<_uint, CONTINUOUS_STATE> m_ContinuousMap;
    UINT THREADS = 256;
    UINT m_FrameIndex = 0;
    UINT writeIdx{};
    UINT lastIndexCount = 0;

    const UINT kSpawnRingSize = 4096;
    DECAL_SPAWN_REQ m_SpawnRing[4096];
    std::atomic<UINT> m_SpawnWrite{0};
    std::atomic<UINT> m_SpawnRead{0};


};
END