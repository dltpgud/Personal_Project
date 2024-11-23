#include "stdafx.h"
#include "BossBullet_Missile.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Bullet.h"
CBossBullet_Missile::CBossBullet_Missile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSkill{pDevice, pContext}
{
}

CBossBullet_Missile::CBossBullet_Missile(const CBossBullet_Missile& Prototype) : CSkill{Prototype}
{
}

HRESULT CBossBullet_Missile::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossBullet_Missile::Initialize(void* pArg)
{

    CBossBullet_Missile_DESC* pDesc = static_cast<CBossBullet_Missile_DESC*>(pArg);
      m_pTagetPos = pDesc->pTagetPos;

      if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;


    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, m_vPos);

    _vector Dir = m_pTagetPos - m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
    Dir = XMVectorSetW(Dir, 0.f);
    m_vDir = XMVector3Normalize(Dir);


    m_pTransformCom->Set_Scaling(3.f, 3.f, 3.f);
    m_pNavigationCom->Find_CurrentCell(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION));





    return S_OK;
}

_int CBossBullet_Missile::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        return OBJ_DEAD;
    }  

    __super::Priority_Update(fTimeDelta);




    m_pTransformCom->GO_Dir(fTimeDelta, m_vDir, m_pNavigationCom, &m_bMoveStop);

   


    return OBJ_NOEVENT;

}

void CBossBullet_Missile::Update(_float fTimeDelta)
{

}

void CBossBullet_Missile::Late_Update(_float fTimeDelta)
{


    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
        return;
    __super::Late_Update(fTimeDelta);
}

HRESULT CBossBullet_Missile::Render()
{
     if (FAILED(Bind_ShaderResources()))
         return E_FAIL;

    _uint iNumMesh = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMesh; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
            "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(3)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
    return S_OK;

}

HRESULT CBossBullet_Missile::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
        reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;

;
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FireBallSP"),
        TEXT("Com_Sphere"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBossBullet_Missile::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_RGB", &m_Clolor[CSkill::COLOR::CSTART], sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_RGBEnd", &m_Clolor[CSkill::COLOR::CEND], sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_TimeSum", &m_fTimeSum, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}




CBossBullet_Missile* CBossBullet_Missile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBossBullet_Missile* pInstance = new CBossBullet_Missile(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBossBullet_Missile");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBossBullet_Missile::Clone(void* pArg)
{
    CBossBullet_Missile* pInstance = new CBossBullet_Missile(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBossBullet_Berrle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBossBullet_Missile::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
}
