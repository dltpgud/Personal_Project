#include "Layer.h"
#include "GameObject.h"
#include "Model.h"
CLayer::CLayer()
{  
   
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
    if (nullptr == pGameObject)
        return E_FAIL;

    m_GameObjects.push_back(pGameObject);

    return S_OK;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
   for (auto& pGameObject : m_GameObjects)
   {
       if (nullptr != pGameObject)
           pGameObject->Priority_Update(fTimeDelta);
   }
}

void CLayer::Update(_float fTimeDelta)
{
   for (auto& pGameObject : m_GameObjects)
   {
       if (nullptr != pGameObject)
           pGameObject->Update(fTimeDelta);
   }
}

void CLayer::Late_Update(_float fTimeDelta)
{
  for (auto& pGameObject : m_GameObjects)
  {
      if (nullptr != pGameObject)
      {
          pGameObject->Late_Update(fTimeDelta);
      }
  }
  
}

void CLayer::Delete()
{
  for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
  {
      if ((*iter)&& true == (*iter)->Get_Dead())
      {
          Safe_Release(*iter);
          iter = m_GameObjects.erase(iter);
      }
      else
          iter++;
  }
}

CGameObject* CLayer::Get_GameObject( const _uint& ProtoTag)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (pGameObject->Get_ObjectType() == ProtoTag)
            return pGameObject;
    }
    return nullptr;
}

_bool CLayer::IsGameObject()
{
    return m_GameObjects.size() == 0 ? false : true;
}

CGameObject::PICKEDOBJ_DESC CLayer::Pking_onMash(_vector RayPos, _vector RayDir)
{
    CGameObject::PICKEDOBJ_DESC Desc{};

    Desc.pPickedObj = nullptr;
    Desc.fDis = {0xffff};
    _vector vPos{};
    _float fCurDIs{};

        for (auto& pGameObject : m_GameObjects)
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
      
    return Desc;
}

CGameObject* CLayer::Recent_GameObject()
{
    if (m_GameObjects.size() == 0)
        return nullptr;

    return m_GameObjects.back();
}

list<class CGameObject*> CLayer::Get_GameObject_List()
{
    return m_GameObjects;
}

CLayer* CLayer::Create()
{
    return new CLayer();
}

void CLayer::Free()
{
    __super::Free();
        for (auto& pGameObject : m_GameObjects) Safe_Release(pGameObject);
        m_GameObjects.clear();
}
