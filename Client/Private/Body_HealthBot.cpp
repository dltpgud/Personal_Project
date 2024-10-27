#include "stdafx.h"
#include "Body_HealthBot.h"
#include "GameInstance.h"
#include "HealthBot.h"

CBody_HealthBot::CBody_HealthBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{pDevice, pContext}
{
}

CBody_HealthBot::CBody_HealthBot(const CBody_HealthBot& Prototype) : CPartObject{Prototype}
{
}

HRESULT CBody_HealthBot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_HealthBot::Initialize(void* pArg)
{

    BODY_HEALTHBOT_DESC* pDesc = static_cast<BODY_HEALTHBOT_DESC*>(pArg);

    m_pParentState = pDesc->pParentState;

    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

_int CBody_HealthBot::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CBody_HealthBot::Update(_float fTimeDelta)
{
    _bool bMotionChange = {false}, bLoop = {false};


    if (*m_pParentState == CHealthBot::ST_Dead && m_iCurMotion != CHealthBot::ST_Dead)
    {
        m_iCurMotion = CHealthBot::ST_Dead;
        return;
    }

    if (*m_pParentState == CHealthBot::ST_Interactive && m_iCurMotion != CHealthBot::ST_Interactive)
    {
        m_iCurMotion = CHealthBot::ST_Interactive;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CHealthBot::ST_Idle && m_iCurMotion != CHealthBot::ST_Idle)
    {
        m_iCurMotion = CHealthBot::ST_Idle;
        bMotionChange = true;
        bLoop = false;
    }
    if (bMotionChange)
        m_pModelCom->Set_Animation(m_iCurMotion, bLoop);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {

 
         m_bFinishAni = true;

        if(m_iCurMotion == CHealthBot::ST_Idle)
         m_pModelCom->Set_Animation(m_iCurMotion, true);
    }
    else
    {
        m_bFinishAni = false;
    }
}

void CBody_HealthBot::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CBody_HealthBot::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {

        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

       
        if (FAILED(m_pShaderCom->Bind_Bool("g_TagetDeadBool", m_iCurMotion == CHealthBot::ST_Dead)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody_HealthBot::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Proto Component HealthBot_Model"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_HealthBot::Bind_ShaderResources()
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

    return S_OK;
}

CBody_HealthBot* CBody_HealthBot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_HealthBot* pInstance = new CBody_HealthBot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_HealthBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_HealthBot::Clone(void* pArg)
{
    CBody_HealthBot* pInstance = new CBody_HealthBot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBody_HealthBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_HealthBot::Free()
{
    __super::Free();
}
