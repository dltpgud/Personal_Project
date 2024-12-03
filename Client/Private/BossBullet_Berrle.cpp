#include "stdafx.h"
#include "BossBullet_Berrle.h"
#include "GameInstance.h"
#include "BossBullet_Laser.h"
#include "BillyBoom.h"
CBossBullet_Berrle::CBossBullet_Berrle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CSkill{pDevice, pContext}
{
}

CBossBullet_Berrle::CBossBullet_Berrle(const CBossBullet_Berrle& Prototype) : CSkill{Prototype}
{
}

HRESULT CBossBullet_Berrle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossBullet_Berrle::Initialize(void* pArg)
{

   CBossBullet_Berrle_DESC* pDesc = static_cast<CBossBullet_Berrle_DESC*>(pArg);
      m_vPos = pDesc->vPos;
      m_pTagetPos = pDesc->pTagetPos;
      m_LaserpSocketMatrix = pDesc->LaserpSocketMatrix;
      m_LaserpParentMatrix = pDesc->LaserpParentMatrix;
      m_LaserRightLeft = pDesc->LaserRightLeft;
      m_pParentState = pDesc->state;
      if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;
      

    if (FAILED(Add_Components()))
        return E_FAIL;




    if (FAILED(Initialize_SkillType()))
        return E_FAIL;

    return S_OK;
}

_int CBossBullet_Berrle::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
    {
     return OBJ_DEAD;
    }  

    __super::Priority_Update(fTimeDelta);
    
    if (*m_pParentState == CBillyBoom::ST_Comp_Idle)
        m_bDead = true;

    if (m_fScale == 6.f)
    {
        CBossBullet_Laser::CBossBullet_Laser_DESC lDesc{};
        lDesc.fSpeedPerSec = 20.f;
        lDesc.fRotationPerSec = XMConvertToRadians(120.f);
        lDesc.iSkillType = CSkill::STYPE_LASER;
        lDesc.bRightLeft = m_LaserRightLeft;
        lDesc.fDamage = m_pDamage;
        lDesc.pParentMatrix = m_LaserpParentMatrix;
        lDesc.pSocketMatrix = m_LaserpSocketMatrix;
        lDesc.state = m_pParentState;
        m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), CGameObject::SKILL, L"Prototype GameObject_BossBullet_Laser", nullptr, 0, &lDesc, 0);

      

        m_bDead = true;
    }
    if(CSkill::SKill::STYPE_BERRLE == m_iSkillType)
     m_pTransformCom->GO_Dir(fTimeDelta, m_vDir, m_pNavigationCom, &m_bMoveStop);


    if (CSkill::SKill::STYPE_LASER == m_iSkillType)
    {
        m_fScale += 6.f*fTimeDelta;
        if (m_fScale > 4.f && m_fScale != 6.f)
            m_fScale = 4.f;
        m_pTransformCom->Set_Scaling(m_fScale, m_fScale, m_fScale);



        if (m_fScale == 4.f)
            m_fScale = 6.f;
        m_pTransformCom->Set_Scaling(m_fScale, m_fScale, m_fScale);



      
    }


    return OBJ_NOEVENT;
}

void CBossBullet_Berrle::Update(_float fTimeDelta)
{

    if (m_iSkillType == CSkill::SKill::STYPE_LASER)
    {
        _matrix		SocketMatrix = XMLoadFloat4x4(m_LaserpSocketMatrix);

        for (size_t i = 0; i < 3; i++)
            SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

        XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_LaserpParentMatrix));
    }
}

void CBossBullet_Berrle::Late_Update(_float fTimeDelta)
{
 

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
        return;

    if (m_iSkillType == CSkill::STYPE_LASER)
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
            return;
    }
    __super::Late_Update(fTimeDelta);
}

HRESULT CBossBullet_Berrle::Render()
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

void CBossBullet_Berrle::Dead_Rutine(_float fTimeDelta)
{
    m_bDead = true;
}

HRESULT CBossBullet_Berrle::Initialize_SkillType()
{
    switch (m_iSkillType)
    {
    case CSkill::SKill::STYPE_BERRLE :

        m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, m_vPos);

       _vector Dir = m_pTagetPos - m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
       Dir = XMVectorSetW(Dir, 0.f);
       m_vDir = XMVector3Normalize(Dir);
        m_Clolor[CSkill::COLOR::CSTART] = { 0.f, 0.f, 0.f,0.5f };
        m_Clolor[CSkill::COLOR::CEND] = { 1.f, 0.f, 0.f,1.f };

        m_pTransformCom->Set_Scaling(3.f, 3.f, 3.f);
        m_pNavigationCom->Find_CurrentCell(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION));

        break;


    case CSkill::SKill::STYPE_LASER : 

        m_Clolor[CSkill::COLOR::CSTART] = { 0.5f, 0.8f, 0.f,0.5f };
        m_Clolor[CSkill::COLOR::CEND] = { 1.f, 1.f, 1.f,1.f };


        m_fScale = 1.f;

        m_pTransformCom->Set_Scaling(m_fScale, m_fScale, m_fScale);

        if (m_LaserRightLeft == true) {

            m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(3.f, -0.7f, 0.f, 1.f));
        }
        else
        if (m_LaserRightLeft == false) {

            m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(-3.f, 0.7f, 0.f, 1.f));
        }

        break;

    default:
        break;
    }


    return S_OK;
}

HRESULT CBossBullet_Berrle::Add_Components()
{

    if (CSkill::SKill::STYPE_BERRLE) {
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
            reinterpret_cast<CComponent**>(&m_pNavigationCom))))
            return E_FAIL;
    }

;
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Model_FireBallSP"),
        TEXT("Com_Sphere"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBossBullet_Berrle::Bind_ShaderResources()
{

    if (m_iSkillType == CSkill::SKill::STYPE_BERRLE) {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;
    }
    if (m_iSkillType == CSkill::SKill::STYPE_LASER)
    {
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
            return E_FAIL;
    }


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




CBossBullet_Berrle* CBossBullet_Berrle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBossBullet_Berrle* pInstance = new CBossBullet_Berrle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBossBullet_Berrle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBossBullet_Berrle::Clone(void* pArg)
{
    CBossBullet_Berrle* pInstance = new CBossBullet_Berrle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBossBullet_Berrle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBossBullet_Berrle::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);

}
