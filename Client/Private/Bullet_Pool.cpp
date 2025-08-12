#include "stdafx.h"
#include "Bullet_Pool.h"
#include "GameInstance.h"
#include "GameObject.h"
CBullet_Pool::CBullet_Pool()
    : m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

CGameObject* CBullet_Pool::GetBullet(void* Arg)
{
    for (auto& pBullet : m_vecBullet)
    {
        CBullet* p = dynamic_cast<CBullet*>(pBullet);
        if (p )
        {
            p->Reset(Arg);
            return p;
        }
    }

    return m_pGameInstance->Clone_Prototype(L"Prototype GameObject_Bullet", Arg);
}

HRESULT CBullet_Pool::Initialize_Prototype(size_t size, void* Arg)
{
    CBullet::CBULLET_DESC* pDesc = static_cast<CBullet::CBULLET_DESC*>(Arg);
    m_vecBullet.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
        CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(L"Prototype GameObject_Bullet",pDesc);
        m_vecBullet.push_back(pGameObject);
        Safe_AddRef(pGameObject);
    }
    return S_OK;
}

CBullet_Pool* CBullet_Pool::Create(size_t size, void* Arg)
{
    CBullet_Pool* pInstance = new CBullet_Pool();

    if (FAILED(pInstance->Initialize_Prototype( size, Arg)))
    {
        MSG_BOX("Failed to Created : CBullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBullet_Pool::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);

    for (auto iter : m_vecBullet) Safe_Release(iter);
    m_vecBullet.clear();
    m_vecBullet.shrink_to_fit();
}
