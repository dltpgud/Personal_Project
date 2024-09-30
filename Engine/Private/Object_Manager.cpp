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

	/*원하는 레벨의 레이어를 관리하기 위해서 레이어를 동적 배열로 할당한다.*/
	m_pLayers = new map< const  _uint , class CLayer*>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
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

CGameObject* CObject_Manager::Find_Prototype(const _wstring& strPrototypeTag)
{
	auto iter = m_Prototypes.find(strPrototypeTag);

	if(iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}



_bool CObject_Manager::IsGameObject(_uint iLevelIndex, const _uint& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		return nullptr;
	} 

	return pLayer->IsGameObject(strLayerTag);
}

CGameObject::PICKEDOBJ_DESC CObject_Manager::Pking_onMash(_vector RayPos, _vector RayDir)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
			{
				return	Pair.second->Pking_onMash(RayPos, RayDir);
			}
		}
	}
	return{};
}

CGameObject* CObject_Manager::Recent_GameObject(const _uint& strLayerTag)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
			{
				return	Pair.second->Recent_GameObject(strLayerTag);
			}
		}
	}

	return nullptr;
}

list<class CGameObject*> CObject_Manager::Get_ALL_GameObject(_uint iLevelIndex, const _uint& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		MSG_BOX("못 가져옴");

	return  pLayer->Get_ALL_GameObject(strLayerTag);
}



CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _uint& strLayerTag)
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


