#include "stdafx.h"
#include "Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{ pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain& Prototype) : CGameObject{ Prototype }
{
}

HRESULT CTerrain::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
   CGameObject::GAMEOBJ_DESC* pDesc = static_cast <GAMEOBJ_DESC*>(pArg);
   
   if (FAILED(__super::Initialize(pDesc)))
       return E_FAIL;

    if (pDesc->Object_Type == Terrain_TYPE::TYPE_MAIN)
            m_bMain = true;

    if (FAILED(Add_Components()))
        return E_FAIL;
    Set_Model(pDesc->ProtoName, pDesc->CuriLevelIndex);

    if (pDesc->CuriLevelIndex != LEVEL_STAGE1)
    Set_Buffer(pDesc->Buffer[0], pDesc->Buffer[1]);
    else
    Set_Buffer(513, 513);

    if (m_pNavigationCom != nullptr)
    {
        m_pNavigationCom->Update(m_pTransformCom->Get_WorldMatrixPtr());
    }
    return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
    m_fTimeSum += fTimeDelta / m_iUVoffset;
}

void CTerrain::Update(_float fTimeDelta)
{
    if (isPowerOfTwoPlusOne(m_pSize[0]) && isPowerOfTwoPlusOne(m_pSize[1]))
        m_pVIBufferCom->Culling(m_pTransformCom->Get_WorldMatrix_Inverse());
}

void CTerrain::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_HEIGHT, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
        return;
    if (m_bFire == 1)
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
            return;
    }

    if (nullptr != m_pNavigationCom)
    {
        m_pGameInstance->Add_DebugComponents(m_pNavigationCom);
    }
}

HRESULT CTerrain::Render()
{ 
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_TimeSum", &m_fTimeSum, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_onEmissive", &m_bFire, sizeof(_int))))
            return E_FAIL;

    m_pShaderCom->Begin(m_bFire);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CTerrain::Set_Model(const _wstring& protoModel, _uint ILevel)
{
  if (FAILED(__super::Add_Component(ILevel, protoModel, TEXT("Com_Model"),
  reinterpret_cast<CComponent**>(&m_pTextureCom))))
  return;
}

void CTerrain::Set_Buffer(_uint x, _uint y)
{
    m_pSize[0] = x;             
    m_pSize[1] =  y; 

    if (isPowerOfTwoPlusOne(m_pSize[0]) && isPowerOfTwoPlusOne(m_pSize[1]))
    {
        m_pVIBufferCom->DYNAMIC_Set_Buffer(m_pSize[0], m_pSize[1]);
        m_pVIBufferCom->Set_QuadTree();
    }else
    {
        m_pVIBufferCom->Set_Buffer(m_pSize[0], m_pSize[1]);
    }

}
HRESULT CTerrain::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_TimeSum", &m_fTimeSum, sizeof(_float))))
        return E_FAIL;

    m_pShaderCom->Begin(2);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

    return S_OK;
}


_float3* CTerrain::Get_VtxPos()
{
    return  m_pVIBufferCom->Get_VtxPos();
}


void CTerrain::Set_Scalra_uint(_uint scalra)
{
    m_bFire = scalra;

}

void CTerrain::Set_Scalra_float(_float scalra)
{
    m_iUVoffset = scalra;
}


HRESULT CTerrain::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_Buffer"),
        reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (true == m_bMain)
    {
        CNavigation::NAVIGATION_DESC Desc{};
        Desc.iCurrentCellIndex = 0;
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
            TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom),&Desc)))
            return E_FAIL;
    }
    return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTerrain* pInstance = new CTerrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{

    CTerrain* pInstance = new CTerrain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTerrain::Free()
{
    __super::Free();

    if(m_pNavigationCom != nullptr)
    Safe_Release(m_pNavigationCom);


    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
