#pragma once
#include "EffectStream.h"

BEGIN(Engine)

class ENGINE_DLL CEffect_SparkStream final : public CEffectStream
{
public:
    struct DESC
    {
        _uint maxParticles = 512;
        _float fadeSpeed = 1.0f;
        _float lifeTime = 0.8f;
        const _tchar* pTexturePath = nullptr;
        _uint iSpriteCount = 4;
    };

    struct SPAWN_REQUEST
    {
        _uint valid;
        _float3 pos;
        _float3 dir;
        _float speed;
        _float life;
        _float4 color;
    };

private:
     CEffect_SparkStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEffect_SparkStream() = default;

public:
    static CEffect_SparkStream* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pDesc);
    virtual void Free() override;

    HRESULT Initialize(void* pArg) override;
    void Update(_float fTimeDelta) override;
    HRESULT Render(CShader* pShader) override;
    HRESULT Trigger_Effect(void* pArg) override;

private:
    HRESULT createShaders();
    HRESULT createBuffers();
    HRESULT createCB();

private:
    DESC m_desc{};
    vector<SPAWN_REQUEST> m_spawnQueue;

    ID3D11Buffer* m_pParticleBuf = nullptr;
    ID3D11UnorderedAccessView* m_UAV_Particle = nullptr;
    ID3D11ShaderResourceView* m_SRV_Particle = nullptr;

    ID3D11Buffer* m_pCSCB = nullptr;
    ID3D11Buffer* m_pVSCB = nullptr;

    ID3D11ComputeShader* m_pCS_Update = nullptr;
    CShader* m_pShader = nullptr;
    class CTexture* m_pTexture = nullptr;

    _uint m_iSpriteCount{};
    _float m_fAccumTime = 0.f;
    _bool m_bPing = false;
};
END
