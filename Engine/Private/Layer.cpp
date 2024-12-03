#include "Layer.h"
#include "GameObject.h"
#include "Model.h"
CLayer::CLayer()
{  
   
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject, const _uint& strLayerTag)
{
    if (nullptr == pGameObject)
        return E_FAIL;

    m_GameObjects[strLayerTag].push_back(pGameObject);

   
    return S_OK;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
    for (size_t i = 0; i < m_iObjType; i++)
    {

        for (auto iter = m_GameObjects[i].begin(); iter != m_GameObjects[i].end();)
        {
            _int iResult = (*iter)->Priority_Update(fTimeDelta);

            if (OBJ_DEAD == iResult)
            {
                Safe_Release(*iter);
                iter = m_GameObjects[i].erase(iter);
            }
            else
                ++iter;
        }
    }
}

void CLayer::Update(_float fTimeDelta)
{
    for (size_t i = 0; i < m_iObjType; i++)
    {

        for (auto& pGameObject : m_GameObjects[i])
        {

            if (nullptr != pGameObject)
                pGameObject->Update(fTimeDelta);
        }
    }
}

void CLayer::Late_Update(_float fTimeDelta)
{
    for (size_t i = 0; i < m_iObjType; i++)
    {

        for (auto& pGameObject : m_GameObjects[i])
        {
            if (nullptr != pGameObject)
            {
                pGameObject->Late_Update(fTimeDelta);
            }
        }
    }
}

CGameObject* CLayer::Get_GameObject(const _uint& strLayerTag , const _uint& ProtoTag)
{
    for (auto& pGameObject : m_GameObjects[strLayerTag])
    {
        if (pGameObject->Get_Data() == ProtoTag)
            return pGameObject;
    }
    return nullptr;
}

_bool CLayer::IsGameObject(const _uint& strLayerTag)
{
    return m_GameObjects[strLayerTag].size() == 0 ? false : true;
}

CGameObject::PICKEDOBJ_DESC CLayer::Pking_onMash(_vector RayPos, _vector RayDir)
{
    CGameObject::PICKEDOBJ_DESC Desc{};

    Desc.pPickedObj = nullptr;
    Desc.fDis = {0xffff};
    _vector vPos{};
    _float fCurDIs{};
    for (size_t i = 0; i < m_iObjType; i++)
    {
        for (auto& pGameObject : m_GameObjects[i])
        {
            if (nullptr == pGameObject)
                continue;

            if (nullptr == pGameObject->Get_Model())
                continue;

            fCurDIs = pGameObject->check_BoxDist(RayPos, RayDir);

            if (fCurDIs < Desc.fDis)
            {
                Desc.fDis = fCurDIs;
                Desc.pPickedObj = pGameObject;
                Desc.vPos = RayPos + RayDir * fCurDIs;
            }
        }
    }
            
    return Desc;
}

CGameObject* CLayer::Recent_GameObject(const _uint& strLayerTag)
{

    if (m_GameObjects[strLayerTag].size() == 0)
        return nullptr;

    return m_GameObjects[strLayerTag].back();
}

list<class CGameObject*> CLayer::Get_ALL_GameObject(const _uint& strLayerTag)
{
    return m_GameObjects[strLayerTag];
}



CLayer* CLayer::Create()
{
    return new CLayer();
}

void CLayer::Free()
{
    __super::Free();
    for (_uint i = 0; i < m_iObjType; i++)
    {
        for (auto& pGameObject : m_GameObjects[i]) Safe_Release(pGameObject);
        m_GameObjects[i].clear();
    }

}
