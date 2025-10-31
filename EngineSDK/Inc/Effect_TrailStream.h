#pragma once
#include "EffectStream.h"
#include <vector>

BEGIN(Engine)

class ENGINE_DLL CEffect_TrailStream final : public CEffectStream
{
public:
    enum RenderPass
    {
        RP_SPRITE = 0u,
        RP_CURVE = 1u,
        RP_TEX = 2u
    };
    enum RenderMode
    {
        RM_DEFULT = 0u,
        RM_CURVE = 1u,
    };
    struct TRAILSDESC
    {
        _uint maxTrails = 128;
        _uint maxPointsPerTrail = 64;
        _float fadeSpeed = 1.0f;
        _float lifeTime = 2.0f;
        _uint iPass = 2;
        _uint Mode{};
        const _tchar* pTrailTexturePath = nullptr;
        _uint iTextureNum = 1;
        _uint iTotalSprite = 0;
        _float2 vTrailTexUVScale = {1.f, 1.f};
    };

 struct alignas(16) SPAWN_REQUEST
    {
        _uint valid;      // 4
        _uint trailIndex; // 4
        _float3 headPos;  // 12
        _float addLife;   // 4  -> 여기까지 24 bytes, 다음은 16배수 단위라 alignas 필요X
        _float width;     // 4
        _float3 _pad0;    // ✅ 패딩 (float3 == 12 bytes → width 포함해서 16 align 맞춤)

        _float4 color; // 16
        _uint frameIndex; // 4
        _uint isSegment;  // 4
        _float3 tailPos;  // 12
        _uint generation;
    };

private:
    explicit CEffect_TrailStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEffect_TrailStream() = default;

public:
    static CEffect_TrailStream* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pDesc);
    virtual void Free() override;

    HRESULT Initialize(void* pArg) override;
    void Update(_float dt) override;
    HRESULT Render(class CShader* pShader) override;
    HRESULT Trigger_Effect(void* pArg, _float fTimeDelta) override;

    int AllocateTrail();
    void ReleaseTrail(int indx);
private:
    HRESULT createShaders();
    HRESULT createBuffers();
    HRESULT createPerFrameCB();
    HRESULT createIndirectArgs();
    void pingpong();
    //==============================
    // 내부 구조체 (HLSL과 동일)
    //==============================
    struct TrailPoint
    {
        _float3 pos;
        _float life;
        _float4 color;
    };

    struct TrailHeader
    {
        _float width;
        _float3 _pad0;
        _uint active;
        _uint head;
        _uint count;
        _uint first;
        _uint frameIndex;
        _uint generation;
        _uint _pad1[2]; 
    };

    struct CS_PERFRAME_CS
    {
        float dt;
        float fadeSpeed;
        float lifeTime;
        UINT maxTrails;

        UINT maxPointsPerTrail;
        UINT spawnCount;
        UINT mode;
        float minStepDist;

        UINT maxStitch;
        float padCS[3]; // 🔸 남은 12바이트를 채워 총 64바이트(16배수)
    };

    struct CS_PERFRAME_INTERP
    {
        UINT mode;
        UINT maxTrails;
        UINT maxPointsPerTrail;
        float lifeTime;
        DirectX::XMFLOAT3 g_CamPosWS;
        float _pad0;
    };

    struct VS_PERFRAME
    {
        _float4x4 g_View;
        _float4x4 g_Proj;
        _float3 g_CamPosWS;
        _float _pad0;
        _float2 g_TrailUVScale;
        _float2 _pad1;
        _uint g_TotalFrames;
        _float3 _pad2;
    };

private:
    //==============================
    // 멤버 변수
    //==============================
    TRAILSDESC m_desc{};
    vector<SPAWN_REQUEST> m_spawnQueue;
    vector<UINT> m_inUse;
    UINT m_NextTrailID = 0;              // 다음에 할당될 trail index (0~maxTrails-1 순환)
    std::vector<UINT> m_GenerationTable; // 각 인덱스의 generation 추적용 (CPU 캐시)

    // Trail Buffers
    ID3D11Buffer* m_pTrailBufA = nullptr;
    ID3D11Buffer* m_pTrailBufB = nullptr;
    ID3D11ShaderResourceView* m_SRV_A = nullptr;
    ID3D11ShaderResourceView* m_SRV_B = nullptr;
    ID3D11UnorderedAccessView* m_UAV_A = nullptr;
    ID3D11UnorderedAccessView* m_UAV_B = nullptr;

    ID3D11Buffer* m_pTrailHeader = nullptr;
    ID3D11ShaderResourceView* m_SRV_Header = nullptr;
    ID3D11UnorderedAccessView* m_UAV_Header = nullptr;

    ID3D11Buffer* m_pSpawnUpload = nullptr;
    ID3D11ShaderResourceView* m_SRV_SpawnUpload = nullptr;

    ID3D11Buffer* m_pCSPerFrame = nullptr;
    ID3D11Buffer* m_pVSPerFrame = nullptr;
    ID3D11Buffer* m_pCSPerFrame_Interp = nullptr;
 

    ID3D11Buffer* m_pTrailVertex = nullptr;
    ID3D11UnorderedAccessView* m_UAV_TrailVertex = nullptr;
    ID3D11Buffer* m_pIndirectArgs = nullptr;
    ID3D11UnorderedAccessView* m_UAV_IndirectArgs = nullptr;

    CShader* m_pShader = nullptr;
    ID3D11ComputeShader* m_pCS_Update = nullptr;
    ID3D11ComputeShader* m_pCS_Interp = nullptr;
    class CTexture* m_pTrailTexCom = nullptr;

    _float2 m_vTrailTexUVScale{1.f, 1.f};
    _uint m_iPass{};
    _int m_iTexTotalFrames{};
    _bool m_AasInput = false;
    _int m_iMode{};


};

END
