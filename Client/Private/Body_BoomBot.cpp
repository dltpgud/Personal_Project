#include "stdafx.h"
#include "BoomBot.h"
#include "GameInstance.h"
#include "Body_BoomBot.h"
#include "Bullet.h"
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
    m_RimDesc.eState = pDesc->pRimState;

    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
   m_fPlayAniTime = 0.5f;
   m_DyTime = 2.f;
   m_pFindBonMatrix = Get_SocketMatrix("Canon_Scale");
    return S_OK;
}

void CBody_BoomBot::Priority_Update(_float fTimeDelta)
{

}

void CBody_BoomBot::Update(_float fTimeDelta)
{
    _bool bMotionChange = {false}, bLoop = {false};
    if (*m_pParentState == CBoomBot::ST_Aim_Down && m_iCurMotion != CBoomBot::ST_Aim_Down)
    {
        m_DyingTime = true;
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_ROLL2);
        m_iCurMotion = CBoomBot::ST_Aim_Down;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = false;
    }

    if (*m_pParentState == CBoomBot::ST_Hit_Front && m_iCurMotion != CBoomBot::ST_Hit_Front)
    {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_ROLL2);
        m_iCurMotion = CBoomBot::ST_Hit_Front;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }

    if (*m_pParentState == CBoomBot::ST_Idle && m_iCurMotion != CBoomBot::ST_Idle)
    {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_ROLL2);
        m_iCurMotion = CBoomBot::ST_Idle;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }


    if (*m_pParentState == CBoomBot::ST_Run_Front && m_iCurMotion != CBoomBot::ST_Run_Front)
    {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_ROLL2);
        m_pGameInstance->Play_Sound(L"ST_Enemy_Move_Roll2.ogg", CSound::SOUND_MONSTER_ROLL2, 0.2f);
    
        m_iCurMotion = CBoomBot::ST_Run_Front;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = true;
    }


    if (*m_pParentState == CBoomBot::ST_Run_Back && m_iCurMotion != CBoomBot::ST_Run_Back)
    {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_ROLL2);
        m_pGameInstance->Play_Sound(L"ST_Enemy_Move_Roll.ogg", CSound::SOUND_MONSTER_ROLL2, 0.2f);
        m_iCurMotion = CBoomBot::ST_Run_Back;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = true;
    }

    if (*m_pParentState == CBoomBot::ST_Shoot && m_iCurMotion != CBoomBot::ST_Shoot)
    {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_ROLL2);
        Make_Bullet();
        m_iCurMotion = CBoomBot::ST_Shoot;
        m_fPlayAniTime = 0.5f;
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
      
        m_bFinishAni = true;
        m_pModelCom->Set_Animation(m_iCurMotion, false);
    }
    else
    { 
        m_bFinishAni = false;
     
        if (m_iCurMotion == CBoomBot::ST_Aim_Down)
        m_pModelCom->Set_Animation(m_iCurMotion, false);
    }

    __super::Update(fTimeDelta);
}

void CBody_BoomBot::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);

    if (true == m_pGameInstance->isIn_Frustum_WorldSpace(XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, m_WorldMatrix._44), 1.5f))
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;


    }  //      if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
        //    return;
}

HRESULT CBody_BoomBot::Render()
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

        if (i == 2)
        {
            bNormal = true;

            if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_NORMALS, 0,
                                                                 "g_NormalTexture")))
                return E_FAIL;
        }
        else
            bNormal = false;

        
        if (FAILED(m_pShaderCom->Bind_RawValue("g_bNomal",&bNormal, sizeof(_bool))))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody_BoomBot::Render_Shadow()
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

void CBody_BoomBot::Make_Bullet()
{
    m_pGameInstance->Play_Sound(L"ST_MortarPod_Shoot.ogg", CSound::SOUND_EFFECT, 0.3f);
    _vector Hend_Local_Pos = { m_pFindBonMatrix->_41, m_pFindBonMatrix->_42,  m_pFindBonMatrix->_43,  m_pFindBonMatrix->_44 };

    _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(&m_WorldMatrix));

    _vector Dir = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION)
        - m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION);

    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = Dir;
    Desc.vPos = vHPos;
    Desc.fDamage = &m_pDamage;

    Desc.iActorType = CSkill::MONSTER::TYPE_BOOMBOT;
   
    Desc.iSkillType = CSkill::STYPE_STURN;
    CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(L"Prototype GameObject_Bullet", &Desc);
    m_pGameInstance->Add_Clon_to_Layers(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"), pGameObject);
}

HRESULT CBody_BoomBot::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Proto_Component_BoomBot_Model"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

 if (FAILED(m_pModelCom->InsertAiTexture(aiTextureType::aiTextureType_NORMALS, 2,TEXT("../Bin/Resources/Models/Nomal/T_Tire_N.dds"))))
       return E_FAIL;

    /* For.Com_Tex */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
        TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
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
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_RawValue("g_TagetBool", &m_RimDesc.eState, sizeof(_int))))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RimPow", &m_RimDesc.iPower, sizeof(_int))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_RimDesc.fcolor, sizeof(_float4))))
        return E_FAIL;
        
    _bool state{false};
    if (m_iCurMotion == CBoomBot::ST_Aim_Down)
        state = true;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_TagetDeadBool",&state, sizeof(_bool) )))
        return E_FAIL;

 
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_maskTexture", 0)))
        return E_FAIL;

     if (FAILED(m_pShaderCom->Bind_RawValue("g_threshold", &m_interver, sizeof(_float))))
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
