#include "stdafx.h"
#include "BillyBoom.h"
#include "GameInstance.h"
#include "Body_BillyBoom.h"
#include "Bullet.h"
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

    m_pParentState = pDesc->pParentState;
    m_RimDesc.eState = pDesc->pRimState;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    m_DATA_TYPE = CGameObject::DATA_MONSTER;
    if (FAILED(Add_Components()))
        return E_FAIL;
   m_fPlayAniTime = 0.5f;

 // m_pFindBonMatrix = Get_SocketMatrix("R_Canon_02");
   m_pFindBonMatrix[0] = Get_SocketMatrix("L_TopArm_05"); // 왼쪽 위 팔
   m_pFindBonMatrix[1] = Get_SocketMatrix("R_TopArm_05"); // 오른 쪽 위 팔
   m_pFindBonMatrix[2] = Get_SocketMatrix("R_TopArm_04"); // 위팔 모아 쏴
   m_pFindBonMatrix[3] = Get_SocketMatrix("R_Arm_04"); // 위팔 모아 쏴
    return S_OK;
}

_int CBody_BillyBoom::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CBody_BillyBoom::Update(_float fTimeDelta)
{
    _bool bMotionChange = {false}, bLoop = {false};
    if (*m_pParentState == CBillyBoom::ST_Idle && m_iCurMotion != CBillyBoom::ST_Idle)
    {
        m_iCurMotion = CBillyBoom::ST_Idle;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CBillyBoom::ST_Intro && m_iCurMotion != CBillyBoom::ST_Intro)
    {
        m_iCurMotion = CBillyBoom::ST_Intro;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
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

    if (true == m_pModelCom->Play_Animation(fTimeDelta * m_fPlayAniTime))
    {
        if (m_iCurMotion != CBillyBoom::ST_Intro) {
            m_bFinishAni = true;
            m_iCurMotion = CBillyBoom::ST_Idle;
            m_pModelCom->Set_Animation(m_iCurMotion, false);
        }
    }
    else
    {  
      
        m_bFinishAni = false;
       if(m_iCurMotion == CBillyBoom::ST_Stun_Start )
        m_pModelCom->Set_Animation(m_iCurMotion, false);
    }
}

void CBody_BillyBoom::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CBody_BillyBoom::Render()
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

void CBody_BillyBoom::Make_Bullet()
{
 //  _vector Hend_Local_Pos = { m_pFindBonMatrix->_41, m_pFindBonMatrix->_42,  m_pFindBonMatrix->_43,  m_pFindBonMatrix->_44 };
 //
 //  _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(&m_WorldMatrix));
 //
 //  _vector Dir = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)
 //      - m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
 //
 //  CBullet::CBULLET_DESC Desc{};
 //  Desc.fSpeedPerSec = 20.f;
 //  Desc.pTagetPos = Dir;
 //  Desc.vPos = vHPos;
 //  Desc.Damage = &m_pDamage;
 //  Desc.iWeaponType = CBullet::MONSTER_BULLET::TYPE_MECANOBOT; 
 //  Desc.LifTime = 1.f;
 //  CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(L"Prototype GameObject_Bullet", &Desc);
 //  m_pGameInstance->Add_Clon_to_Layers(m_pGameInstance->Get_iCurrentLevel(), CGameObject::SKILL, pGameObject);
 //  m_pGameInstance->Add_MonsterBullet(m_pGameInstance->Get_iCurrentLevel(), pGameObject);
   
}

HRESULT CBody_BillyBoom::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_ComPonent_BillyBoom"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_BillyBoom::Bind_ShaderResources()
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


    if (FAILED(m_pShaderCom->Bind_Bool("g_TagetBool", m_RimDesc.eState)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Int("g_RimPow", m_RimDesc.iPower)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_RimDesc.fcolor, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Bool("g_TagetDeadBool", m_iCurMotion == CBillyBoom::ST_Stun_Start)))
        return E_FAIL;

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
}
