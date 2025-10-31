#pragma once
#include "EffectStream.h"

struct GPU_DecalHeader
{
    _float3 Pos;       // 0 ~ 11
    _float Life;       // 12 ~ 15
    _float3 Dir;       // 16 ~ 27
    _float DecalTime;  // 28 ~ 31
    _float3 Size;      // 32 ~ 43
    _float DeltaScale; // 44 ~ 47
    _int TexIndex;     // 48 ~ 51
    _int Type;         // 52 ~ 55
    _int Active;       // 56 ~ 59
    _int bNormal;
};

struct CB_DECAL_FRAME
{
    float g_DeltaTime;
    UINT g_SpawnCount;
    UINT g_MaxDecals;
    float pad;
};

__declspec(align(16)) struct DECAL_SPAWN_REQ
{
    _float3 Pos;
    _uint Valid; // 16바이트 (0 ~ 15)

    _float3 Dir;
    _float Size; // 16바이트 (16 ~ 31)

    _float Depth;
    _float LifeTime; // 8바이트  (32 ~ 39)
    _float DeltaScale;
    _float PadA; // 8바이트  (40 ~ 47)
    // 여기까지 총 48바이트

    _int TexIndex;  // 4바이트  (48 ~ 51)
    _int DecalType; // 4바이트  (52 ~ 55)
    _int bNormal;   // 4바이트  (56 ~ 59)
    _int PadB;      // 4바이트  (60 ~ 63)
    _float4x4 WorldInv;
    // 총 128바이트
};

struct CONTINUOUS_STATE
{
    _vector LastPos;
    _float LastTime;
    _bool Initialized = false;
};

__declspec(align(16)) struct GPU_DecalInstanceData
{
    DirectX::XMFLOAT4X4 WorldInv; // row_major float4x4

    DirectX::XMFLOAT3 DecalPos;
    float _padA;

    DirectX::XMFLOAT3 DecalDir;
    float _padB;

    DirectX::XMFLOAT3 HalfSize;
    float LifeTime;

    float DecalTime;
    int TexIndex;
    int DecalType;
    int bNormal;
};

BEGIN(Engine)
class ENGINE_DLL CEffect_DecalStream final : public CEffectStream
{
public:
    struct DECALSTREAM_DESC
    {
        UINT MaxDecals = 0;        // 총 슬롯 개수
        UINT MaxSpawnPerFrame = 0; // 프레임당 최대 스폰 수

        _wstring FilePathFmt;   // 예: L"Textures/Decal_ProtoA_%d.dds"
        _uint TextureCount = 0; // 이 프로토의 슬라이스 개수
    };

    explicit CEffect_DecalStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEffect_DecalStream() = default;

public:
    static CEffect_DecalStream* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);

    // 기본 인터페이스
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update(_float fTimeDelta) override;
    virtual HRESULT Render(class CShader* pShader) override;
    virtual HRESULT Trigger_Effect(void* pArg, _float fTimeDelta) override;
    virtual void Free() override;

private:
    // 초기화용 내부 함수
    HRESULT createGeometryBuffers();
    HRESULT createGPUStorageBuffers();
    HRESULT createSpawnUploadBuffer();
    HRESULT createIndirectArgsBuffer();
    HRESULT createComputeShaders();
    HRESULT createCB();

    // 실행 로직
    HRESULT uploadSpawnRequestsToGPU();
    HRESULT dispatchSpawnUpdateCS(float dt);
    HRESULT dispatchBuildDrawCS();
    void resetDrawArgsOnCPU();
#ifdef _DEBUG
    void DebugGPUState();
#endif // _DEBUG
   HRESULT BuildGlobalDecalArray(_wstring FilePathFmt, _uint TextureCount);

private:
    // GPU 리소스
    ID3D11ComputeShader* m_pCS_SpawnUpdate = nullptr;
    ID3D11ComputeShader* m_pCS_BuildDrawData = nullptr;

    ID3D11Buffer* m_pVB_Cube = nullptr;
    ID3D11Buffer* m_pIB_Cube = nullptr;
    UINT m_iIndexCount = 36;

    ID3D11Buffer* m_pDecalSlots = nullptr;
    ID3D11ShaderResourceView* m_pDecalSlotsSRV = nullptr;
    ID3D11UnorderedAccessView* m_pDecalSlotsUAV = nullptr;

    ID3D11Buffer* m_pLiveList = nullptr;
    ID3D11ShaderResourceView* m_pLiveListSRV = nullptr;
    ID3D11UnorderedAccessView* m_pLiveListUAV = nullptr;

    ID3D11Buffer* m_pInstanceBuffer = nullptr;
    ID3D11ShaderResourceView* m_pInstanceSRV = nullptr;
    ID3D11UnorderedAccessView* m_pInstanceUAV = nullptr;

    ID3D11Buffer* m_pSpawnUpload = nullptr;
    ID3D11ShaderResourceView* m_pSpawnUploadSRV = nullptr;

    ID3D11Buffer* m_pIndirectArgs = nullptr;
    ID3D11UnorderedAccessView* m_pIndirectArgsUAV = nullptr;

    ID3D11ShaderResourceView* m_pDecalArraySRV = nullptr;
    ID3D11Buffer* m_pCB_DecalFrame = nullptr;

private:
    UINT m_MaxDecals = 0;
    UINT m_MaxSpawnPerFrame = 0;
    const _float fkMinDistance = 0.5; // 최소 거리 50cm
    const _float kCooldownMs = 0.03f;
    _float m_TotalTime = 0.f;
    unordered_map<_uint, CONTINUOUS_STATE> m_ContinuousMap;
    UINT THREADS = 256;

    struct CPU_DECAL_REQUEST
    {
        DECAL_SPAWN_REQ Req;
    };
    vector<CPU_DECAL_REQUEST> m_SpawnQueue;
};
END