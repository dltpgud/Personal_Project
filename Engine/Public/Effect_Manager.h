#pragma once
#include "Base.h"
#include "EffectStream.h"
#include "Effect_TrailStream.h"
#include "Effect_DecalStream.h"
BEGIN(Engine)

class CEffect_Manager final : public CBase
{
private:
    explicit CEffect_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEffect_Manager() = default;

public:
    void Update(_float fTimeDelta);
    HRESULT Render_All(class CShader* pShader = nullptr);
    HRESULT Render_Decal(class CShader* pShader = nullptr);
   
    HRESULT Add_EffectStream(const _wstring& key, CEffectStream* pStream);
    HRESULT Trigger_Effect(const _wstring& streamKey, void* pSpawnDesc, _float fTimeDelta = 0.f);
    CEffectStream* Find_EffectStream(const _wstring& key);

private:
    HRESULT Initialize();
public:
    static CEffect_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    class CGameInstance* m_pGameInstance = nullptr;

    unordered_map<_wstring, CEffectStream*> m_EffectStreams;

};

END
