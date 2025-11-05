#include "stdafx.h"
#include "BossBullet_Laser.h"
#include "GameInstance.h"
#include "BillyBoom.h"

_int CBossBullet_Laser::iContinuous = 1;

CBossBullet_Laser::CBossBullet_Laser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSkill{pDevice, pContext}
{
    m_DecalDesc.bNormal = true;
    m_DecalDesc.fSize = 2.f;
    m_DecalDesc.fLifeTime = 5.f;
    m_DecalDesc.iTexIndex = 1;
    m_DecalDesc.bOnce = false;
    m_DecalDesc.Key = TEXT("Base");
}

CBossBullet_Laser::CBossBullet_Laser(const CBossBullet_Laser& Prototype)
    : CSkill{Prototype}
{

}
    
HRESULT CBossBullet_Laser::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossBullet_Laser::Initialize(void* pArg)
{
   CBossBullet_Laser_DESC* pDesc = static_cast<CBossBullet_Laser_DESC*>(pArg); 
   m_bRightLeft = pDesc->bRightLeft;
   m_pSocketMatrix = pDesc->pSocketMatrix;
   m_pParentMatrix = pDesc->pParentMatrix;

    if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;

   
    if (m_bRightLeft == true) {
        m_pTransformCom->Rotation(XMConvertToRadians(0.0f), XMConvertToRadians(-90.0f), XMConvertToRadians(0.0f));
        m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(1.f, -0.7f, 0.f, 1.f));
    }
     else 
     if (m_bRightLeft == false) {
         m_pTransformCom->Rotation(XMConvertToRadians(0.0f), XMConvertToRadians(90.0f), XMConvertToRadians(0.0f));
         m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(-1.f, 0.7f, 0.f, 1.f));
     }

    m_Color[CSkill::COLOR::CSTART] = { 0.f, 0.f, 0.f,0.5f };
    m_Color[CSkill::COLOR::CEND] = { 1.f, 0.f, 0.f,1.f };

    m_pTransformCom->Set_Scaling(0.3f, 0.3f, 0.9f);

    m_DecalDesc.iContinuous = iContinuous++;
    return S_OK;
}

void CBossBullet_Laser::Priority_Update(_float fTimeDelta)
{
    m_fTimeSum += fTimeDelta;
    __super::Priority_Update(fTimeDelta);
}

void CBossBullet_Laser::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
}

void CBossBullet_Laser::Late_Update(_float fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; i++) SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    XMStoreFloat4x4(&m_WorldMatrix,
                    m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

    m_DecalDesc.vPos = XMVector3TransformCoord(XMVectorSet(0.f,0.f,0.f,1.f), SocketMatrix * XMLoadFloat4x4(m_pParentMatrix)); // 발사 위치

    _vector Rot;
    if (m_bRightLeft == true)
        Rot = XMVectorSet(1.f, 0.f, 0.f, 0.f);
    else
        Rot = XMVectorSet(-1.f, 0.f, 0.f, 0.f);

    m_DecalDesc.vDir =  XMVector3Normalize(XMVector3TransformNormal(Rot, SocketMatrix * XMLoadFloat4x4(m_pParentMatrix))); // 발사 방향

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;


    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
        return;
    if (FAILED(m_pGameInstance->Add_GameObject_To_ColGroup(this, Collider_Manager::CollGroup::COL_MONSTER_SKILL)))
        return;
  
    __super::Late_Update(fTimeDelta);
}

void CBossBullet_Laser::Dead_Rutine()
{
    m_iLifeState = OBJ_DEAD;
}

HRESULT CBossBullet_Laser::Render()
{
    if (FAILED(Bind_ShaderResources()))
         return E_FAIL;

    _uint iNumMesh = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMesh; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
            "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(6)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBossBullet_Laser::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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
        if (FAILED(m_pShaderCom->Begin(2)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
    return S_OK;
}

HRESULT CBossBullet_Laser::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
        reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_Beam"),
        TEXT("Com_Beam"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Proto Component Fire1_Terrain"),
        TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    CBounding_OBB::BOUND_OBB_DESC		OBBDesc{};
    OBBDesc.vRotation = { 0.f,0.f,0.f };
    OBBDesc.vExtents = _float3(2.f, 2.f, 14.f);
    OBBDesc.vCenter = _float3(0.f, 0.5f, -10.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBossBullet_Laser::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_RGB", &m_Color[CSkill::COLOR::CSTART], sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_RGBEnd", &m_Color[CSkill::COLOR::CEND], sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_TimeSum", &m_fTimeSum, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_maskTexture", 0)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;
    return S_OK;
}

CBossBullet_Laser* CBossBullet_Laser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBossBullet_Laser* pInstance = new CBossBullet_Laser(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBossBullet_Laser");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBossBullet_Laser::Clone(void* pArg)
{
    CBossBullet_Laser* pInstance = new CBossBullet_Laser(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBossBullet_Laser");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBossBullet_Laser::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pTextureCom);
}
