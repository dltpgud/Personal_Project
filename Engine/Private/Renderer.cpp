#include "Renderer.h"
#include "GameInstance.h"
#include "GameObject.h"
//#include "UI.h"

#include "VIBuffer_Rect.h"
#include "Shader.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{pDevice}, m_pContext{pContext}, m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
    _uint		iNumViewports = { 1 };

    D3D11_VIEWPORT		ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    /* For.Target_Diffuse */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* For.Target_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* For.Target_Shade */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.MRT_GameObjects */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;

    /* For.MRT_LightAcc */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;


    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;


#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.f, 150.f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.f, 450.f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.f, 150.f, 300.f, 300.f)))
        return E_FAIL;

#endif

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

#ifdef _DEBUG
    //if (FAILED(Render_Debug()))
    //    return E_FAIL;
#endif

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
#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pVIBuffer->Bind_Buffers();

    m_pGameInstance->Render_RT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);
    return S_OK;
}
#endif

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

    Safe_Release(m_pShader);
    Safe_Release(m_pVIBuffer);

    for (auto& GameObjects : m_RenderGameObjects)
    {
        for (auto& pRenderGameObject : GameObjects) Safe_Release(pRenderGameObject);
        GameObjects.clear();
    }

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
