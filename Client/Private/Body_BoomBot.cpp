#include "stdafx.h"
#include "BoomBot.h"
#include "GameInstance.h"
#include "Body_BoomBot.h"

CBody_BoomBot::CBody_BoomBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{pDevice, pContext}
{
}

CBody_BoomBot::CBody_BoomBot(const CBody_BoomBot& Prototype) : CPartObject{Prototype}
{
}

HRESULT CBody_BoomBot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_BoomBot::Initialize(void* pArg)
{

    CBody_BoomBot_Desc* pDesc = static_cast<CBody_BoomBot_Desc*>(pArg);

    m_pParentState = pDesc->pParentState;

    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

_int CBody_BoomBot::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CBody_BoomBot::Update(_float fTimeDelta)
{
    _bool bMotionChange = {false}, bLoop = {false};
    if (*m_pParentState == CBoomBot::ST_Aim_Down && m_iCurMotion != CBoomBot::ST_Aim_Down)
    {
        m_iCurMotion = CBoomBot::ST_Aim_Down;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CBoomBot::ST_Aim_Down_Left&& m_iCurMotion != CBoomBot::ST_Aim_Down_Left)
    {
        m_iCurMotion = CBoomBot::ST_Aim_Down_Left;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Aim_Down_Right && m_iCurMotion != CBoomBot::ST_Aim_Down_Right)
    {
        m_iCurMotion = CBoomBot::ST_Aim_Down_Right; 
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Aim_Left&& m_iCurMotion != CBoomBot::ST_Aim_Left)
    {
        m_iCurMotion = CBoomBot::ST_Aim_Left;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Aim_Middle && m_iCurMotion != CBoomBot::ST_Aim_Middle)
    {
        m_iCurMotion = CBoomBot::ST_Aim_Middle;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Aim_Right && m_iCurMotion != CBoomBot::ST_Aim_Right)
    {
        m_iCurMotion = CBoomBot::ST_Aim_Right;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Aim_Top && m_iCurMotion != CBoomBot::ST_Aim_Top)
    {
        m_iCurMotion = CBoomBot::ST_Aim_Top;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Aim_Top_Left && m_iCurMotion != CBoomBot::ST_Aim_Top_Left)
    {
        m_iCurMotion = CBoomBot::ST_Aim_Top_Left;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Aim_Top_Right&& m_iCurMotion != CBoomBot::ST_Aim_Top_Right)
    {
        m_iCurMotion = CBoomBot::ST_Aim_Top_Right;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Hit_Back && m_iCurMotion != CBoomBot::ST_Hit_Back)
    {
        m_iCurMotion = CBoomBot::ST_Hit_Back;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Hit_Front && m_iCurMotion != CBoomBot::ST_Hit_Front)
    {
        m_iCurMotion = CBoomBot::ST_Hit_Front;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CBoomBot::ST_Hit_Left && m_iCurMotion != CBoomBot::ST_Hit_Left)
    {
        m_iCurMotion = CBoomBot::ST_Hit_Left;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Hit_Right && m_iCurMotion != CBoomBot::ST_Hit_Right)
    {
        m_iCurMotion = CBoomBot::ST_Hit_Right;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Idle && m_iCurMotion != CBoomBot::ST_Idle)
    {
        m_iCurMotion = CBoomBot::ST_Idle;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_PreShoot && m_iCurMotion != CBoomBot::ST_PreShoot)
    {
        m_iCurMotion = CBoomBot::ST_PreShoot;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Run_Back && m_iCurMotion != CBoomBot::ST_Run_Back)
    {
        m_iCurMotion = CBoomBot::ST_Run_Back;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Run_Front && m_iCurMotion != CBoomBot::ST_Run_Front)
    {
        m_iCurMotion = CBoomBot::ST_Run_Front;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Run_Left && m_iCurMotion != CBoomBot::ST_Run_Left)
    {
        m_iCurMotion = CBoomBot::ST_Run_Left;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Run_Left_Back && m_iCurMotion != CBoomBot::ST_Run_Left_Back)
    {
        m_iCurMotion = CBoomBot::ST_Run_Left_Back;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Run_Right && m_iCurMotion != CBoomBot::ST_Run_Right)
    {
        m_iCurMotion = CBoomBot::ST_Run_Right;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Run_Right_Back && m_iCurMotion != CBoomBot::ST_Run_Right_Back)
    {
        m_iCurMotion = CBoomBot::ST_Run_Right_Back;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CBoomBot::ST_Shoot && m_iCurMotion != CBoomBot::ST_Shoot)
    {
        m_iCurMotion = CBoomBot::ST_Shoot;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CBoomBot::ST_Stun_Loop && m_iCurMotion != CBoomBot::ST_Stun_Loop)
    {
        m_iCurMotion = CBoomBot::ST_Stun_Loop;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CBoomBot::ST_Stun_Start && m_iCurMotion != CBoomBot::ST_Stun_Start)
    {
        m_iCurMotion = CBoomBot::ST_Stun_Start;
        bMotionChange = true;
        bLoop = true;
    }

    if (bMotionChange)
        m_pModelCom->Set_Animation(m_iCurMotion, bLoop);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        m_bFinishAni = true;
        m_pModelCom->Set_Animation(m_iCurMotion, true);

        if (m_iCurMotion == CBoomBot::ST_Shoot)
            m_bHitAttackMotion = false;
    }
    else
    {
        if (m_iCurMotion == CBoomBot::ST_Hit_Front)
        {
            m_bHitAttackMotion = true;
        }
        m_bFinishAni = false;
        if (m_iCurMotion == CBoomBot::ST_Aim_Down)
        m_pModelCom->Set_Animation(m_iCurMotion, false);
    }
}

void CBody_BoomBot::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CBody_BoomBot::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {

        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Bool("g_TagetBool", m_iCurMotion == CBoomBot::ST_Hit_Front)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Bool("g_TagetDeadBool", m_iCurMotion == CBoomBot::ST_Aim_Down)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody_BoomBot::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Proto_Component_BoomBot_Model"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_BoomBot::Bind_ShaderResources()
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

CBody_BoomBot* CBody_BoomBot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_BoomBot* pInstance = new CBody_BoomBot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_BoomBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_BoomBot::Clone(void* pArg)
{
    CBody_BoomBot* pInstance = new CBody_BoomBot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBody_BoomBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_BoomBot::Free()
{
    __super::Free();
}
