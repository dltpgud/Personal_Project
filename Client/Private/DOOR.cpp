#include "stdafx.h"
#include "DOOR.h"
#include "InteractiveUI.h"
#include "GameInstance.h"
#include "Player.h"
#include "Level_Loading.h"
#include "Pade.h"
CDOOR::CDOOR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CDOOR::CDOOR(const CDOOR& Prototype) : CGameObject{Prototype}
{
}

HRESULT CDOOR::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDOOR::Initialize(void* pArg)
{
    GAMEOBJ_DESC* pDesc = static_cast<GAMEOBJ_DESC*>(pArg);
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Set_Model(pDesc->ProtoName, pDesc->CuriLevelIndex);
    m_ChangeLevelDoor = pDesc->Object_Type;

    m_InteractiveUI = static_cast<CInteractiveUI*>(m_pGameInstance->Find_Clone_UIObj(L"Interactive"));


    m_bInterect = false;
    Add_StageDoorLight();
    Add_BossDoorLight();
    return S_OK;
}

void CDOOR::Priority_Update(_float fTimeDelta)
{
}

void CDOOR::Update(_float fTimeDelta)
{
    if (true == m_pColliderCom->IsColl() && false == m_bInterect && false == m_bOpen)
    {
        m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_InteractiveUI, CUI::UIID_PlayerWeaPon_Aim);
        m_InteractiveUI->Set_Text(L"¹® ¿­±â");
        m_bInterect = true;
    }
    else if (false == m_pColliderCom->IsColl() && true == m_bInterect)
    {
        m_bInterect = false;
       m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_PlayerWeaPon_Aim, CUI::UIID_InteractiveUI);
    }
    else if (false == m_pColliderCom->IsColl() && true == m_bOpen)
    {
        m_bOpen = false;
        m_iDoorType == DoorType::ITEM ? m_iState = State2::ClOSE2 : m_iState = State::ClOSE;
        m_pModelCom->Set_Animation(m_iState, false);
        m_pGameInstance->changeCellType(1);
        m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_PlayerWeaPon_Aim, CUI::UIID_InteractiveUI);
    }

    if (true == m_InteractiveUI->Get_Interactive() && true == m_bInterect)
    {
        switch (m_iDoorType)
        {
        case Client::CDOOR::STAGE: 
            m_pGameInstance->Play_Sound(L"ST_Door_Act1_Open.ogg", CSound::SOUND_BGM, 1.f); 
             m_iState = State::OPEN;
            break;
        case Client::CDOOR::ITEM:
            m_pGameInstance->Play_Sound(L"ST_Door_Special_Airlock_Open.ogg", CSound::SOUND_BGM, 1.f);
            m_iState = State2::OPEN2;
            break;
        case Client::CDOOR::BOSS:
            m_pGameInstance->Play_Sound(L"ST_Door_Boss_Act1_Open.ogg", CSound::SOUND_BGM, 1.f);
            m_iState = State::OPEN;
            break;
        }
        m_iState == State::OPEN ? m_OpenTime = 0.3f : m_OpenTime = 0.6f;
        m_pModelCom->Set_Animation(m_iState, false);
        m_bOpen = true;
        m_bInterect = false;
        m_bSoud = false;
        m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_PlayerWeaPon_Aim, CUI::UIID_InteractiveUI);

        m_pGameInstance->changeCellType(0);

        m_InteractiveUI->Set_Interactive(false);
    }
   
    if (m_iDoorType == DoorType::STAGE || m_iDoorType == DoorType::BOSS)
    {
        m_pModelCom->Callback(State::ClOSE, 6,
                              [this]()
                              {
                                  if (false == m_bSoud)
                                  {
                                      m_pGameInstance->changeCellType(1);
                                      m_pGameInstance->Play_Sound(L"ST_Door_Act1_Close.ogg", CSound::SOUND_BGM, 0.5f);
                                      m_bSoud = true;
                                  }
                              });
    }
    
    if (true == m_pModelCom->Play_Animation(fTimeDelta * m_OpenTime)) 
    {
        if (m_ChangeLevelDoor == 2 && m_iState == State::OPEN)
        {
            static_cast<CPade*>(m_pGameInstance->Find_Clone_UIObj(L"Fade"))->Set_Pade(true);
            m_pGameInstance->Set_Open_Bool(true);
            
        }
    }
    __super::Update(fTimeDelta);
}

void CDOOR::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;

    if (FAILED(m_pGameInstance->Add_Interctive(this)))
        return;
    __super::Late_Update(fTimeDelta);
}

HRESULT CDOOR::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (i != 0 && m_iDoorType!=DoorType::ITEM)
        {
            if (FAILED(m_pShaderCom->Bind_RawValue("g_DoorBool", &m_bInterect , sizeof(_bool))))
                return E_FAIL;
        }

        if (m_iDoorType == DoorType::ITEM)
        {
            if (i == 1) {
                m_bEmissive = true;
                if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                    "g_EmissiveTexture")))
                    return E_FAIL;
            }
            else
                m_bEmissive = false;   
        }
        else
            m_bEmissive = false;

        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_bDoorEmissive", &m_bEmissive, sizeof(_bool))))
                return E_FAIL;
 
        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(4)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CDOOR::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::TRANSFORM_STATE::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::TRANSFORM_STATE::D3DTS_PROJ))))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pShaderCom->Begin(5)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

   return S_OK;
}

void CDOOR::Set_Model(const _wstring& protoModel, _uint ILevel)
{
    if (FAILED(__super::Add_Component(ILevel, protoModel, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        MSG_BOX("Set_Model failed");
        return;
    }

    if (protoModel == L"Proto Component ItemDoor Model_aniObj")
        m_iDoorType = DoorType::ITEM;
    if (protoModel == L"Proto Component StageDoor Model_aniObj")
        m_iDoorType = DoorType::STAGE;
    if (protoModel == L"Proto Component BossDoor Model_aniObj")
        m_iDoorType = DoorType::BOSS;

    m_iDoorType == DoorType::ITEM ? m_iState = State2::ClOSE2 : m_iState = State::ClOSE;
    m_RimColor = {0.f, 1.f, 0.f, 1.f};
 
    m_pModelCom->Set_Animation(m_iState, false);
    

    if (ILevel == LEVEL_STAGE1)
        m_fDoorEmissiveColor = { 1.f,0.749f,0.2156f, 1.f };
    else if(ILevel == LEVEL_STAGE2)
        m_fDoorEmissiveColor = { 1.f,0.f,0.f, 1.f };
}

HRESULT CDOOR::Add_StageDoorLight()
{
    if (m_iDoorType != DoorType::STAGE)
        return S_OK;

     const _float4x4* LightPos = m_pModelCom->Get_BoneMatrix("Door_Down");
    
     _vector vLocalPos{};
     _vector vWoldPos{};
     _float4 fWoldPos{};
     LIGHT_DESC	LightDesc{};

     vLocalPos = { LightPos->_41 - 4.5f, LightPos->_42 + 7.f, LightPos->_43 + 3.f, LightPos->_44 };
     vWoldPos =  XMVector3TransformCoord(vLocalPos, m_pTransformCom->Get_WorldMatrix());
     XMStoreFloat4(&fWoldPos, vWoldPos);

    ZeroMemory(&LightDesc, sizeof LightDesc);
    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(fWoldPos.x, fWoldPos.y, fWoldPos.z, 1.f);
    LightDesc.fRange = 3.f;
    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;


    vLocalPos = { LightPos->_41 + 3.5f, LightPos->_42 + 7.f, LightPos->_43 + 3.f, LightPos->_44 };
    vWoldPos = XMVector3TransformCoord(vLocalPos, m_pTransformCom->Get_WorldMatrix());
    XMStoreFloat4(&fWoldPos, vWoldPos);

    ZeroMemory(&LightDesc, sizeof LightDesc);
    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(fWoldPos.x, fWoldPos.y, fWoldPos.z, 1.f);
    LightDesc.fRange = 3.f;
    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    vLocalPos = { LightPos->_41 + 4.f, LightPos->_42 + 7.f, LightPos->_43 - 3.f, LightPos->_44 };
    vWoldPos = XMVector3TransformCoord(vLocalPos, m_pTransformCom->Get_WorldMatrix());
    XMStoreFloat4(&fWoldPos, vWoldPos);

    ZeroMemory(&LightDesc, sizeof LightDesc);
    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(fWoldPos.x, fWoldPos.y, fWoldPos.z, 1.f);
    LightDesc.fRange = 3.f;
    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    vLocalPos = { LightPos->_41 - 4.f, LightPos->_42 + 7.f, LightPos->_43 - 3.f, LightPos->_44 };
    vWoldPos = XMVector3TransformCoord(vLocalPos, m_pTransformCom->Get_WorldMatrix());
    XMStoreFloat4(&fWoldPos, vWoldPos);

    ZeroMemory(&LightDesc, sizeof LightDesc);
    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(fWoldPos.x, fWoldPos.y, fWoldPos.z, 1.f);
    LightDesc.fRange = 3.f;
    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
    
    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDOOR::Add_BossDoorLight()
{
    if (m_iDoorType != DoorType::BOSS)
        return S_OK;

    const _float4x4* LightPos = m_pModelCom->Get_BoneMatrix("Clap_L");

    _vector vLocalPos{};
    _vector vWoldPos{};
    _float4 fWoldPos{};
    LIGHT_DESC	LightDesc{};

    vLocalPos = { LightPos->_41-2.f, LightPos->_42 +7.f, LightPos->_43-4.f, LightPos->_44 };
    vWoldPos = XMVector3TransformCoord(vLocalPos, m_pTransformCom->Get_WorldMatrix());
    XMStoreFloat4(&fWoldPos, vWoldPos);

    ZeroMemory(&LightDesc, sizeof LightDesc);
    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(fWoldPos.x, fWoldPos.y, fWoldPos.z, 1.f);
    LightDesc.fRange = 3.f;
    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    vLocalPos = { LightPos->_41 + 2.f, LightPos->_42 + 7.f, LightPos->_43-4.f, LightPos->_44 };
    vWoldPos = XMVector3TransformCoord(vLocalPos, m_pTransformCom->Get_WorldMatrix());
    XMStoreFloat4(&fWoldPos, vWoldPos);

    ZeroMemory(&LightDesc, sizeof LightDesc);
    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(fWoldPos.x, fWoldPos.y, fWoldPos.z, 1.f);
    LightDesc.fRange = 3.f;
    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDOOR::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};
    OBBDesc.vExtents = _float3(3.f, 1.f, 3.f);
    OBBDesc.vCenter = _float3(0.f, 1.f, 0.f);
    OBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
                                      TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom),
                                      &OBBDesc)))
        return E_FAIL;
    return S_OK;
}

HRESULT CDOOR::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_RimColor, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_DoorEmissiveColor", &m_fDoorEmissiveColor, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CDOOR* CDOOR::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDOOR* pInstance = new CDOOR(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDOOR::Clone(void* pArg)
{
    CDOOR* pInstance = new CDOOR(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CAniObj");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDOOR::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
