#include "stdafx.h"
#include "Body_GunPawn.h"
#include "GameInstance.h"
#include "GunPawn.h"

CBody_GunPawn::CBody_GunPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{pDevice, pContext}
{
}

CBody_GunPawn::CBody_GunPawn(const CBody_GunPawn& Prototype) : CPartObject{Prototype}
{
}

HRESULT CBody_GunPawn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_GunPawn::Initialize(void* pArg)
{

    BODY_GUNPAWN_DESC* pDesc = static_cast<BODY_GUNPAWN_DESC*>(pArg);

    m_pParentState = pDesc->pParentState;

    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

_int CBody_GunPawn::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CBody_GunPawn::Update(_float fTimeDelta)
{
    _bool bMotionChange = {false}, bLoop = {false};

    if (*m_pParentState == CGunPawn::ST_GRENADE_PRESHOOT && m_iCurMotion != CGunPawn::ST_GRENADE_PRESHOOT)
    {
        m_iCurMotion = CGunPawn::ST_GRENADE_PRESHOOT;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CGunPawn::ST_JETPACK && m_iCurMotion != CGunPawn::ST_JETPACK)
    {
        m_iCurMotion = CGunPawn::ST_JETPACK;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_SHOOT03 && m_iCurMotion != CGunPawn::ST_SHOOT03)
    {
        m_iCurMotion = CGunPawn::ST_SHOOT03;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_STURN_LOOP && m_iCurMotion != CGunPawn::ST_STURN_LOOP)
    {
        m_iCurMotion = CGunPawn::ST_STURN_LOOP;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_RUN_BACK && m_iCurMotion != CGunPawn::ST_RUN_BACK)
    {
        m_iCurMotion = CGunPawn::ST_RUN_BACK;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_RUN_BACK_LEFT && m_iCurMotion != CGunPawn::ST_RUN_BACK_LEFT)
    {
        m_iCurMotion = CGunPawn::ST_RUN_BACK_LEFT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_RUN_BACK_RIGHT && m_iCurMotion != CGunPawn::ST_RUN_BACK_RIGHT)
    {
        m_iCurMotion = CGunPawn::ST_RUN_BACK_RIGHT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_RUN_LEFT && m_iCurMotion != CGunPawn::ST_RUN_LEFT)
    {
        m_iCurMotion = CGunPawn::ST_RUN_LEFT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_RUN_RIGHT && m_iCurMotion != CGunPawn::ST_RUN_RIGHT)
    {
        m_iCurMotion = CGunPawn::ST_RUN_RIGHT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_STAGGER_BACK && m_iCurMotion != CGunPawn::ST_STAGGER_BACK)
    {
        m_iCurMotion = CGunPawn::ST_STAGGER_BACK;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_STAGGER_FRONT && m_iCurMotion != CGunPawn::ST_STAGGER_FRONT)
    {
        m_iCurMotion = CGunPawn::ST_STAGGER_FRONT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_STAGGER_LEFT && m_iCurMotion != CGunPawn::ST_STAGGER_LEFT)
    {
        m_iCurMotion = CGunPawn::ST_STAGGER_LEFT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_STAGGER_RIGHT && m_iCurMotion != CGunPawn::ST_STAGGER_RIGHT)
    {
        m_iCurMotion = CGunPawn::ST_STAGGER_RIGHT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_STUN_START && m_iCurMotion != CGunPawn::ST_STUN_START)
    {
        m_iCurMotion = CGunPawn::ST_STUN_START;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CGunPawn::ST_HIT_BACK && m_iCurMotion != CGunPawn::ST_HIT_BACK)
    {
        m_iCurMotion = CGunPawn::ST_HIT_BACK;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_HIT_FRONT && m_iCurMotion != CGunPawn::ST_HIT_FRONT)
    {
        m_iCurMotion = CGunPawn::ST_HIT_FRONT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_HIT_LEFT && m_iCurMotion != CGunPawn::ST_HIT_LEFT)
    {
        m_iCurMotion = CGunPawn::ST_HIT_LEFT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_HIT_RIGHT && m_iCurMotion != CGunPawn::ST_HIT_RIGHT)
    {
        m_iCurMotion = CGunPawn::ST_HIT_RIGHT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_IDLE && m_iCurMotion != CGunPawn::ST_IDLE)
    {
        m_iCurMotion = CGunPawn::ST_IDLE;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_GRENADE_SHOOT && m_iCurMotion != CGunPawn::ST_GRENADE_SHOOT)
    {
        m_iCurMotion = CGunPawn::ST_GRENADE_SHOOT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_RUN_BACK_FRONT && m_iCurMotion != CGunPawn::ST_RUN_BACK_FRONT)
    {
        m_iCurMotion = CGunPawn::ST_RUN_BACK_FRONT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_SHOOT01 && m_iCurMotion != CGunPawn::ST_SHOOT01)
    {
        m_iCurMotion = CGunPawn::ST_SHOOT01;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_PRESHOOT && m_iCurMotion != CGunPawn::ST_PRESHOOT)
    {
        m_iCurMotion = CGunPawn::ST_PRESHOOT;
        bMotionChange = true;
        bLoop = true;
    }
    if (bMotionChange)
        m_pModelCom->Set_Animation(m_iCurMotion, bLoop);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        
        m_bFinishAni = true;
        m_iCurMotion = CGunPawn::ST_IDLE;
        m_pModelCom->Set_Animation(m_iCurMotion, true);

    }
    else 
    {
        m_bFinishAni = false;
    }
}

void CBody_GunPawn::Late_Update(_float fTimeDelta)
{

    XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pParentMatrix) *
                                        m_pTransformCom->Get_WorldMatrix()); // 부모행렬과 내 월드랑 곱해서 그린다


    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CBody_GunPawn::Render()
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

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

const _float4x4* CBody_GunPawn::Get_SocketMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}

HRESULT CBody_GunPawn::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Proto_Component_GunPawn_Model"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;


    return S_OK;
}

HRESULT CBody_GunPawn::Bind_ShaderResources()
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

CBody_GunPawn* CBody_GunPawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_GunPawn* pInstance = new CBody_GunPawn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_GunPawn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_GunPawn::Clone(void* pArg)
{
    CBody_GunPawn* pInstance = new CBody_GunPawn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBody_GunPawn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_GunPawn::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
