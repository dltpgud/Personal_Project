#include "Renderer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BlendObject.h"
#include "UI.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Texture.h"
#include "CS_SSAO.h"

_uint g_iSizeX = 8192; 
_uint g_iSizeY = 8192;

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{pDevice}, m_pContext{pContext}, m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize(_uint iWinSizeX, _uint iWinSizeY)
{
    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;

    Initialize_RT();
    Initialize_SizeViewPort();
    Initialize_MRT();
    Add_Components();

    ///////////////////
    ID3D11Texture2D* pDepthStencilTexture = nullptr;

    D3D11_TEXTURE2D_DESC TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = g_iSizeX;
    TextureDesc.Height = g_iSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthStencilView)))
        return E_FAIL;

    Safe_Release(pDepthStencilTexture); 

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Depth"), 50.f, 50.f, 150.f, 150.f)))
         return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Decal"),  50.f, 200.f, 150.f, 150.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"),   50.f, 350.f, 150.f, 150.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 200.f, 50.f, 150.f, 150.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Final"), 200.f, 200.f, 150.f, 150.f)))
        return E_FAIL;

    m_pCS_SSAO->Ready_Debug(200.f, 350.f, 150.f, 150.f);

     if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Accum"), 200.f, 350.f, 150.f, 150.f)))
       return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_VFX"), 350.f, 50.f, 150.f, 150.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Final"), 350.f, 200.f, 150.f, 150.f)))
       return E_FAIL;    
   // if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Bloom"), 500.f, 50.f, 150.f, 150.f)))
   //     return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Emissive"), 650.f, 50.f, 150.f, 150.f)))
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CRenderer::Add_RenderGameObject(RENDERGROUP eRenderGroup, CGameObject* pRenderGameObject)
{
    if (eRenderGroup >= RG_END || nullptr == pRenderGameObject)
        return E_FAIL;

    if (eRenderGroup < RG_PRIORITY)
        return E_FAIL;

    m_RenderGameObjects[eRenderGroup].push_back(pRenderGameObject);

    Safe_AddRef(pRenderGameObject);

    return S_OK;
}

HRESULT CRenderer::Add_DebugComponents(CComponent* pComponent)
{
    m_DebugComponents.push_back(pComponent);

    Safe_AddRef(pComponent);

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
    
    if (FAILED(Render_Decal()))
        return E_FAIL;

    if (FAILED(Render_Lights()))
        return E_FAIL;

    if (FAILED(Render_LightsCombine()))
        return E_FAIL;

    if (FAILED(Render_Effect()))
        return E_FAIL;

    if (FAILED(Render_Bloom()))
        return E_FAIL;

    if (FAILED(Render_Final()))
        return E_FAIL;

    if (FAILED(Render_NonLight()))
        return E_FAIL;

    if (FAILED(Render_UI()))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(Render_Debug()))
       return E_FAIL;
#endif

    return S_OK;
}



HRESULT CRenderer::Initialize_SizeViewPort()
{
    // 오리지널 사이즈
    D3D11_VIEWPORT ViewPortDesc;
    ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = static_cast<_float>(m_iWinSizeX);
    ViewPortDesc.Height = static_cast<_float>(m_iWinSizeY);
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;
    m_ViewPortDescs[SIZE_ORIGINAL] = ViewPortDesc;

    m_fdX[SIZE_ORIGINAL] = 1.f / static_cast<_float>(m_iWinSizeX); // 샘플링할 텍스쳐 쿠드들 좌표 
    m_fdY[SIZE_ORIGINAL] = 1.f / static_cast<_float>(m_iWinSizeY);

    // 4 다운 샘플링
    ViewPortDesc.Width = static_cast<_float>(m_iWinSizeX) / 2.f;
    ViewPortDesc.Height = static_cast<_float>(m_iWinSizeY) / 2.f;
    m_ViewPortDescs[SIZE_DOWN_2] = ViewPortDesc;

    m_fdX[SIZE_DOWN_2] = 2.f / static_cast<_float>(m_iWinSizeX);
    m_fdY[SIZE_DOWN_2] = 2.f / static_cast<_float>(m_iWinSizeY);

    // 4x4 다운 샘플링
    ViewPortDesc.Width = static_cast<_float>(m_iWinSizeX) / 4.f;
    ViewPortDesc.Height = static_cast<_float>(m_iWinSizeY) / 4.f;
    m_ViewPortDescs[SIZE_DOWN_4] = ViewPortDesc;

    m_fdX[SIZE_DOWN_4] = 4.f / static_cast<_float>(m_iWinSizeX);
    m_fdY[SIZE_DOWN_4] = 4.f / static_cast<_float>(m_iWinSizeY);

    // 4x4x4 다운 샘플링
    ViewPortDesc.Width = static_cast<_float>(m_iWinSizeX) / 8.f;
    ViewPortDesc.Height = static_cast<_float>(m_iWinSizeY) / 8.f;
    m_ViewPortDescs[SIZE_DOWN_8] = ViewPortDesc;

    m_fdX[SIZE_DOWN_8] = 8.f / static_cast<_float>(m_iWinSizeX);
    m_fdY[SIZE_DOWN_8] = 8.f / static_cast<_float>(m_iWinSizeY);

    /*그림자 세팅*/
    ViewPortDesc.Width = (_float)g_iSizeX;
    ViewPortDesc.Height = (_float)g_iSizeY;
    m_ViewPortDescs[SIZE_SHADOW] = ViewPortDesc;

    return S_OK;
}

HRESULT CRenderer::Initialize_RT()
{ /* For.Target_Diffuse */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_EFDiffuse"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    
    /* For.Target_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_MtrlAmbient */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_MtrlAmbient"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_LightDepth*/
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32_FLOAT,
                                                 _float4(1.f, 1.f, 1.f, 1.f))))  
        return E_FAIL;

    /* For.Target_Shade */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* For.Target_Specular */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_Rim */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Rim"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* For.Target_Emissive */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_Decal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Decal"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_DecalNormal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DecalNormal"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0, 0, 0, 0))))
        return E_FAIL;

    /* For.Target_Final */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_VFX"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_Temp"), m_iWinSizeX, m_iWinSizeY,
                                                 DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_2"), m_iWinSizeX / 2, m_iWinSizeY / 2,
                                                 DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_2_Temp"), m_iWinSizeX / 2, m_iWinSizeY / 2,
                                                 DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))

        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_4"), m_iWinSizeX / 4, m_iWinSizeY / 4,
                                                 DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_4_Temp"), m_iWinSizeX / 4, m_iWinSizeY / 4,
                                                 DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_8"), m_iWinSizeX / 8, m_iWinSizeY / 8,
                                                 DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_8_Temp"), m_iWinSizeX / 8, m_iWinSizeY / 8,
                                                 DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Accum"), m_iWinSizeX, m_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT,_float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_AccumNormal"), m_iWinSizeX, m_iWinSizeY,DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Revealage"), m_iWinSizeX, m_iWinSizeY, DXGI_FORMAT_R32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_AccumBlur"), m_iWinSizeX, m_iWinSizeY,DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Initialize_MRT()
{
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_Bloom"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_2"), TEXT("Target_Bloom_2"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_4"), TEXT("Target_Bloom_4"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_8"), TEXT("Target_Bloom_8"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_Temp"), TEXT("Target_Bloom_Temp"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_2_Temp"), TEXT("Target_Bloom_2_Temp"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_4_Temp"), TEXT("Target_Bloom_4_Temp"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_8_Temp"), TEXT("Target_Bloom_8_Temp"))))
        return E_FAIL;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* For.MRT_GameObjects */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Rim"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_MtrlAmbient"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Bloom"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_VFX"))))
        return E_FAIL;
    
    /* For.MRT_LightAcc */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Decal"), TEXT("Target_Decal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Decal"), TEXT("Target_DecalNormal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Final"))))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_WBOIT"), TEXT("Target_Accum"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_WBOIT"), TEXT("Target_AccumNormal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_WBOIT"), TEXT("Target_Revealage"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Add_Components()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    m_WorldMatrix._11 = static_cast<_float>(m_iWinSizeX);
    m_WorldMatrix._22 = static_cast<_float>(m_iWinSizeY);

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<_float>(m_iWinSizeX),
                                                          static_cast<_float>(m_iWinSizeY), 0.f, 1.f));

    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"),
                                VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
    m_pDecalShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Decal.hlsl"),
                                     VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pDecalShader)
        return E_FAIL;

    m_pCS_SSAO = CSSAO_ComputeShader::Create(m_pDevice, m_pContext);
    if (!m_pCS_SSAO)
        return E_FAIL;

    return S_OK;
}


HRESULT CRenderer::Render_Priority()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"))))
        return E_FAIL;
   
    for (auto& pRenderGameObject : m_RenderGameObjects[RG_PRIORITY])
    {
        if (nullptr != pRenderGameObject)
            pRenderGameObject->Render();

        Safe_Release(pRenderGameObject);
    }

    m_RenderGameObjects[RG_PRIORITY].clear();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Final"))))
        return E_FAIL;
   
    return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
    m_pContext->ClearDepthStencilView(m_pLightDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    m_pContext->RSSetViewports(1, &m_ViewPortDescs[SIZE_SHADOW]);

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pLightDepthStencilView)))
        return E_FAIL;
   
        for (auto& pRenderGameObject : m_RenderGameObjects[RG_SHADOW])
        {
        if (nullptr != pRenderGameObject)
            pRenderGameObject->Render_Shadow();
    
            Safe_Release(pRenderGameObject);
        }
    
        m_RenderGameObjects[RG_SHADOW].clear();

   if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Shadow"))))
        return E_FAIL;

    m_pContext->RSSetViewports(1, &m_ViewPortDescs[SIZE_ORIGINAL]);
   
    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;

    for (auto& pRenderGameObject : m_RenderGameObjects[RG_NONBLEND])
    {
        if (nullptr != pRenderGameObject)
            pRenderGameObject->Render();

        Safe_Release(pRenderGameObject);
    }

    m_RenderGameObjects[RG_NONBLEND].clear();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
    // 4배 다운 샘플링
    m_pContext->RSSetViewports(1, &m_ViewPortDescs[SIZE_DOWN_2]); // 이 4배 다운 샘플링 뷰포트에 적용
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_2"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DiffuseTexture", TEXT("Target_Bloom"))))
        return E_FAIL;

    m_pShader->Begin(4); // 원본이미지를 4배 다운샘플링 진행하겠다.

    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_2"))))
        return E_FAIL;

    // 16배 다운 샘플링
    m_pContext->RSSetViewports(1, &m_ViewPortDescs[SIZE_DOWN_4]); // 16배 다운 샘플링 뷰포트
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_4"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DiffuseTexture", TEXT("Target_Bloom_2"))))
        return E_FAIL; // 4배 다운 샘플링한 결과를 가져온다.

    m_pShader->Begin(4); // 4배 다운 샘플링한 결과에 16배 다운 샘플링을 진행하겠다.

    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_4"))))
        return E_FAIL;

    // 64배 다운 샘플링
    m_pContext->RSSetViewports(1, &m_ViewPortDescs[SIZE_DOWN_8]); // 64배 다운 샘플링 뷰포트
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_8"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DiffuseTexture", TEXT("Target_Bloom_4"))))
        return E_FAIL; // 16배 다운 샘플링한 결과를 가져온다.

    m_pShader->Begin(4); // 16배 다운 샘플링한 결과에 64배 다운 샘플링을 진행하겠다.

    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_8"))))
        return E_FAIL;
    // 64배 텍스쳐에 대한 블러와 업스케일링, 44 텍스쳐에 대한 add 연산
    if (FAILED(m_pShader->Bind_RawValue("dX", &m_fdX[SIZE_DOWN_8],
                                        sizeof(_float)))) // 64배 뷰포트에 텍스쳐 쿠드 좌표를 쉐이더로 던져라
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("dY", &m_fdY[SIZE_DOWN_8], sizeof(_float))))
        return E_FAIL;

    // X 블러
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_8_Temp"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DiffuseTexture", TEXT("Target_Bloom_8"))))
        return E_FAIL; // 64배 다운 샘플링한 텍스쳐를 던저라

    m_pShader->Begin(5); // X블러
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();
    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_8_Temp"))))
        return E_FAIL;

    // Y 블러
    m_pContext->RSSetViewports(1, &m_ViewPortDescs[SIZE_DOWN_4]); // 16배 다운 샘플링 뷰포트
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_4"), nullptr, false)))
        return E_FAIL; // 기존 텍스처를 밀지말고 이미지를 가져와라

    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DiffuseTexture", TEXT("Target_Bloom_8_Temp"))))
        return E_FAIL;
    m_pShader->Begin(6); // X블러한 이미지를 가져와서 Y블러 처리해라
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();
    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_4"))))
        return E_FAIL;

    // 16배 텍스쳐에 대한 블러와 업스케일링, 4 텍스쳐에 대한 add 연산
    if (FAILED(m_pShader->Bind_RawValue("dX", &m_fdX[SIZE_DOWN_4],
                                        sizeof(_float)))) // 16배 뷰포트에 텍스쳐 쿠드 좌표를 쉐이더로 던져라
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("dY", &m_fdY[SIZE_DOWN_4], sizeof(_float))))
        return E_FAIL;

    // X 블러
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_4_Temp"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DiffuseTexture", TEXT("Target_Bloom_4"))))
        return E_FAIL; // 16배 다운 샘플링한 이미지를  던져라

    m_pShader->Begin(5); // x블러
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();
    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_4_Temp"))))
        return E_FAIL;

    // Y 블러
    m_pContext->RSSetViewports(1, &m_ViewPortDescs[SIZE_DOWN_2]);

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_2"), nullptr, false)))
        return E_FAIL; // 4배 다운 샘플링한 이미지를 밀지 말고 열어서  블러 시작하자
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DiffuseTexture", TEXT("Target_Bloom_4_Temp"))))
        return E_FAIL; // 16배 다운 샘플링 후 X블러를 먹인 이지를 던져라
    m_pShader->Begin(6);
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_2"))))
        return E_FAIL;

    // 4배 텍스쳐에 대한 블러와 업스케일링, 블룸 텍스쳐에 대한 add 연산
    if (FAILED(m_pShader->Bind_RawValue("dX", &m_fdX[SIZE_DOWN_2],
                                        sizeof(_float)))) // 4배 뷰포트에 텍스쳐 쿠드 좌표를 쉐이더로 던져라
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("dY", &m_fdY[SIZE_DOWN_2], sizeof(_float))))
        return E_FAIL;

    // X 블러링
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_2_Temp"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DiffuseTexture", TEXT("Target_Bloom_2"))))
        return E_FAIL; // 4배 다운 샘플링한 텍스쳐를 가져와서 X블러 실행

    m_pShader->Begin(5);
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom_2_Temp"))))
        return E_FAIL;

    // Y 블러링
    m_pContext->RSSetViewports(1, &m_ViewPortDescs[SIZE_ORIGINAL]);
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DiffuseTexture", TEXT("Target_Bloom_2_Temp"))))
        return E_FAIL;

    m_pShader->Begin(6);
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Bloom"))))
        return E_FAIL;
    return S_OK;
}

HRESULT CRenderer::Render_Decal()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_WBOIT"))))
        return E_FAIL;

    if (FAILED(m_pDecalShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pDecalShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pDecalShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pDecalShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pDecalShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pDecalShader, "g_DepthTexture", TEXT("Target_Depth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pDecalShader, "g_NormalTexture", TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pDecalShader->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pDecalShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    _float2 WindowSize = {(_float)m_iWinSizeX, (_float)m_iWinSizeY};
    m_pDecalShader->Bind_RawValue("g_WinDowSize", &WindowSize, sizeof(_float2));

    m_pGameInstance->Render_AllDecal(m_pDecalShader);

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_WBOIT"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Decal") )))
       return E_FAIL;
    
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_AccumNormalTexture", TEXT("Target_AccumNormal"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_AccumTexture", TEXT("Target_Accum"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_RevealageTexture", TEXT("Target_Revealage"))))
        return E_FAIL;

    m_pShader->Begin(8);
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Decal"))))
       return E_FAIL;
    return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"),nullptr,false)))
        return E_FAIL;

        m_pGameInstance->Render_All();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;
    return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
   

    if (m_pGameInstance->Get_DIKeyDown(DIK_Y))
    {
        if (bssao)
            bssao = false;
        else
            bssao = true;
    }


    if (true == bssao)
    {
        m_pCS_SSAO->DispatchSSAO(TEXT("Target_Depth"), TEXT("Target_Normal"));
        m_pCS_SSAO->Bind_SRV(m_pShader, "g_SSAOTexture");
    }

    if (FAILED(m_pShader->Bind_RawValue("g_SSAOEnable", &bssao, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_NormalTexture", TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_vMtrlAmbient", TEXT("Target_MtrlAmbient"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DecalNormalTexture", TEXT("Target_DecalNormal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DecalTexture", TEXT("Target_Decal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_VFXTexture", TEXT("Target_VFX"))))
        return E_FAIL;
    
    m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_LightsCombine()
{
    // 조명 +그림자 + 외곽선
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"), nullptr, false)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

     if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    
    
    _float2 WindowSize = {(_float)m_iWinSizeX, (_float)m_iWinSizeY};
    if (FAILED(m_pShader->Bind_RawValue("g_WinDowSize", &WindowSize, sizeof(_float2))))
        return E_FAIL;

    _float2 ShadowSize = {(_float)g_iSizeX, (_float)g_iSizeY};
    if (FAILED(m_pShader->Bind_RawValue("g_shadowMapSize", &ShadowSize, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_ShadeTexture", TEXT("Target_Shade"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_SpecularTexture", TEXT("Target_Specular"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_LightDepthTexture", TEXT("Target_LightDepth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DecalTexture", TEXT("Target_Decal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_NormalTexture", TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
        return E_FAIL;

    m_pShader->Begin(3);
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT(TEXT("MRT_Final"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Final()
{ 
    //if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
    //    return E_FAIL;
    //if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
    //    return E_FAIL;
    //if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    //    return E_FAIL;
    //if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
    //    return E_FAIL;
    //
    //if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
    //    return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_EmissiveTexture", TEXT("Target_Emissive"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_BloomTexture", TEXT("Target_Bloom"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_RimTexture", TEXT("Target_Rim"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_SRV(m_pShader, "g_FinalTexture", TEXT("Target_Final"))))
        return E_FAIL;
    
  //  if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
   //     return E_FAIL;
   // if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
   //     return E_FAIL;
    
    m_pShader->Begin(7);
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();


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

HRESULT CRenderer::Render_UI()
{
     m_RenderGameObjects[RG_UI].sort( [](CGameObject* pSour, CGameObject* pDest) -> _bool
        {
            return static_cast<CUI*>(pSour)->Get_fz() > static_cast<CUI*>(pDest)->Get_fz();
        }); 
    
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
    for (auto& pDebugCom : m_DebugComponents)
    {
        if (nullptr != pDebugCom)
        {
            pDebugCom->Render();
            Safe_Release(pDebugCom);
        }
    }
      m_DebugComponents.clear();

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
   
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
   
    m_pVIBuffer->Bind_Buffers();
   
    m_pGameInstance->Render_RT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);
    m_pGameInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer);
    m_pGameInstance->Render_RT_Debug(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer);
    m_pGameInstance->Render_RT_Debug(TEXT("MRT_Decal"), m_pShader, m_pVIBuffer);
    m_pGameInstance->Render_RT_Debug(TEXT("MRT_Bloom_2"), m_pShader, m_pVIBuffer);
    m_pGameInstance->Render_RT_Debug(TEXT("MRT_Bloom_4"), m_pShader, m_pVIBuffer);
    m_pGameInstance->Render_RT_Debug(TEXT("MRT_Bloom_8"), m_pShader, m_pVIBuffer);
    m_pGameInstance->Render_RT_Debug(TEXT("MRT_Height"), m_pShader, m_pVIBuffer);
    m_pGameInstance->Render_RT_Debug(TEXT("MRT_Final"), m_pShader, m_pVIBuffer);
    m_pGameInstance->Render_RT_Debug(TEXT("MRT_WBOIT"), m_pShader, m_pVIBuffer);
    
  //  m_pCS_SSAO->Render(m_pShader, m_pVIBuffer);
   // m_pGameInstance->RenderGrid();
    return S_OK;
}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWinSizeX, _uint iWinSizeY)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iWinSizeX, iWinSizeY)))
    {
        MSG_BOX("Failed to Created : CRenderer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderer::Free()
{
    __super::Free();

    Safe_Release(m_pLightDepthStencilView);
    Safe_Release(m_pDecalShader);
    Safe_Release(m_pCS_SSAO);
    Safe_Release(m_pShader);
    Safe_Release(m_pVIBuffer);

    for (auto& pDebugCom : m_DebugComponents) Safe_Release(pDebugCom);

    m_DebugComponents.clear();

    for (auto& GameObjects : m_RenderGameObjects)
    {
        for (auto& pRenderGameObject : GameObjects) Safe_Release(pRenderGameObject);
        GameObjects.clear();
    }

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}

