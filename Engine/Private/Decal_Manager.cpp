#include "Decal_Manager.h"
#include "Decal.h"

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

HRESULT CDecal_Manager::Add_Decal(const wstring& Key, const DECAL_DESC& DecalDesc)
{
    auto iter = m_ProtoDecal_Map.find(Key);

    if (iter == m_ProtoDecal_Map.end())
        return S_OK;


    CDecal* pDecal = ObjectPool<CDecal>::Pop(*iter->second,DecalDesc);
  //  if (!pDecal)
  //      return E_FAIL;



   // CDecal* pDecal = iter->second->Clone(DecalDesc);
    
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
    for (auto& pDecals : m_Decals) Safe_Release(pDecals);
	m_Decals.clear();

	return S_OK;
}

HRESULT CDecal_Manager::Delete()
{
    for (auto iter = m_Decals.begin(); iter != m_Decals.end();)
    {
        if ((*iter) && true == (*iter)->Get_Dead())
        {
            ObjectPool<CDecal>::Push(*iter);
            iter = m_Decals.erase(iter);
        }
        else
            iter++;
    }
    return S_OK ;
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

	for (auto pDecals : m_Decals) Safe_Release(pDecals);
	    m_Decals.clear();

	for (auto pDecals : m_ProtoDecal_Map) Safe_Release(pDecals.second);
        m_ProtoDecal_Map.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
