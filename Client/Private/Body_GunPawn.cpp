#include "stdafx.h"
#include "Body_GunPawn.h"
#include "GameInstance.h"
#include "GunPawn.h"
#include "Bullet.h"
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

    m_RimDesc.eState  = pDesc->pRimState;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
    m_DyTime = 2.f;
    m_pFindBonMatrix = Get_SocketMatrix("R_Canon_Scale_02");

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

    if (*m_pParentState == CGunPawn::ST_RUN_BACK && m_iCurMotion != CGunPawn::ST_RUN_BACK)
    {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_SETB);
        m_pGameInstance->PlayBGM(CSound::SOUND_MONSTER_SETB, L"ST_Enemy_Step_Medium.ogg", 0.2f);
        m_iCurMotion = CGunPawn::ST_RUN_BACK;
        bMotionChange = true;
        bLoop = true;
    }

    if (*m_pParentState == CGunPawn::ST_RUN_LEFT && m_iCurMotion != CGunPawn::ST_RUN_LEFT)
    {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_SETB);
        m_pGameInstance->PlayBGM(CSound::SOUND_MONSTER_SETB, L"ST_Enemy_Step_Medium.ogg", 0.2f);
        m_iCurMotion = CGunPawn::ST_RUN_LEFT;
        bMotionChange = true;
        bLoop = true;
    }
    if (*m_pParentState == CGunPawn::ST_RUN_RIGHT && m_iCurMotion != CGunPawn::ST_RUN_RIGHT)
    {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_SETB);
        m_pGameInstance->PlayBGM(CSound::SOUND_MONSTER_SETB, L"ST_Enemy_Step_Medium.ogg", 0.2f);
        m_iCurMotion = CGunPawn::ST_RUN_RIGHT;
        bMotionChange = true;
        bLoop = true;
    }

    if (*m_pParentState == CGunPawn::ST_STUN_START && m_iCurMotion != CGunPawn::ST_STUN_START)
    {
        m_iCurMotion = CGunPawn::ST_STUN_START;
        m_fPlayAniTime = 0.5f;
        bMotionChange = true;
        bLoop = false;
    }
    else
        m_fPlayAniTime = 0.6f;

    if (*m_pParentState == CGunPawn::ST_HIT_FRONT && m_iCurMotion != CGunPawn::ST_HIT_FRONT)
    {
        m_iCurMotion = CGunPawn::ST_HIT_FRONT;
        bMotionChange = true;
        bLoop = true;
    }

    if (*m_pParentState == CGunPawn::ST_IDLE && m_iCurMotion != CGunPawn::ST_IDLE)
    {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_SETB);
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

    if (*m_pParentState == CGunPawn::ST_PRESHOOT && m_iCurMotion != CGunPawn::ST_PRESHOOT)
    {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_SETB);
        m_DyingTime = true;
        m_iCurMotion = CGunPawn::ST_PRESHOOT;
        m_fPlayAniTime = 0.5;
        bMotionChange = true;
        bLoop = false;
    }
    else
        m_fPlayAniTime = 1.f;


    if (*m_RimDesc.eState == RIM_LIGHT_DESC::STATE_RIM)
    {
        m_RimDesc.fcolor = { 1.f,1.f,1.f,1.f };
        m_RimDesc.iPower = 1;
    }


    if (*m_RimDesc.eState == RIM_LIGHT_DESC::STATE_NORIM) {
        m_RimDesc.fcolor = { 0.f,0.f,0.f,0.f };
        m_RimDesc.iPower = 1;
    }

    if (m_iCurMotion == CGunPawn::ST_RUN_LEFT || m_iCurMotion == CGunPawn::ST_RUN_RIGHT)
    {
        m_FireTime += fTimeDelta;

        if (m_FireTime > 0.5f) {
            Make_Bullet();
            m_FireTime = 0.f;
        }
    }

    if (bMotionChange)
        m_pModelCom->Set_Animation(m_iCurMotion, bLoop);

    if (true == m_pModelCom->Play_Animation(fTimeDelta * m_fPlayAniTime))
    {
       
        m_bFinishAni = true;
        m_iCurMotion = CGunPawn::ST_IDLE;
        m_pModelCom->Set_Animation(m_iCurMotion, true);
    }
    else
    {
        m_bFinishAni = false;

        if (m_iCurMotion == CGunPawn::ST_GRENADE_PRESHOOT)
            m_bRUN = true;
        else
            m_bRUN = false;
        if (m_iCurMotion == CGunPawn::ST_PRESHOOT)
            m_pModelCom->Set_Animation(m_iCurMotion, false);
    }

    __super::Update(fTimeDelta);
}

void CBody_GunPawn::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);

    if (true == m_pGameInstance->isIn_Frustum_WorldSpace(XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, m_WorldMatrix._44), 1.5f))
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;
      
    } // if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
       //     return;
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

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody_GunPawn::Render_Shadow()
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

HRESULT CBody_GunPawn::Make_Bullet()
{
    m_pGameInstance->Play_Sound(L"ST_Enemy_Rocket_Shoot.ogg", CSound::SOUND_EFFECT, 0.3f);
    _vector Hend_Local_Pos = { m_pFindBonMatrix->_41, m_pFindBonMatrix->_42,  m_pFindBonMatrix->_43,  m_pFindBonMatrix->_44 };

    _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(&m_WorldMatrix));
    
     _vector Dir = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION) 
                      - m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = Dir;
    Desc.vPos = vHPos;
    Desc.fDamage = &m_pDamage;
    Desc.iActorType = CSkill::MONSTER::TYPE_GUNPAWN;
    CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(L"Prototype GameObject_Bullet", &Desc);
    m_pGameInstance->Add_Clon_to_Layers(m_pGameInstance->Get_iCurrentLevel(), CGameObject::SKILL, pGameObject);

    return S_OK;
}

HRESULT CBody_GunPawn::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Proto_Component_GunPawn_Model"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;


    /* For.Com_Tex */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
        TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
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

    if (FAILED(m_pShaderCom->Bind_Bool("g_TagetBool", *m_RimDesc.eState)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Int("g_RimPow", m_RimDesc.iPower)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_RimDesc.fcolor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Bool("g_TagetDeadBool", m_iCurMotion == CGunPawn::ST_PRESHOOT)))
        return E_FAIL;


    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_maskTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Float("g_threshold", m_interver)))
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
}
