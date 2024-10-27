#include "stdafx.h"
#include "DOOR.h"
#include "InteractiveUI.h"
#include "GameInstance.h"
#include "Player.h"
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
    _vector vPlayerPos = m_pPlayer->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPos - vPlayerPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));

    if (fLength <= 6.f && m_bState == false ) {
     
        m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_InteractiveUI, CUI::UIID_PlayerWeaPon_Aim);
        m_InteractiveUI->Set_Text(L"문 열기",CInteractiveUI::INTERACTIVE_STATE::IS_DOOR);
        if (true == m_InteractiveUI->Get_Interactive())
        {

            if (m_pModelName != L"Proto Component ItemDoor Model_aniObj")
                m_iState = State::OPEN;
            else
                m_iState = State2::OPEN2;

            m_pModelCom->Set_Animation(m_iState, false);
            Go_Move = true;
            m_bOpen = true;
            m_InteractiveUI->Set_Interactive(false);
            m_bState = true;
        }
    }
 
    if (fLength > 6.f && m_bState == true) {

        if(m_pModelName != L"Proto Component ItemDoor Model_aniObj")
        m_iState = State::ClOSE;
        else
            m_iState = State2::ClOSE2;

        Go_Move = false;
        m_pModelCom->Set_Animation(m_iState, false);
        m_bState = false;

    }

    if (true == m_bOpen)
    {
        m_pGameInstance->Set_OpenUI(CUI::UIID_InteractiveUI, false);
        m_bOpen = false;
    }
   

    if (fLength <= 6.f && false == Go_Move)
    {
      m_pPlayer->Set_NavigationType(1);
    }
 
    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
      
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
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

        /*애니용 추가*/
        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CDOOR::Set_Model(const _wstring& protoModel)
{
    m_pModelName = protoModel;
    if (FAILED(__super::Add_Component(LEVEL_STAGE1, protoModel, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        MSG_BOX("Set_Model failed");
        return;
    }

    if(m_pModelName == L"Proto Component ItemDoor Model_aniObj")
        m_pModelCom->Set_Animation(2, false);
    else {

        m_iState = State::ClOSE;
        m_pModelCom->Set_Animation(m_iState, true);
    }
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
