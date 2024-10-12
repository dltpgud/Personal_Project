#include "stdafx.h"
#include "fabric.h"
#include "GameInstance.h"

Cfabric::Cfabric(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

Cfabric::Cfabric(const Cfabric& Prototype) : CGameObject{Prototype}
{
}

HRESULT Cfabric::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Cfabric::Initialize(void* pArg)
{
    GAMEOBJ_DESC* pDesc = static_cast<GAMEOBJ_DESC*>(pArg);
    m_DATA_TYPE = pDesc->DATA_TYPE;
    

    size_t iLen = wcslen(pDesc->ProtoName) + 1;
    m_Proto = new wchar_t[iLen];
    lstrcpyW(m_Proto, pDesc->ProtoName);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

_int Cfabric::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        return OBJ_DEAD;
    }

    _float3 Center;
    XMStoreFloat3(&Center, m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION));
    pBox.Center = Center;
    pBox.Extents = { 1,1,1 };

    return OBJ_NOEVENT;
}

void Cfabric::Update(_float fTimeDelta)
{
}

void Cfabric::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT Cfabric::Render()
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
    return S_OK;
}

void Cfabric::Set_Model(const _wstring& protoModel)
{
    m_wModel = protoModel;
    if (FAILED(__super::Add_Component(LEVEL_STATIC, m_wModel, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        MSG_BOX("Set_Model failed");
        return;
    }
}

_tchar* Cfabric::Get_ProtoName()
{
    return m_Proto;
}

_float Cfabric::check_BoxDist(_vector RayPos, _vector RayDir)
{
    _matrix matWorld = m_pTransformCom->Get_WorldMatrix_Inverse();
   
    _vector CurRayPos = XMVector3TransformCoord(RayPos, matWorld);
    _vector CurRayDir = XMVector3TransformNormal(RayDir, matWorld);
    CurRayDir = XMVector3Normalize(CurRayDir);


    //RayDir = XMVector3Normalize(RayDir);
    _float Dist{};
    if (pBox.Intersects(RayPos, RayDir, Dist))
    {
        return Dist;
    }

    return _float(0xffff);
}



HRESULT Cfabric::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT Cfabric::Bind_ShaderResources()
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

Cfabric* Cfabric::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    Cfabric* pInstance = new Cfabric(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Cfabric");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* Cfabric::Clone(void* pArg)
{
    Cfabric* pInstance = new Cfabric(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : Cfabric");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Cfabric::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

        if (m_bClone)
            Safe_Delete_Array(m_Proto);
}
