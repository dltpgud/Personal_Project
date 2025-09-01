#include "UI_Manager.h"
#include "GameObject.h"
#include "UI.h"

CUI_Manager::CUI_Manager()
{
}

HRESULT CUI_Manager::Initialize()
{
    return S_OK;
}

HRESULT CUI_Manager::Add_UI_To_Proto(const _wstring& strProtoTag, CGameObject* pUI)
{
    CUI* UIObj = dynamic_cast<CUI*>(pUI);

    WRITE_LOCK;
    auto result = m_UIObj[UIOBJECT::UI_PROTO].emplace(strProtoTag, UIObj);

    if (!result.second)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CUI_Manager::Add_UI_To_CLone(const _wstring& strCloneTag, const _wstring& strProtoTag, void* pArg)
{
    auto& ui = m_UIObj[UIOBJECT::UI_PROTO].find(strProtoTag);

    CUI* UIObj = dynamic_cast<CUI*>(ui->second->Clone(pArg));

    m_UIObj[UIOBJECT::UI_CLONE].emplace(strCloneTag, UIObj);

    return S_OK;
}

CUI* CUI_Manager::Find_Proto_UIObj(const _wstring& strProtoUITag)
{
     auto iter = m_UIObj[UIOBJECT::UI_PROTO].find(strProtoUITag);

    if (iter == m_UIObj[UIOBJECT::UI_PROTO].end())
        return nullptr;

    return iter->second;
}

CUI* CUI_Manager::Find_Clone_UIObj(const _wstring& strCloneTag)
{
   auto iter = m_UIObj[UIOBJECT::UI_CLONE].find(strCloneTag);

   if (iter == m_UIObj[UIOBJECT::UI_CLONE].end())
        return nullptr;
 
   return iter->second;
}


void CUI_Manager::Priority_Update(_float fTimeDelta)
{
    for (auto& iter : m_UIObj[UIOBJECT::UI_CLONE])
    {
      if (true == iter.second->Check_Update())
         iter.second->Priority_Update(fTimeDelta);
    }

    ShakingEvent(fTimeDelta);
}

void CUI_Manager::Update(_float fTimeDelta)
{
   for (auto& iter : m_UIObj[UIOBJECT::UI_CLONE])
   {
        if (true == iter.second->Check_Update())
            iter.second->Update(fTimeDelta);
   }
}

void CUI_Manager::Late_Update(_float fTimeDelta)
{
   for (auto& iter : m_UIObj[UIOBJECT::UI_CLONE])
   {
       if (true == iter.second->Check_Update())
           iter.second->Late_Update(fTimeDelta);
   }
}

void CUI_Manager::Delete()
{
   for (auto& iter  = m_UIObj[UIOBJECT::UI_CLONE].begin(); iter != m_UIObj[UIOBJECT::UI_CLONE].end();)
   {
       if ((*iter).second && true == (*iter).second->Get_Dead())
       {
          Safe_Release((*iter).second);
          iter = m_UIObj[UIOBJECT::UI_CLONE].erase(iter);
       }
       else
           iter++;
   }
}

void CUI_Manager::Clear(_uint iClearLevelID)
{
  //for (auto iter = m_UIObj[UIOBJECT::UI_CLONE].begin(); iter != m_UIObj[UIOBJECT::UI_CLONE].end();)
  //{
  //    if ( true == (*iter).second->Check_Deleate(iClearLevelID-1))
  //    {
  //       Safe_Release((*iter).second);
  //      iter = m_UIObj[UIOBJECT::UI_CLONE].erase(iter);
  //    }
  //    else
  //        iter++;
  //}
}

void CUI_Manager::ShakingEvent(_float fTimeDelta)
{
   for (auto& iter : m_UIEeventList[EVENT_SHAKING]) 
   { 
       iter->ShakingEvent(fTimeDelta);
       Safe_Release(iter);
   }
   
    m_UIEeventList[EVENT_SHAKING].clear();
}

HRESULT CUI_Manager::Set_OpenUI(_bool bOpen, const _wstring& strCloneTag, class CGameObject* Owner)
{
     auto iter = m_UIObj[UIOBJECT::UI_CLONE].find(strCloneTag);
      
     if (iter == m_UIObj[UIOBJECT::UI_CLONE].end())
         return S_OK;
     if ( bOpen)
     {
         if (nullptr != Owner)
         {
             iter->second->Show(Owner);
         }
         else
         {
             iter->second->Set_Open(bOpen);
         }
     }
     else 
     {
         if (nullptr != Owner)
         {
             iter->second->Hide(Owner);
         }
         else
         {
             iter->second->Set_Open(bOpen);
         }
     }
        
   return S_OK;
}



HRESULT CUI_Manager::Set_UI_shaking(const _uint& uID, _float fShakingTime, _float fPowerX, _float fPowerY)
{
   for (auto& iter : m_UIObj[UIOBJECT::UI_CLONE])
   {
       if (iter.second->Get_UIID() == uID)
       {
           iter.second->Set_UI_shaking(fShakingTime, fPowerX, fPowerY);
       }
   } 
    return S_OK;
}

HRESULT CUI_Manager::UI_shaking(const _uint& uID, _float fTimeDelta)
{
    for (auto& iter : m_UIObj[UIOBJECT::UI_CLONE])
    {
        if (iter.second->Get_UIID() == uID)
        {
            iter.second->UI_shaking(fTimeDelta);
         }
    }
    return S_OK;
}

HRESULT CUI_Manager::ADD_UI_ShakingList(CUI* uiobj)
{
    m_UIEeventList[EVENT_SHAKING].push_back(uiobj);
    Safe_AddRef(uiobj);

    return S_OK;
}

CUI_Manager* CUI_Manager::Create()
{
    CUI_Manager* pInstance = new CUI_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CUI_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Manager::Free()
{
    __super::Free();

    for (size_t i = 0; i < UIOBJECT::UI_END; i++)
    {  
       for (auto& iter : m_UIObj[i]) { Safe_Release(iter.second); }
       m_UIObj[i].clear();
    }

    for (size_t i = 0; i < UIEVENT::EVENT_END; i++)
    {
        for (auto& iter : m_UIEeventList[i]) { Safe_Release(iter); }
        m_UIEeventList[i].clear();
    }
}
