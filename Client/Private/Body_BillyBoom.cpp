#include "stdafx.h"
#include "BillyBoom.h"
#include "GameInstance.h"
#include "Body_BillyBoom.h"
#include "StateMachine.h"
#include "BillyBoom_Idle.h"
#include "BillyBoom_Hit.h"
#include "BillyBoom_Dead.h"
#include "BillyBoom_Barre.h"
#include "BillyBoom_Bash.h"
#include "BillyBoom_ShockWave.h"
#include "BillyBoom_Laser.h"
#include "BillyBoom_Intro.h"
#include "BillyBoom_Move.h"
CBody_BillyBoom::CBody_BillyBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{pDevice, pContext}
{
}

CBody_BillyBoom::CBody_BillyBoom(const CBody_BillyBoom& Prototype) : CPartObject{Prototype}
{
}

HRESULT CBody_BillyBoom::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_BillyBoom::Initialize(void* pArg)
{
    CBody_BillyBoom_Desc* pDesc = static_cast<CBody_BillyBoom_Desc*>(pArg);

    m_pParentObj = pDesc->pParentObj;
    m_pParentState = pDesc->pParentState;
    m_RimDesc.eState = pDesc->pRimState;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pFindAttBonMatrix[BM_LEFT_TOP] = Get_SocketMatrix("L_TopArm_04"); // ¿ÞÂÊ À§ ÆÈ
    m_pFindAttBonMatrix[BM_RIGHT_TOP] = Get_SocketMatrix("R_TopArm_04"); // ¿À¸¥ ÂÊ À§ ÆÈ
    m_pFindAttBonMatrix[BM_HAND] = Get_SocketMatrix("R_Arm_04");     // ÅÂ¾ç³¯¸®±â

    if (FAILED(Set_StateMachine()))
       return E_FAIL;
 
    return S_OK;
}

void CBody_BillyBoom::Priority_Update(_float fTimeDelta)
{
 
}

void CBody_BillyBoom::Update(_float fTimeDelta)
{

    if (CStateMachine::Result::Finished == m_pStateMachine[*m_pParentState]->StateMachine_Playing(fTimeDelta,&m_RimDesc))
    {
        if (m_pStateMachine[*m_pParentState]->Get_NextMachineIndex() != -1)
            ChangeState(m_pStateMachine[*m_pParentState]->Get_NextMachineIndex());
        else
            ChangeState(CBillyBoom::ST_IDLE);
    }
 
    m_pHeanColl->Update(XMLoadFloat4x4(&m_HeandWorldMatrix));
    __super::Update(fTimeDelta);
}

void CBody_BillyBoom::Late_Update(_float fTimeDelta)
{
  if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
      return;
  if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
      return;
  _matrix SocketMatrix = XMLoadFloat4x4(m_pFindAttBonMatrix[2]);

  for (size_t i = 0; i < 3; i++)
      SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]); 

  XMStoreFloat4x4(&m_HeandWorldMatrix,
      SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

  if (FAILED(m_pGameInstance->Add_Collider(m_pHeanColl, 4)))
      return;

  m_pGameInstance->Add_DebugComponents(m_pHeanColl);

  __super::Late_Update(fTimeDelta);
}

void CBody_BillyBoom::ChangeState(_int nextState)
{
    if (m_pStateMachine[*m_pParentState])
        m_pStateMachine[*m_pParentState]->Reset_StateMachine(&m_RimDesc);

    *m_pParentState = nextState;

    if (m_pStateMachine[*m_pParentState])
        m_pStateMachine[*m_pParentState]->StateMachine_Playing(0.f, &m_RimDesc);
}

HRESULT CBody_BillyBoom::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    _bool bEmissive{false};

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

        if (i == m_iEmissiveMashNum || -1 == m_iEmissiveMashNum)
        {
            bEmissive = true;
        }
        else
            bEmissive = false;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_bEmissive", &bEmissive, sizeof(_bool))))
            return E_FAIL;
           
        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(2)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody_BillyBoom::Render_Shadow()
{
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
                                             m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
                                             m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

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

HRESULT CBody_BillyBoom::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_ComPonent_BillyBoom"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

     _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++) {

        if (FAILED(m_pModelCom->InsertAiTexture(aiTextureType::aiTextureType_NORMALS, i,TEXT("../Bin/Resources/Models/Nomal/T_BillyBoom_02_N.dds"))))
        return E_FAIL;
    }

     CBounding_OBB::BOUND_OBB_DESC OBBDesc{};

     OBBDesc.vExtents  = {1.f, 0.5f, 1.f};
     OBBDesc.vCenter   = {0.f, 0.f, 0.f};
     OBBDesc.vRotation = { 0.f, 0.f, 0.f };

     if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider_OBB"),
                                       reinterpret_cast<CComponent**>(&m_pHeanColl), &OBBDesc)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CBody_BillyBoom::Bind_ShaderResources()
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_EmissivePower", &m_fEmissivePower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_EmissiveColor", &m_fEmissiveColor, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, 1, aiTextureType_EMISSIVE, 0,"g_EmissiveTexture")))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_BillyBoom::Set_StateMachine()
{
    m_pStateMachine.resize(CBillyBoom::ST_END);

#pragma region IDLE
    CBillyBoom_Idle::STATEMACHINE_DESC pMachineDesc{};
    pMachineDesc.pParentModel = m_pModelCom;
    pMachineDesc.pParentObject = m_pParentObj;
    pMachineDesc.iNextMachineIdx = CBillyBoom::ST_IDLE;

    m_pStateMachine[CBillyBoom::ST_IDLE] = CBillyBoom_Idle::Create(&pMachineDesc);
#pragma endregion

#pragma region Barre
    CBillyBoom_Barre::ATTACK_DESC pBarreDesc{};
    pBarreDesc.pParentModel = m_pModelCom;
    pBarreDesc.pPerantPartBonMatrix = m_pFindAttBonMatrix[BM_HAND];
    pBarreDesc.pPerantWorldMat = &m_WorldMatrix;
    pBarreDesc.pParentPartObject = this;
    pBarreDesc.pParentObject = m_pParentObj;
    pBarreDesc.iNextMachineIdx = CBillyBoom::ST_IDLE;

    m_pStateMachine[CBillyBoom::ST_BARRE] = CBillyBoom_Barre::Create(&pBarreDesc);
#pragma endregion

#pragma region ShockWave
    CBillyBoom_ShockWave::ATTACK_DESC pShockWaveDesc{};
    pShockWaveDesc.pParentModel = m_pModelCom;
    pShockWaveDesc.pPerantPartBonMatrix = m_pFindAttBonMatrix[BM_RIGHT_TOP];
    pShockWaveDesc.pPerantWorldMat = &m_WorldMatrix;
    pShockWaveDesc.pParentPartObject = this;
    pShockWaveDesc.iNextMachineIdx = CBillyBoom::ST_IDLE;

    m_pStateMachine[CBillyBoom::ST_SHOCKWAVE] = CBillyBoom_ShockWave::Create(&pShockWaveDesc);
#pragma endregion

#pragma region LASER
    CBillyBoom_Laser::ATTACK_DESC pLaserDesc{};
    pLaserDesc.pParentModel = m_pModelCom;
    pLaserDesc.pPerantPartBonMatrix[CBillyBoom_Laser::LEFT_BONE] = m_pFindAttBonMatrix[BONE_MAT::BM_LEFT_TOP];
    pLaserDesc.pPerantPartBonMatrix[CBillyBoom_Laser::RIGHT_BONE] = m_pFindAttBonMatrix[BONE_MAT::BM_RIGHT_TOP];
    pLaserDesc.pPerantWorldMat = &m_WorldMatrix;
    pLaserDesc.pParentPartObject = this;
    pLaserDesc.iNextMachineIdx = CBillyBoom::ST_IDLE;

    m_pStateMachine[CBillyBoom::ST_LASER] = CBillyBoom_Laser::Create(&pLaserDesc);
#pragma endregion

#pragma region Bash
    CBillyBoom_Bash::ATTACK_DESC pBashDesc{};
    pBashDesc.pParentModel = m_pModelCom;
    pBashDesc.iNextMachineIdx = CBillyBoom::ST_IDLE;

    m_pStateMachine[CBillyBoom::ST_BASH] = CBillyBoom_Bash::Create(&pBashDesc);
#pragma endregion

#pragma region Intro
    CBillyBoom_Intro::ATTACK_DESC pIntroDesc{};
    pIntroDesc.pParentModel = m_pModelCom;
    pIntroDesc.pParentPartObject = this;
    pIntroDesc.pParentObject = m_pParentObj;
    pIntroDesc.fEmissiveColor = &m_fEmissiveColor;
    pIntroDesc.fEmissivePower = &m_fEmissivePower;
    pIntroDesc.iEmissiveMashNum = &m_iEmissiveMashNum;
    pIntroDesc.iNextMachineIdx = CBillyBoom::ST_IDLE;

    m_pStateMachine[CBillyBoom::ST_INTRO] = CBillyBoom_Intro::Create(&pIntroDesc);
#pragma endregion

#pragma region Move
    CBillyBoom_Move::MOVE_DESC pMoveDesc{};
    pMoveDesc.pParentModel = m_pModelCom;
    pMoveDesc.pParentObject = m_pParentObj;
    pMoveDesc.pParentPartObject = this;
    pMoveDesc.iNextMachineIdx = CBillyBoom::ST_IDLE;
    pMoveDesc.fLength = static_cast<CBillyBoom*>(m_pParentObj)->Get_fAttackLength();

    m_pStateMachine[CBillyBoom::ST_MOVE] = CBillyBoom_Move::Create(&pMoveDesc);
#pragma endregion

#pragma region Dead
    CBillyBoom_Dead::DEAD_DESC pDeadDesc{};
    pDeadDesc.pParentModel = m_pModelCom;
    pDeadDesc.pParentObject = m_pParentObj;
    pDeadDesc.fEmissiveColor = &m_fEmissiveColor;
    pDeadDesc.fEmissivePower = &m_fEmissivePower;
    pDeadDesc.iEmissiveMashNum = &m_iEmissiveMashNum;
    pDeadDesc.pParentPartObject = this;
    pDeadDesc.iNextMachineIdx = CBillyBoom::ST_DEAD;

    m_pStateMachine[CBillyBoom::ST_DEAD] = CBillyBoom_Dead::Create(&pDeadDesc);
#pragma endregion

#pragma region Hit
    CBillyBoom_Hit::HIT_DESC pHitDesc{};
    pHitDesc.pParentModel = m_pModelCom;
    pHitDesc.iNextMachineIdx = CBillyBoom::ST_MOVE;
    pHitDesc.HitType = static_cast<CBillyBoom*>(m_pParentObj)->Get_AttackAngle();
    m_pStateMachine[CBillyBoom::ST_HIT] = CBillyBoom_Hit::Create(&pHitDesc);
#pragma endregion

    return S_OK;
}

CBody_BillyBoom* CBody_BillyBoom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_BillyBoom* pInstance = new CBody_BillyBoom(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_BillyBoom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_BillyBoom::Clone(void* pArg)
{
    CBody_BillyBoom* pInstance = new CBody_BillyBoom(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBody_BillyBoom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_BillyBoom::Free()
{
    __super::Free();
    Safe_Release(m_pHeanColl);

    for (auto& Machine : m_pStateMachine) Safe_Release(Machine);
    m_pStateMachine.clear();
    m_pStateMachine.shrink_to_fit();
}
