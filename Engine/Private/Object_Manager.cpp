#include "Object_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "UI_Manager.h"
CObject_Manager::CObject_Manager() 
{
	
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

    m_pLayers = new map<const _wstring, class CLayer*>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;
    
	WRITE_LOCK;
	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
    if (m_pPlayer)
        m_pPlayer->Priority_Update(fTimeDelta);

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
			{
			  Pair.second->Priority_Update(fTimeDelta);
			}
		}
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
    if (m_pPlayer)
        m_pPlayer->Update(fTimeDelta);

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
            {       
     		  Pair.second->Update(fTimeDelta);
			}
		}
	}
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
    if (m_pPlayer)
        m_pPlayer->Late_Update(fTimeDelta);

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{      
			if (nullptr != Pair.second)
            {       
				Pair.second->Late_Update(fTimeDelta);
			}
		}
	}
}

void CObject_Manager::Delete()
{
    for (size_t i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pLayers[i])
        {
            if (nullptr != Pair.second)
            {
                Pair.second->Delete();
            }
        }
    }
}

void CObject_Manager::Clear(_uint iClearLevelID)
{
	if (iClearLevelID >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLayers[iClearLevelID])
	{
		Safe_Release(Pair.second);
	}
	m_pLayers[iClearLevelID].clear();
}

HRESULT CObject_Manager::Add_Clon_to_Layers(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* clone)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(clone);
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else 
		pLayer->Add_GameObject(clone);

	return S_OK;
}

CGameObject* CObject_Manager::Find_Prototype(const _wstring& strPrototypeTag)
{
	auto iter = m_Prototypes.find(strPrototypeTag);

	if(iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CGameObject* CObject_Manager::Find_CloneGameObject(_uint iLevelIndex, const _wstring& strLayerTag, const _uint& ProtoTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	
	if (nullptr == pLayer)
		return nullptr;
	  
	return pLayer->Get_GameObject(ProtoTag);
}

CGameObject* CObject_Manager::Clone_Prototype(const _wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

void CObject_Manager::Set_Player(const _wstring& ProtoTag, void* pArg)
{
 m_pPlayer = Clone_Prototype(ProtoTag, pArg);
}

_bool CObject_Manager::IsGameObject(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		return nullptr;
	} 

	return pLayer->IsGameObject();
}

CGameObject::PICKEDOBJ_DESC CObject_Manager::Pking_onMash(const _uint& Level, const _wstring& Layer, _vector RayPos, _vector RayDir)
{
    CLayer* pLayer = Find_Layer(Level, Layer);

	if (nullptr == pLayer)
    {
            return {};
    } 
	return pLayer->Pking_onMash(RayPos, RayDir);
			
}

CGameObject* CObject_Manager::Recent_GameObject(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		MSG_BOX("못 가져옴");
	return  pLayer->Recent_GameObject();
}

list<class CGameObject*> CObject_Manager::Get_ALL_GameObject(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		MSG_BOX("못 가져옴");

	return  pLayer->Get_GameObject_List();
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	if(iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	__super::Free();

    Safe_Release(m_pPlayer);

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
                Safe_Release(Pair.second);
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
 	m_Prototypes.clear();
}


