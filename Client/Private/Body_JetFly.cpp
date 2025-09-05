#include "stdafx.h"
#include "JetFly.h"
#include "GameInstance.h"
#include "Body_JetFly.h"
#include "Bullet.h"
#include "StateMachine.h"
#include "JetFly_Idle.h"
#include "JetFly_Hit.h"
#include "JetFly_Dead.h"
#include "JetFly_Attack.h"
#include "JetFly_Move.h"
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
    m_pParentObj = pDesc->pParentObj;
    m_pParentState = pDesc->pParentState;
    m_RimDesc.eState = pDesc->pRimState;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fDeadTime = 2.5f;
    m_pSocketMatrix = Get_SocketMatrix("Canon_Scale");

    if (FAILED(Set_StateMachine()))
        return E_FAIL;

  
    return S_OK;
}

void CBody_JetFly::Priority_Update(_float fTimeDelta)
{
 
}

void CBody_JetFly::Update(_float fTimeDelta)
{

    if (CStateMachine::Result::Finished == m_pStateMachine[*m_pParentState]->StateMachine_Playing(fTimeDelta, &m_RimDesc))
    {
        if (m_pStateMachine[*m_pParentState]->Get_NextMachineIndex() != -1)
            ChangeState(m_pStateMachine[*m_pParentState]->Get_NextMachineIndex());
        else
            ChangeState(CJetFly::ST_IDLE);
    }

    __super::Update(fTimeDelta);
}

void CBody_JetFly::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
 
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
        return;

    if (true == m_pGameInstance->isIn_Frustum_WorldSpace(XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, m_WorldMatrix._44), 1.5f))
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
    } 
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

HRESULT CBody_JetFly::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(5)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CBody_JetFly::ChangeState(_int nextState)
{
    if (m_pStateMachine[*m_pParentState])
        m_pStateMachine[*m_pParentState]->Reset_StateMachine(&m_RimDesc);

    *m_pParentState = nextState;

    if (m_pStateMachine[*m_pParentState])
        m_pStateMachine[*m_pParentState]->StateMachine_Playing(0.f, &m_RimDesc);
}

HRESULT CBody_JetFly::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Proto_Component_JetFly_Model"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
        TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_JetFly::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_RimPow", &m_RimDesc.iPower, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_RimDesc.fcolor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_TagetDeadBool", &m_bDeadState, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_maskTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_threshold", &m_fthreshold, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_JetFly::Set_StateMachine()
{
    m_pStateMachine.resize(CJetFly::ST_END);

#pragma region IDLE
    CJetFly_Idle::STATEMACHINE_DESC pMachineDesc{};
    pMachineDesc.pParentModel = m_pModelCom;
    pMachineDesc.iNextMachineIdx = CJetFly::ST_IDLE;
    m_pStateMachine[CJetFly::ST_IDLE] = CJetFly_Idle::Create(&pMachineDesc);
#pragma endregion

#pragma region Attack
    CJetFly_Attack::ATTACK_DESC pAttackDesc{};
    pAttackDesc.pParentModel = m_pModelCom;
    pAttackDesc.pParentPartObject = this;
    pAttackDesc.pParentObject = m_pParentObj;
    pAttackDesc.pPerantPartBonMatrix = m_pSocketMatrix;
    pAttackDesc.pPerantWorldMat = &m_WorldMatrix;
    pAttackDesc.iNextMachineIdx = CJetFly::ST_MOVE;
    pAttackDesc.fLength = static_cast<CJetFly*>(m_pParentObj)->Get_fAttackLength();
    m_pStateMachine[CJetFly::ST_SHOOT] = CJetFly_Attack::Create(&pAttackDesc);
#pragma endregion

#pragma region Move
    CJetFly_Move::MOVE_DESC pMoveDesc{};
    pMoveDesc.pParentModel = m_pModelCom;
    pMoveDesc.pParentObject = m_pParentObj;
    pMoveDesc.iNextMachineIdx = CJetFly::ST_SHOOT;
    pMoveDesc.fLength = static_cast<CJetFly*>(m_pParentObj)->Get_fAttackLength();
    m_pStateMachine[CJetFly::ST_MOVE] = CJetFly_Move::Create(&pMoveDesc);
#pragma endregion

#pragma region Dead
    CJetFly_Dead::DEAD_DESC pDeadDesc{};
    pDeadDesc.pParentModel = m_pModelCom;
    pDeadDesc.pParentObject = m_pParentObj;
    pDeadDesc.pParentPartObject = this;
    m_pStateMachine[CJetFly::ST_DEAD] = CJetFly_Dead::Create(&pDeadDesc);
#pragma endregion

#pragma region Hit
    CJetFly_Hit::HIT_DESC pHitDesc{};
    pHitDesc.pParentModel = m_pModelCom;
    pHitDesc.iNextMachineIdx = CJetFly::ST_SHOOT;
    m_pStateMachine[CJetFly::ST_HIT] = CJetFly_Hit::Create(&pHitDesc);
#pragma endregion

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
    for (auto& Machine : m_pStateMachine) Safe_Release(Machine);
    m_pStateMachine.clear();
    m_pStateMachine.shrink_to_fit();
}
