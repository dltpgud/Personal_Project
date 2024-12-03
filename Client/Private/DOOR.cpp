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
    /* 패킷, 파일입ㅇ출력을 통한 초기화. */

    return S_OK;
}

HRESULT CDOOR::Initialize(void* pArg)
{
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    m_DATA_TYPE = GAMEOBJ_DATA::DATA_DOOR;
    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_InteractiveUI = static_cast<CInteractiveUI*>(m_pGameInstance->Get_UI(LEVEL_STATIC, CUI::UIID_InteractiveUI));
    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Player());

    return S_OK;
}

_int CDOOR::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    if (true == Go_Move)
    {
        m_pPlayer->Set_NavigationType(0);
    }
  
    return OBJ_NOEVENT;
}

void CDOOR::Update(_float fTimeDelta)
{

    if (m_bSet_Light) {
        Add_StageDoorLight();
        Add_BossDoorLight();
        m_bSet_Light = false;
    }
    _vector vPlayerPos = m_pPlayer->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPos - vPlayerPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));

    if (fLength <= 6.f && m_bState == false ) {
     
        m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_InteractiveUI, CUI::UIID_PlayerWeaPon_Aim);
        m_InteractiveUI->Set_Text(L"문 열기");

        if (true == m_InteractiveUI->Get_Interactive())
        {

            if (m_pModelName == L"Proto Component StageDoor Model_aniObj")
                m_pGameInstance->Play_Sound(L"ST_Door_Act1_Open.ogg", CSound::SOUND_BGM, 1.f);
            

            if (m_pModelName == L"Proto Component BossDoor Model_aniObj")
                m_pGameInstance->Play_Sound(L"ST_Door_Boss_Act1_Open.ogg", CSound::SOUND_BGM, 1.f);

            if (m_pModelName == L"Proto Component ItemDoor Model_aniObj")
                m_pGameInstance->Play_Sound(L"ST_Door_Special_Airlock_Open.ogg", CSound::SOUND_BGM, 1.f);


            if (m_pModelName != L"Proto Component ItemDoor Model_aniObj")
                m_iState = State::OPEN;
            else
                m_iState = State2::OPEN2;

            m_iState == State::OPEN ? m_OpenTime = 0.3f : m_OpenTime = 0.6f;
            m_pModelCom->Set_Animation(m_iState, false);
            Go_Move = true;
            m_bOpen = true;
            m_InteractiveUI->Set_Interactive(false);
            m_bState = true;

        }   
        m_bInterect = true;
    } 
 
    if (fLength > 6.f && m_bInterect == true) {
        m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_PlayerWeaPon_Aim, CUI::UIID_InteractiveUI);
        m_bInterect = false;
    }
    if (fLength > 6.f && m_bState == true) {
   
        if (m_pModelName == L"Proto Component ItemDoor Model_aniObj")
            m_pGameInstance->Play_Sound(L"ST_Door_Special_Open.ogg", CSound::SOUND_BGM, 1.f);

        if(m_pModelName != L"Proto Component ItemDoor Model_aniObj")
            m_iState = State::ClOSE;
        else
            m_iState = State2::ClOSE2;
        m_iState == State::ClOSE ? m_OpenTime = 0.3f : m_OpenTime = 0.6f;
        Go_Move = false;
        m_pModelCom->Set_Animation(m_iState, false);
        m_bState = false;
        m_bSoud = true;
    }

    if (true == m_bSoud)
        m_fSoundTimeSum += fTimeDelta;

    if (m_iState == State::ClOSE)
    {
        if (m_fSoundTimeSum > 1.f) {
            if (m_pModelName == L"Proto Component StageDoor Model_aniObj")
            {
                m_pGameInstance->Play_Sound(L"ST_Door_Act1_Close.ogg", CSound::SOUND_BGM, 0.5f);
                m_fSoundTimeSum = 0.f;
                m_bSoud = false;
            }
        }

    }

    if (true == m_bOpen)
    {
        m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_PlayerWeaPon_Aim, CUI::UIID_InteractiveUI);
        m_bOpen = false;
    }
   

    if (fLength <= 6.f && false == Go_Move)
    {
      m_pPlayer->Set_NavigationType(1);
    }
 
    if (true == m_pModelCom->Play_Animation(fTimeDelta * m_OpenTime)) 
    {
        if (m_DoorType == 2 && m_iState == State::OPEN)
        {
            static_cast<CPade*>(m_pGameInstance->Get_UI(LEVEL_STATIC, CUI::UIID_Pade))->Set_Pade(true);

           
                m_pGameInstance->Set_Open_Bool(true);
            
        }
    }
  
}

void CDOOR::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;


}

HRESULT CDOOR::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {

        if (i != 0 && m_pModelName != L"Proto Component ItemDoor Model_aniObj")
        {
            if (FAILED(m_pShaderCom->Bind_Bool("g_DoorBool", m_bInterect == true)))
                return E_FAIL;
        }


        if (m_pModelName == L"Proto Component ItemDoor Model_aniObj" ) {

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

        if(FAILED(m_pShaderCom->Bind_Bool("g_bDoorEmissive", m_bEmissive)))
                return E_FAIL;
        /*애니용 추가*/
        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(4)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CDOOR::Set_Model(const _wstring& protoModel, _uint ILevel)
{
    m_pModelName = protoModel;
    if (FAILED(__super::Add_Component(ILevel, protoModel, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        MSG_BOX("Set_Model failed");
        return;
    }

    if(m_pModelName == L"Proto Component ItemDoor Model_aniObj" )
        m_pModelCom->Set_Animation(2, false);
    else {
        m_RingColor = { 0.f,1.f,0.f,1.f };
        m_iState = State::ClOSE;
        m_pModelCom->Set_Animation(m_iState, false);
    }


    if (ILevel == LEVEL_STAGE1)
        m_fDoorEmissiveColor = { 1.f,0.749f,0.2156f, 1.f };
    else if(ILevel == LEVEL_STAGE2)
        m_fDoorEmissiveColor = { 1.f,0.f,0.f, 1.f };
}

void CDOOR::Set_Buffer(_uint x, _uint y)
{
    m_DoorType = y;
}

HRESULT CDOOR::Add_StageDoorLight()
{

    if (m_pModelName == L"Proto Component ItemDoor Model_aniObj")
        return S_OK;
    if (m_pModelName == L"Proto Component BossDoor Model_aniObj")
        return S_OK;

     const _float4x4* LightPos = m_pModelCom->Get_BoneMatrix("Door_Down");
    
     _vector vLocalPos{};
     _vector vWoldPos{};
     _float4 fWoldPos{};
     LIGHT_DESC	LightDesc{};

     vLocalPos  = { LightPos->_41 - 4.5f, LightPos->_42 + 7.f, LightPos->_43 + 3.f, LightPos->_44 };
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

    if (m_pModelName == L"Proto Component ItemDoor Model_aniObj")
        return S_OK;
    if (m_pModelName == L"Proto Component StageDoor Model_aniObj")
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

    return S_OK;
}

HRESULT CDOOR::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(
            m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(
            m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_RingColor, sizeof(_float4))))
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
