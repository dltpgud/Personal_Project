#include "stdafx.h"
#include "NonAniObj.h"
#include "GameInstance.h"

CNonAni::CNonAni(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject {pDevice, pContext}
{
}

CNonAni::CNonAni(const CNonAni& Prototype) : CGameObject { Prototype }
{
}

HRESULT CNonAni::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNonAni::Initialize(void* pArg)
{
    GAMEOBJ_DESC* pDesc = static_cast<GAMEOBJ_DESC*>(pArg);
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

   
    return S_OK;
}

void CNonAni::Priority_Update(_float fTimeDelta)
{

}

void CNonAni::Update(_float fTimeDelta)
{
}

void CNonAni::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
  
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_HEIGHT, this)))
        return;
}

HRESULT CNonAni::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;
    
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::TRANSFORM_STATE::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::TRANSFORM_STATE::D3DTS_PROJ))))
        return E_FAIL;
    
    
    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
    
    for (_uint i = 0; i < iNumMeshes; i++)
    {
     
        if (FAILED(m_pShaderCom->Begin(2)))
            return E_FAIL;
    
            m_pModelCom->Render(i);
    }
    
    return S_OK;
}

HRESULT CNonAni::Render()
{
   
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMesh = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMesh; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;
 
        if (FAILED(m_pShaderCom->Begin(m_iPass)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
    return S_OK;
}

void CNonAni::Set_Model(const _wstring& protoModel, _uint ILevel)
{
     m_wModel = protoModel;
    if (FAILED(__super::Add_Component(ILevel, protoModel, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        MSG_BOX("Set_Model failed");
        return;
    }

    if (protoModel == L"Proto Component BossFloor Model_nonaniObj")
        m_iPass = 1;
    else
        m_iPass = 0;

   // if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOWBG, this)))
    //    return;
}

void CNonAni::Set_InstaceBuffer(const vector<_matrix>& worldmat)
{
   if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshIst"), TEXT("Com_Shader"),
                                 reinterpret_cast<CComponent**>(&m_pShaderCom))))
   return ;
  m_pModelCom->Set_InstanceBuffer(worldmat);
}

HRESULT CNonAni::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(
            m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;
 
    return S_OK;
}

CNonAni* CNonAni::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNonAni* pInstance = new CNonAni(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CNonAni");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNonAni::Clone(void* pArg)
{
    CNonAni* pInstance = new CNonAni(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CNonAni");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNonAni::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
