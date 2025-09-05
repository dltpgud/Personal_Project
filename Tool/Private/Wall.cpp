#include "stdafx.h"
#include "Wall.h"
#include "GameInstance.h"

CWALL::CWALL(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CWALL::CWALL(const CWALL& Prototype) : CGameObject{Prototype}
{
}

HRESULT CWALL::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWALL::Initialize(void* pArg)
{
    GAMEOBJ_DESC* pDesc = static_cast<GAMEOBJ_DESC*>(pArg);
    m_iObjectType = pDesc->Object_Type;
    

    size_t iLen = wcslen(pDesc->ProtoName) + 1;
    m_Proto = new wchar_t[iLen];
    lstrcpyW(m_Proto, pDesc->ProtoName);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CWALL::Priority_Update(_float fTimeDelta)
{

    __super::Priority_Update(fTimeDelta);

}

void CWALL::Update(_float fTimeDelta)
{

    __super::Update(fTimeDelta);

}

void CWALL::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
        return;
    __super::Late_Update(fTimeDelta);
}

HRESULT CWALL::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMesh = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMesh; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
   // __super::Render();
    return S_OK;
}

HRESULT CWALL::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix(
            "g_ViewMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::TRANSFORM_STATE::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix(
            "g_ProjMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::TRANSFORM_STATE::D3DTS_PROJ))))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {

        if (FAILED(m_pShaderCom->Begin(3)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
    return S_OK;
}

void CWALL::Set_Model(const _wstring& protoModel, _uint ILevel)
{
    m_wModel = protoModel;
    if (FAILED(__super::Add_Component(LEVEL_STATIC, m_wModel, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        MSG_BOX("Set_Model failed");
        return;
    }


    CBounding_OBB::BOUND_OBB_DESC		OBBDesc{};
    m_pModelCom->Center_Ext(&OBBDesc.vCenter, &OBBDesc.vExtents);
    OBBDesc.vRotation = { 0.f,0.f,0.f };
    //AABBDesc.vExtents = _float3(0.5f, 0.75f, 0.5f);
    //AABBDesc.vCenter = _float3(0.f, 0.5f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return;
}

_tchar* CWALL::Get_ProtoName()
{
    return m_Proto;
}

HRESULT CWALL::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWALL::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(
            m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(
            m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CWALL* CWALL::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWALL* pInstance = new CWALL(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Cfabric");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWALL::Clone(void* pArg)
{
    CWALL* pInstance = new CWALL(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CWALL");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWALL::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

        if (m_bClone)
            Safe_Delete_Array(m_Proto);
}
