#include "stdafx.h"
#include "Wall.h"
#include "GameInstance.h"

CWall::CWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CWall::CWall(const CWall& Prototype) : CGameObject{Prototype}
{
}

HRESULT CWall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWall::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

     
   
    return S_OK;
}

_int CWall::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        return OBJ_DEAD;
    }

    return OBJ_NOEVENT;

}

void CWall::Update(_float fTimeDelta)
{
}

void CWall::Late_Update(_float fTimeDelta)
{


if (false == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION), 15.f))
     return ;

        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
      
     if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
        return;
   
}
HRESULT CWall::Render()
{
 
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMesh = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMesh; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

       

            if (m_pModelName == L"Proto Component DoorRock Model_Wall") {
          
                if (i == 0) {
                    m_bEmissive = true;
                    if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                        "g_EmissiveTexture")))
                        return E_FAIL;
                }
                else
                    m_bEmissive = false;


                if (FAILED(m_pShaderCom->Bind_RawValue("g_DoorEmissiveColor", &m_fDoorEmissiveColor, sizeof(_float4))))
                    return E_FAIL;

              
            }
            else
                m_bEmissive = false;
          
            if (FAILED(m_pShaderCom->Bind_Bool("g_bDoorEmissive", m_bEmissive)))
                    return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(6)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
    return S_OK;
}

HRESULT CWall::Render_Shadow()
{

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

void CWall::Set_Model(const _wstring& protoModel, _uint ILevel)
{
    m_pModelName = protoModel;
    if (FAILED(__super::Add_Component(ILevel, protoModel, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        MSG_BOX("Set_Model failed");
        return;
    }
    if (ILevel == LEVEL_STAGE1)
        m_fDoorEmissiveColor = { 1.f,0.749f,0.2156f, 1.f };
    else if (ILevel == LEVEL_STAGE2)
        m_fDoorEmissiveColor = { 1.f,0.f,0.f, 1.f };

   // if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOWBG, this)))
   //     return ;
}



HRESULT CWall::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWall::Bind_ShaderResources()
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


    return S_OK;
}

CWall* CWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWall* pInstance = new CWall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWall::Clone(void* pArg)
{
    CWall* pInstance = new CWall(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CWall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWall::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
