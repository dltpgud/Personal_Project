#include "UI_Manager.h"
#include "GameObject.h"
#include "UI.h"

CUI_Manager::CUI_Manager()
{
}

HRESULT CUI_Manager::Initialize(_uint iNumLevels)
{
    m_iLevel = iNumLevels;

    m_UIVec = new vector<class CUI*>[iNumLevels];

    return S_OK;
}

HRESULT CUI_Manager::Add_UI(CGameObject* pPrototype, _uint iLevelIndex)
{
    CUI* pUI = dynamic_cast<CUI*>(pPrototype);
    if (pUI == nullptr)
        return E_FAIL;

    m_UIVec[iLevelIndex].push_back(pUI);

    return S_OK;
}

void CUI_Manager::Priority_Update(_float fTimeDelta)
{
    for (_uint i = 0; i < m_iLevel; i++)
    {
        for (auto veciter = m_UIVec[i].begin(); veciter != m_UIVec[i].end();)
        {
            if ((*veciter)->Get_PrUpdate() == true)
            {
                int iResult = (*veciter)->Priority_Update(fTimeDelta);

                if (OBJ_DEAD == iResult)
                {

                    Safe_Release((*veciter));
                    veciter= m_UIVec[i].erase(veciter);
                }
            }
            veciter++;
        }
    }
}

void CUI_Manager::Update(_float fTimeDelta)
{
    for (_uint i = 0; i < m_iLevel; i++)
    {
        for (auto& vec : m_UIVec[i])
        {
            if (vec->Get_Update() == true)
                vec->Update(fTimeDelta);
        }
    }
}

void CUI_Manager::Late_Update(_float fTimeDelta)
{
    for (_uint i = 0; i < m_iLevel; i++)
    {
        for (auto& vec : m_UIVec[i])
        {
            if (vec->Get_LateUpdate() == true)
                vec->Late_Update(fTimeDelta);
        }
    }
}

void CUI_Manager::Clear(_uint iClearLevelID)
{
    for (auto& vec : m_UIVec[iClearLevelID]) { Safe_Release(vec); }
    m_UIVec[iClearLevelID].clear();
    m_UIVec[iClearLevelID].shrink_to_fit();
}

HRESULT CUI_Manager::Set_OpenUI(const _uint& uid, _bool open)
{
    for (size_t i = 0; i < m_iLevel; i++)
    {
        for (auto& vec : m_UIVec[i])
        {
            if (vec->Get_UIID() == uid)
            {
               vec->Set_Open(open);
            }
        }
    }
    return S_OK;
}

HRESULT CUI_Manager::Set_PreUpdateUI(const _uint& uid, _bool open)
{
    for (size_t i = 0; i < m_iLevel; i++)
    {
        for (auto& vec : m_UIVec[i])
        {
            if (vec->Get_UIID() == uid)
            {
                vec->Set_PrUpdate(open);
            }
        }
    }
    return S_OK;
}

HRESULT CUI_Manager::Set_UpdateUI(const _uint& uid, _bool open)
{
    for (size_t i = 0; i < m_iLevel; i++)
    {
        for (auto& vec : m_UIVec[i])
        {
            if (vec->Get_UIID() == uid)
            {
                vec->Set_Update(open);
            }
        }
    }
    return S_OK;
}

HRESULT CUI_Manager::Set_OpenUI_Inverse(const _uint& Openuid, const _uint& Cloaseduid)
{
    for (size_t i = 0; i < m_iLevel; i++)
    {
        for (auto& vec : m_UIVec[i])
        {
            if (vec->Get_UIID() == Openuid)
            {
                vec->Set_Open(true);
            }

            if (vec->Get_UIID() == Cloaseduid)
            {
                vec->Set_Open(false);
            }
        }
    }
    return S_OK;
}

HRESULT CUI_Manager::Set_LateUpdateUI(const _uint& uid, _bool open)
{
    for (size_t i = 0; i < m_iLevel; i++)
    {
        for (auto& vec : m_UIVec[i])
        {
            if (vec->Get_UIID() == uid)
            {
                vec->Set_LateUpdate(open);
            }
        }
    }
    return S_OK;
}

CGameObject* CUI_Manager::Get_UI(const _uint& iLevel, const _uint& uID)
{
    for (auto& vec : m_UIVec[iLevel])
    {
        if (vec->Get_UIID() == uID)
        {
            return vec;
        }
    }

    return nullptr;
}
HRESULT CUI_Manager::Set_UI_shaking(const _uint& uID, _float fShakingTime, _float fPowerX, _float fPowerY)
{
    for (size_t i = 0; i < m_iLevel; i++)
    {
        for (auto& vec : m_UIVec[i])
        {
            if (vec->Get_UIID() == uID)
            {
                vec->Set_UI_shaking(fShakingTime, fPowerX, fPowerY);
     
            }
        }
    }
    return S_OK;
}

HRESULT CUI_Manager::UI_shaking(const _uint& uID, _float fTimeDelta)
{

    for (size_t i = 0; i < m_iLevel; i++)
    {
        for (auto& vec : m_UIVec[i])
        {
            if (vec->Get_UIID() == uID)
            {
                vec->UI_shaking(fTimeDelta);
            }
        }
    }

    return S_OK;
}

CUI_Manager* CUI_Manager::Create(_uint iNumLevels)
{
    CUI_Manager* pInstance = new CUI_Manager();

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Created : CUI_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Manager::Free()
{
    __super::Free();

    for (_uint i = 0; i < m_iLevel; i++)
    {
        for (auto& vec : m_UIVec[i]) { Safe_Release(vec); }
        m_UIVec[i].clear();
        m_UIVec[i].shrink_to_fit();
    }
    Safe_Delete_Array(m_UIVec);
}
