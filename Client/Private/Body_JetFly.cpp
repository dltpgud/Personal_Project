#include "stdafx.h"
#include "JetFly.h"
#include "GameInstance.h"
#include "Body_JetFly.h"

CBody_JetFly::CBody_JetFly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{pDevice, pContext}
{
}

CBody_JetFly::CBody_JetFly(const CBody_JetFly& Prototype) : CPartObject{Prototype}
{
}

HRESULT CBody_JetFly::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_JetFly::Initialize(void* pArg)
{

    CBody_JetFly_Desc* pDesc = static_cast<CBody_JetFly_Desc*>(pArg);

    m_pParentState = pDesc->pParentState;
    m_RimDesc.eState = pDesc->pRimState;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
    m_fPlayAniTime = 0.5f;
    return S_OK;
}

_int CBody_JetFly::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CBody_JetFly::Update(_float fTimeDelta)
{
    _bool bMotionChange = {false}, bLoop = {false};

    if (*m_pParentState == CJetFly::ST_Idle && m_iCurMotion != CJetFly::ST_Idle)
    {
        m_iCurMotion = CJetFly::ST_Idle;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CJetFly::ST_BarrelRoll_Left && m_iCurMotion != CJetFly::ST_BarrelRoll_Left)
    {
        m_iCurMotion = CJetFly::ST_BarrelRoll_Left;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CJetFly::ST_BarrelRoll_Right && m_iCurMotion != CJetFly::ST_BarrelRoll_Right)
    {
        m_iCurMotion = CJetFly::ST_BarrelRoll_Right;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CJetFly::ST_Hit_Back && m_iCurMotion != CJetFly::ST_Hit_Back)
    {
        m_iCurMotion = CJetFly::ST_Hit_Back;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CJetFly::ST_Hit_Front && m_iCurMotion != CJetFly::ST_Hit_Front)
    {
        m_iCurMotion = CJetFly::ST_Hit_Front;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CJetFly::ST_Hit_Left && m_iCurMotion != CJetFly::ST_Hit_Left)
    {
        m_iCurMotion = CJetFly::ST_Hit_Left;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CJetFly::ST_Hit_Right && m_iCurMotion != CJetFly::ST_Hit_Right)
    {
        m_iCurMotion = CJetFly::ST_Hit_Right;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CJetFly::ST_Shoot && m_iCurMotion != CJetFly::ST_Shoot)
    {
        m_iCurMotion = CJetFly::ST_Shoot;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CJetFly::ST_Sragger && m_iCurMotion != CJetFly::ST_Sragger)
    {
        m_iCurMotion = CJetFly::ST_Sragger;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }
  

    if (*m_pParentState == CJetFly::ST_Walk_Back && m_iCurMotion != CJetFly::ST_Walk_Back)
    {
        m_iCurMotion = CJetFly::ST_Walk_Back;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CJetFly::ST_Walk_Front && m_iCurMotion != CJetFly::ST_Walk_Front)
    {
        m_iCurMotion = CJetFly::ST_Walk_Front;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CJetFly::ST_Walk_Left && m_iCurMotion != CJetFly::ST_Walk_Left)
    {
        m_iCurMotion = CJetFly::ST_Walk_Left;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CJetFly::ST_Walk_Right && m_iCurMotion != CJetFly::ST_Walk_Right)
    {
        m_iCurMotion = CJetFly::ST_Walk_Right;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }



    if (*m_RimDesc.eState == RIM_LIGHT_DESC::STATE_RIM)
    {
        m_RimDesc.fcolor = { 1.f,1.f,1.f,1.f };
        m_RimDesc.iPower = 1;
    }

    if (*m_RimDesc.eState == RIM_LIGHT_DESC::STATE_NORIM) {
        m_RimDesc.fcolor = { 0.f,0.f,0.f,0.f };
        m_RimDesc.iPower = 1;
    }


    if (bMotionChange)
        m_pModelCom->Set_Animation(m_iCurMotion, bLoop);

    if (true == m_pModelCom->Play_Animation(fTimeDelta* m_fPlayAniTime))
    {

        m_bFinishAni = true;
        m_iCurMotion = CJetFly::ST_Idle;
        m_pModelCom->Set_Animation(m_iCurMotion, true);
    }
    else
    {
        m_bFinishAni = false;
        if (m_iCurMotion == CJetFly::ST_Hit_Front)
            m_pModelCom->Set_Animation(m_iCurMotion, false);
    }
}

void CBody_JetFly::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CBody_JetFly::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {

        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody_JetFly::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Proto_Component_JetFly_Model"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_JetFly::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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


    if (FAILED(m_pShaderCom->Bind_Bool("g_TagetBool",m_RimDesc.eState)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Int("g_RimPow", m_RimDesc.iPower)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_RimDesc.fcolor, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Bool("g_TagetDeadBool", m_iCurMotion == CJetFly::ST_Hit_Front)))
        return E_FAIL;
    return S_OK;
}

CBody_JetFly* CBody_JetFly::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_JetFly* pInstance = new CBody_JetFly(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_JetFly");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_JetFly::Clone(void* pArg)
{
    CBody_JetFly* pInstance = new CBody_JetFly(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBody_JetFly");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_JetFly::Free()
{
    __super::Free();
}
