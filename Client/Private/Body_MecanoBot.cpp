#include "stdafx.h"
#include "MecanoBot.h"
#include "GameInstance.h"
#include "Body_MecanoBot.h"
#include "Bullet.h"
#include "StateMachine.h"
#include "MecanoBot_Idle.h"
#include "MecanoBot_Hit.h"
#include "MecanoBot_Dead.h"
#include "MecanoBot_Attack.h"
#include "MecanoBot_Move.h"

CBody_MecanoBot::CBody_MecanoBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{pDevice, pContext}
{
}

CBody_MecanoBot::CBody_MecanoBot(const CBody_MecanoBot& Prototype) : CPartObject{Prototype}
{
}

HRESULT CBody_MecanoBot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_MecanoBot::Initialize(void* pArg)
{
    CBody_MecanoBot_Desc* pDesc = static_cast<CBody_MecanoBot_Desc*>(pArg);
    m_pParentObj = pDesc->pParentObj;
    m_pParentState = pDesc->pParentState;
    m_RimDesc.eState = pDesc->pRimState;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
    
    m_pSocketMatrix = Get_SocketMatrix("R_Canon_02");

    if (FAILED(Set_StateMachine()))
        return E_FAIL;

    m_fDeadTime = 0.7f;

    return S_OK;
}

void CBody_MecanoBot::Priority_Update(_float fTimeDelta)
{
  
}

void CBody_MecanoBot::Update(_float fTimeDelta)
{
    if (CStateMachine::Result::Finished == m_pStateMachine[*m_pParentState]->StateMachine_Playing(fTimeDelta, &m_RimDesc))
    {
        if (m_pStateMachine[*m_pParentState]->Get_NextMachineIndex() != -1)
            ChangeState(m_pStateMachine[*m_pParentState]->Get_NextMachineIndex());
        else
            ChangeState(CMecanoBot::ST_IDLE);
    }

    __super::Update(fTimeDelta);
}

void CBody_MecanoBot::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
   
    if (true == m_pGameInstance->isIn_Frustum_WorldSpace(XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, m_WorldMatrix._44), 1.5f))
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
    }   
}

HRESULT CBody_MecanoBot::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    _bool bNormal{};
    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

        if (i == 0)
        {
            bNormal = true;

            if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_NORMALS, 0,
                                                                 "g_NormalTexture")))
                return E_FAIL;
        }
        else
            bNormal = false;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_bNomal", &bNormal, sizeof(_bool))))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
    return S_OK;
}

HRESULT CBody_MecanoBot::Render_Shadow()
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

void CBody_MecanoBot::ChangeState(_int nextState)
{
    if (m_pStateMachine[*m_pParentState])
        m_pStateMachine[*m_pParentState]->Reset_StateMachine(&m_RimDesc);

    *m_pParentState = nextState;

    if (m_pStateMachine[*m_pParentState])
        m_pStateMachine[*m_pParentState]->StateMachine_Playing(0.f, &m_RimDesc);
}

HRESULT CBody_MecanoBot::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_ComPonent_MecanoBot"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (FAILED(m_pModelCom->InsertAiTexture(aiTextureType::aiTextureType_NORMALS, 0,TEXT("../Bin/Resources/Models/Nomal/T_Tire_N.dds"))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"), TEXT("Com_Texture_Mask"),
                                      reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    return S_OK;
}

HRESULT CBody_MecanoBot::Bind_ShaderResources()
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

HRESULT CBody_MecanoBot::Set_StateMachine()
{
    m_pStateMachine.resize(CMecanoBot::ST_END);

#pragma region IDLE
    CMecanoBot_Idle::STATEMACHINE_DESC pMachineDesc{};
    pMachineDesc.pParentModel = m_pModelCom;
    pMachineDesc.iNextMachineIdx = CMecanoBot::ST_IDLE;
    m_pStateMachine[CMecanoBot::ST_IDLE] = CMecanoBot_Idle::Create(&pMachineDesc);
#pragma endregion

#pragma region Attack
   CMecanoBot_Attack::ATTACK_DESC pAttackDesc{};
   pAttackDesc.pParentModel = m_pModelCom;
   pAttackDesc.pParentPartObject = this;
   pAttackDesc.pParentObject = m_pParentObj;
   pAttackDesc.pParentBoneMat = m_pSocketMatrix;
   pAttackDesc.pParentWorldMat = &m_WorldMatrix;
   pAttackDesc.iNextMachineIdx = CMecanoBot::ST_MOVE;
   pAttackDesc.fLength = static_cast<CMecanoBot*>(m_pParentObj)->Get_fAttackLength();
   m_pStateMachine[CMecanoBot::ST_SHOOT] = CMecanoBot_Attack::Create(&pAttackDesc);
#pragma endregion

#pragma region Move
   CMecanoBot_Move::MOVE_DESC pMoveDesc{};
   pMoveDesc.pParentModel = m_pModelCom;
   pMoveDesc.pParentObject = m_pParentObj;
   pMoveDesc.iNextMachineIdx = CMecanoBot::ST_SHOOT;
   pMoveDesc.fLength = static_cast<CMecanoBot*>(m_pParentObj)->Get_fAttackLength();
   m_pStateMachine[CMecanoBot::ST_MOVE] = CMecanoBot_Move::Create(&pMoveDesc);
#pragma endregion

#pragma region Dead
   CMecanoBot_Dead::DEAD_DESC pDeadDesc{};
   pDeadDesc.pParentModel = m_pModelCom;
   pDeadDesc.pParentObject = m_pParentObj;
   pDeadDesc.pParentPartObject = this;
   m_pStateMachine[CMecanoBot::ST_DEAD] = CMecanoBot_Dead::Create(&pDeadDesc);
#pragma endregion

#pragma region Hit
   CMecanoBot_Hit::HIT_DESC pHitDesc{};
   pHitDesc.pParentModel = m_pModelCom;
   pHitDesc.iNextMachineIdx = CMecanoBot::ST_MOVE;
   pHitDesc.HitType = static_cast<CMecanoBot*>(m_pParentObj)->Get_AttackAngle();
   m_pStateMachine[CMecanoBot::ST_HIT] = CMecanoBot_Hit::Create(&pHitDesc);
#pragma endregion

 return S_OK;
}

CBody_MecanoBot* CBody_MecanoBot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_MecanoBot* pInstance = new CBody_MecanoBot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_MecanoBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_MecanoBot::Clone(void* pArg)
{
    CBody_MecanoBot* pInstance = new CBody_MecanoBot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBody_MecanoBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_MecanoBot::Free()
{
    __super::Free();

    for (auto& Machine : m_pStateMachine) Safe_Release(Machine);
    m_pStateMachine.clear();
    m_pStateMachine.shrink_to_fit();
}
