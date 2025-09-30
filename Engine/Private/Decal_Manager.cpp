#include "Decal_Manager.h"
#include "Decal.h"
#include "ObjectPool.h"

CDecal_Manager::CDecal_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{pDevice}, m_pContext{pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CDecal_Manager::Initialize()
{
    return S_OK;
}

HRESULT CDecal_Manager::Add_DecalProto(const wstring& Key, const _tchar* FilePath, const _uint& TexNum)
{
    CDecal* pDecals = CDecal::Create(m_pDevice, m_pContext, FilePath, TexNum);

    if (nullptr == pDecals)
        return E_FAIL;
    
    m_ProtoDecal_Map.emplace(Key, pDecals);
    return S_OK;
}

HRESULT CDecal_Manager::Add_Decal(const wstring& Key, const DECAL_DESC* DecalDesc)
{
    auto iter = m_ProtoDecal_Map.find(Key);

    if (iter == m_ProtoDecal_Map.end())
        return S_OK;
    
    CDecal* pDecal = ObjectPool<CDecal>::Pop(*iter->second, const_cast<DECAL_DESC*>(DecalDesc));

    m_Decals.push_back(pDecal);

    return S_OK;
}

HRESULT CDecal_Manager::Update(_float fTimeDelta)
{
    for (auto& pDecals : m_Decals)
    {
        if (pDecals)
            pDecals->Update(fTimeDelta);
    }

    return S_OK;
}

HRESULT CDecal_Manager::Render(CShader* pShader)
{
    for (auto& pDecals : m_Decals)
    {
        if (pDecals)
            pDecals->Render(pShader);
    }
    return S_OK;
}

HRESULT CDecal_Manager::Clear()
{
    for (auto& obj : m_Decals)
    {
        if (!obj)
            continue;

        const auto state = obj->Get_Dead();
        if (state == OBJ_POOL)
            ObjectPool<CDecal>::Push(obj); // 풀로
        else
            Safe_Release(obj); // 완전 해제
    }
    m_Decals.clear();
    return S_OK;
}

HRESULT CDecal_Manager::Delete()
{
    for (auto it = m_Decals.begin(); it != m_Decals.end();)
    {
        CDecal* obj = *it;
        if (!obj)
        {
            it = m_Decals.erase(it);
            continue;
        }

        const auto state = obj->Get_Dead();

        if (state == OBJ_POOL)
        {
          
            ObjectPool<CDecal>::Push(obj);
            it = m_Decals.erase(it);
        }
        else if (state == OBJ_DEAD)
        {
            Safe_Release(obj);
            it = m_Decals.erase(it);
        }
        else
        {
            ++it; 
        }
    }
    return S_OK;
}

CDecal_Manager* CDecal_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDecal_Manager* pInstance = new CDecal_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CDecal_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDecal_Manager::Free()
{
    __super::Free();

    ObjectPool<CDecal>::ClearAll();

    for (auto& obj : m_Decals) Safe_Release(obj);
    m_Decals.clear();

    for (auto& kv : m_ProtoDecal_Map) { Safe_Release(kv.second); };

    m_ProtoDecal_Map.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
