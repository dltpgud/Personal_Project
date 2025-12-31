#include "Effect_Manager.h"
#include "GameInstance.h"

USING(Engine)

CEffect_Manager::CEffect_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Manager::Initialize()
{
    return S_OK;
}

void CEffect_Manager::Update(_float fTimeDelta)
{
    for (auto it = m_EffectStreams.begin(); it != m_EffectStreams.end(); ++it)
    {
        if (it->second)
            it->second->Update(fTimeDelta);
    }
}

HRESULT CEffect_Manager::Render_All(CShader* pShader)
{
    for (auto it = m_EffectStreams.begin(); it != m_EffectStreams.end(); ++it)
    {
        if (it->second && nullptr == dynamic_cast<CEffect_DecalStream*>(it->second))
            it->second->Render(pShader);
    }
    return S_OK;
}

HRESULT CEffect_Manager::Render_Decal(CShader* pShader)
{
    for (auto it = m_EffectStreams.begin(); it != m_EffectStreams.end(); ++it)
    {
       if (it->second && nullptr != dynamic_cast<CEffect_DecalStream*>(it->second))
           it->second->Render(pShader);
    }
    return S_OK;
}

HRESULT CEffect_Manager::Add_EffectStream(const _wstring& key, CEffectStream* pStream)
{
    if (pStream == nullptr)
        return E_FAIL;

    auto it = m_EffectStreams.find(key);
    if (it != m_EffectStreams.end())
    {
        Safe_Release(pStream);
        return S_OK;
    }

    m_EffectStreams.emplace(key, pStream);
    return S_OK;
}


CEffectStream* CEffect_Manager::Find_EffectStream(const _wstring& key)
{
    auto iter = m_EffectStreams.find(key);
    if (iter == m_EffectStreams.end())
        return nullptr;

    return iter->second;
}

HRESULT CEffect_Manager::Trigger_Effect(const _wstring& streamKey, void* pSpawnDesc, _float fTimeDelta)
{
    CEffectStream* pStream = Find_EffectStream(streamKey);
    if (pStream == nullptr)
        return E_FAIL;

    return pStream->Trigger_Effect(pSpawnDesc, fTimeDelta);
}

CEffect_Manager* CEffect_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_Manager* pInstance = new CEffect_Manager(pDevice, pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create : CEffect_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEffect_Manager::Free()
{
    for (auto iter : m_EffectStreams) 
        Safe_Release(iter.second);
    m_EffectStreams.clear();

     Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
