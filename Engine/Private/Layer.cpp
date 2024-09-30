#include "Layer.h"
#include "GameObject.h"
#include "Model.h"
CLayer::CLayer()
{   m_iObjType = CGameObject::GAMEOBJ_TYPE_END;
    m_GameObjects = new list<class CGameObject*>[m_iObjType];
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject, const _uint& strLayerTag)
{
    if (nullptr == pGameObject)
        return E_FAIL;

    //


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

_bool CLayer::IsGameObject(const _uint& strLayerTag)
{
    return m_GameObjects[strLayerTag].size() == 0 ? false : true;
}

CGameObject::PICKEDOBJ_DESC CLayer::Pking_onMash(_vector RayPos, _vector RayDir)
{
    CGameObject::PICKEDOBJ_DESC Desc{};

    Desc.pPickedObj = nullptr;
    Desc.fDis = {0xffff};

    for (size_t i = 0; i < m_iObjType; i++)
    {
        for (auto& pGameObject : m_GameObjects[i])
        {
            if (nullptr == pGameObject)
                continue;

            if (nullptr == pGameObject->Get_Model())
                continue;

            if( CGameObject::DATA_ANIMAPOBJ==pGameObject->Get_Data() )
                continue;

            _float fCurDIs = pGameObject->Get_Model()->Check_Pick(RayPos, RayDir, pGameObject->Get_Transform());
            if (fCurDIs < Desc.fDis)
            {
                Desc.fDis = fCurDIs;
                Desc.pPickedObj = pGameObject;
            }
        }
    }

    return Desc;
}

CGameObject* CLayer::Recent_GameObject(const _uint& strLayerTag)
{
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
    Safe_Delete_Array(m_GameObjects);
}
