#include "GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }, m_pContext{ pContext }, m_pGameInstance{ CGameInstance::GetInstance() }, m_bClone(false)
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject& Prototype) 
    : m_pDevice{Prototype.m_pDevice}, m_pContext{Prototype.m_pContext}, m_pGameInstance{Prototype.m_pGameInstance},
      m_pTransformCom{Prototype.m_pTransformCom}, m_pColliderCom{ Prototype.m_pColliderCom }, m_bClone(true)
{
    Safe_AddRef(m_pColliderCom);
    Safe_AddRef(m_pTransformCom);
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

HRESULT CGameObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
    GAMEOBJ_DESC* pDesc = static_cast<GAMEOBJ_DESC*>(pArg);
     m_iObjectType   = pDesc->Object_Type;
     m_pTransformCom = CTransform::Create(m_pDevice, m_pContext, pDesc);

    if (nullptr == m_pTransformCom)
        return E_FAIL;

    m_Components.emplace(TEXT("Trans_Com"), m_pTransformCom);

    Safe_AddRef(m_pTransformCom);

    return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
    return;
}

void CGameObject::Update(_float fTimeDelta)
{
    if (m_pColliderCom) {
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
    }
}

void CGameObject::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

    if (m_pColliderCom)
    {
        m_pGameInstance->Add_DebugComponents(m_pColliderCom);
    }
#endif // _DEBUG

}

HRESULT CGameObject::Render()
{

    return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag,
                                   CComponent** ppOut, void* pArg)
{
    CComponent* pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
    if (nullptr == pComponent)
        return E_FAIL;

    if (nullptr != Find_Component(strComponentTag))
        return E_FAIL;

    m_Components.emplace(strComponentTag, pComponent);

    *ppOut = pComponent;

    Safe_AddRef(pComponent);

    return S_OK;
}

CComponent* CGameObject::Find_Component(const _wstring& strComponentTag)
{
    auto iter = m_Components.find(strComponentTag);

    if (iter == m_Components.end())
        return nullptr;

    return iter->second;
}

void CGameObject::Free()
{
    __super::Free();

    for (auto& Pair : m_Components) Safe_Release(Pair.second);
    m_Components.clear();

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
