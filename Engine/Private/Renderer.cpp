#include "Renderer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "UI.h"
CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{pDevice}, m_pContext{pContext}, m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
    return S_OK;
}

HRESULT CRenderer::Add_RenderGameObject(RENDERGROUP eRenderGroup, CGameObject* pRenderGameObject)
{
    if (eRenderGroup >= RG_END || nullptr == pRenderGameObject)
        return E_FAIL;

    m_RenderGameObjects[static_cast<_uint>(eRenderGroup)].push_back(pRenderGameObject);

    Safe_AddRef(pRenderGameObject); //  푸쉬해줄 떄 마다 레퍼런스 올려준다.

    return S_OK;
}

HRESULT CRenderer::Draw()
{
    if (FAILED(Render_Priority()))
        return E_FAIL;

    if (FAILED(Render_Shadow()))
        return E_FAIL;

    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    if (FAILED(Render_NonLight()))
        return E_FAIL;

    if (FAILED(Render_Blend()))
        return E_FAIL;

    if (FAILED(Render_UI()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
    for (auto& pRenderGameObject : m_RenderGameObjects[RG_PRIORITY])
    {
        if (nullptr != pRenderGameObject)
            pRenderGameObject->Render();

        Safe_Release(pRenderGameObject);
    }

    m_RenderGameObjects[RG_PRIORITY].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
    for (auto& pRenderGameObject : m_RenderGameObjects[RG_SHADOW])
    {
        if (nullptr != pRenderGameObject)
            pRenderGameObject->Render();

        Safe_Release(pRenderGameObject);
    }

    m_RenderGameObjects[RG_SHADOW].clear();

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    for (auto& pRenderGameObject : m_RenderGameObjects[RG_NONBLEND])
    {
        if (nullptr != pRenderGameObject)
            pRenderGameObject->Render();

        Safe_Release(pRenderGameObject);
    }

    m_RenderGameObjects[RG_NONBLEND].clear();

    return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
    for (auto& pRenderGameObject : m_RenderGameObjects[RG_NONLIGHT])
    {
        if (nullptr != pRenderGameObject)
            pRenderGameObject->Render();

        Safe_Release(pRenderGameObject);
    }

    m_RenderGameObjects[RG_NONLIGHT].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    for (auto& pRenderGameObject : m_RenderGameObjects[RG_BLEND])
    {
        if (nullptr != pRenderGameObject)
            pRenderGameObject->Render();

        Safe_Release(pRenderGameObject);
    }

    m_RenderGameObjects[RG_BLEND].clear();

    return S_OK;
}

HRESULT CRenderer::Render_UI()
{
    for (auto& pRenderGameObject : m_RenderGameObjects[RG_UI])
    {

        if (nullptr != pRenderGameObject)
            pRenderGameObject->Render();

        Safe_Release(pRenderGameObject);
    }

    m_RenderGameObjects[RG_UI].clear();

    return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderer::Free()
{
    __super::Free();

    for (auto& GameObjects : m_RenderGameObjects)
    {
        for (auto& pRenderGameObject : GameObjects) Safe_Release(pRenderGameObject);
        GameObjects.clear();
    }

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
