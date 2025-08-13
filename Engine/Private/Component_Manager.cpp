#include "Component_Manager.h"

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Initialize(_uint iNumLevels)
{
    if (nullptr != m_pPrototypes)
        return E_FAIL;

    m_pPrototypes = new PROTOTYPES[iNumLevels];

    m_iNumLevels = iNumLevels;

    return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
    /*인자로 받아온 레벨 인덱스가 할당된 맴버보다 크거나 이미 키값이 정해져 있으면 리턴 시켜버린다.*/
    if (iLevelIndex >= m_iNumLevels || nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
        return E_FAIL;
   // WRITE_LOCK;
    m_Component_Mutex.lock();
    m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);
    m_Component_Mutex.unlock();
    return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
    if (iLevelIndex >= m_iNumLevels)
        return nullptr;

    CComponent* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
    if (nullptr == pPrototype)
        return nullptr;

    CComponent* pComponent = pPrototype->Clone(pArg);
    if (nullptr == pComponent)
        return nullptr;

    return pComponent;
}

void CComponent_Manager::Clear(_uint iLevelindex)
{

    if (iLevelindex >= m_iNumLevels)
        return;

    for (auto& Pair : m_pPrototypes[iLevelindex]) Safe_Release(Pair.second);

    m_pPrototypes[iLevelindex].clear();
}

map<const _wstring, class CComponent*> CComponent_Manager::Get_Com_proto_vec(_uint iLevelindex)
{
    return m_pPrototypes[iLevelindex];
}

CComponent* CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
    auto iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

    if (iter == m_pPrototypes[iLevelIndex].end())
        return nullptr;

    return iter->second;
}

CComponent_Manager* CComponent_Manager::Create(_uint iNumLevels)
{
    CComponent_Manager* pInstance = new CComponent_Manager();

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Created : CComponent_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CComponent_Manager::Free()
{
    __super::Free();

    for (size_t i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pPrototypes[i]) Safe_Release(Pair.second);
        m_pPrototypes[i].clear();
    }

    Safe_Delete_Array(m_pPrototypes);
}
