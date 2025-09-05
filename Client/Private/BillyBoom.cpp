#include "stdafx.h"
#include "BillyBoom.h"
#include "GameInstance.h"
#include "Body_BillyBoom.h"
#include "BossIntroBG.h"
#include "BossHP.h"
#include "Particle_Explosion.h"
#include "Fade.h"
#include "Player.h"
CBillyBoom::CBillyBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster{pDevice, pContext}
{
}

CBillyBoom::CBillyBoom(const CBillyBoom& Prototype) : CMonster{Prototype}
{
}

HRESULT CBillyBoom::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBillyBoom::Initialize(void* pArg)
{
    CActor::Actor_DESC* Desc = static_cast<CActor::Actor_DESC*>(pArg);
    Desc->fSpeedPerSec =  7.f;
    Desc->fRotationPerSec = XMConvertToRadians(120.f);
    Desc->JumpPower = 0.f;
    Desc->iHP = 1000;
    Desc->bOnCell = true;
    Desc->fFixY = 0.5f;
    Desc->iState = ST_IDLE;

    if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;
    
    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;
    m_pGameInstance->Play_Sound(L"ST_BillyBoom_Presence.ogg", nullptr, 1.f);
    return S_OK;
}

void CBillyBoom::Priority_Update(_float fTimeDelta)
{
    Compute_Length();

   if (m_bFinishIntro)
       Change_Pattern(); 

    __super::Priority_Update(fTimeDelta);
}

void CBillyBoom::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CBillyBoom::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CBillyBoom::Render()
{
    __super::Render();
    return S_OK; 
}

void CBillyBoom::HIt_Routine()
{
    m_iRim = RIM_LIGHT_DESC::STATE_RIM;

    if (m_bSkill)
        return;

    Compute_Angle();
    static_cast<CBody_BillyBoom*>(m_PartObjects[PART_BODY])->ChangeState(CBillyBoom::ST_HIT);
    m_iHP -= static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_Weapon_Info().Damage;
    m_pHP->Set_BossHP(m_iHP);
}

void CBillyBoom::Dead_Routine()
{
    m_pHP->Set_BossHP(m_iHP);
    static_cast<CBody_BillyBoom*>(m_PartObjects[PART_BODY])->ChangeState(CBillyBoom::ST_DEAD);
    m_bOnCell = false;
}

void CBillyBoom::Set_CurrentHP(_int CurrentHp)
{
    if (m_bSkill)
        return;
    m_iHP -= CurrentHp;
}

void CBillyBoom::Change_State(_int state)
{
    static_cast<CBody_BillyBoom*>(m_PartObjects[PART_BODY])->ChangeState(state);
}


void CBillyBoom::Change_Pattern()
{
    if (m_iState != ST_IDLE )
        return;

    _int RandomAttack{};
    while (true)
    {
        _bool isSame = false;
        RandomAttack = m_Dist(m_Rng);

        for (_int i = 0; i < m_iSkillTime; i++)
        {
            if (m_iPrAttack[i] == RandomAttack)
            {
                isSame = true;
                break;
            }
        }

        if (!isSame)
        {
            m_iPrAttack[m_iSkillTime] = RandomAttack;
            m_iSkillTime++;
            if (m_iSkillTime >= 4)
                m_iSkillTime = 0;
            break;
        }
    }

    m_iNextSkill = static_cast<STATE>(RandomAttack + ST_BARRE); // Move 끝나면 쓸 스킬 저장
    m_iState = ST_MOVE;
}

HRESULT CBillyBoom::Add_Components()
{    
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};

    OBBDesc.vExtents = _float3(3.f, 3.3f, 3.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);
    OBBDesc.vRotation = { 0.f, 0.f, 0.f };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"),
        reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBillyBoom::Add_PartObjects()
{
    CBody_BillyBoom::CBody_BillyBoom_Desc BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pRimState = &m_iRim;
    BodyDesc.pParentObj = this;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_BillyBoom"), PART_BODY, &BodyDesc)))
        return E_FAIL;

    CBossHPUI::CBossHPUI_DESC Desc{};
    Desc.fMaxHP = m_iMAXHP;
    Desc.fHP = m_iHP;
    Desc.Update = false;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"BossHP", L"Prototype_GameObject_Boss_HP", &Desc)))
        return E_FAIL;

    m_pHP = static_cast<CBossHPUI*>(m_pGameInstance->Find_Clone_UIObj(L"BossHP"));
    Safe_AddRef(m_pHP);
    m_pHP->Set_BossMaxHP(m_iMAXHP);

    return S_OK;
}

CBillyBoom* CBillyBoom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBillyBoom* pInstance = new CBillyBoom(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBillyBoom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBillyBoom::Clone(void* pArg)
{
    CBillyBoom* pInstance = new CBillyBoom(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBillyBoom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBillyBoom::Free()
{
    __super::Free();
    Safe_Release(m_pHP);
}
///////////////////////////////////////MODEL_ANIM_DATA//////////////////////////////////////////////////
/* enum MODEL_ANIM
{
   ST_AIM_Left,
   ST_Idle,
   ST_AIM_Left45,
   ST_AIM_Middle,
   ST_AIM_Right,
   ST_AIM_Right45,
   ST_AIM_Antenne_In,
   ST_AIM_Antenne_Out,
   ST_Barre_In,
   ST_Barre_PreShoot,
   ST_Barre_Shoot,
   ST_Bash_PreShoot,
   ST_Bash_Shoot,
   ST_HIT_Front,
   ST_Intro,
   ST_Laser_In,
   ST_Laser_PreShoot,
   ST_Laser_ShootLoop,
   ST_Lever_Activate,
   ST_Lever_In,
   ST_Lever_Out,
   ST_Run_Front,
   ST_ShockWave_In,
   ST_ShockWave_Out,
   ST_ShockWave_PreShoot,
   ST_ShockWave_Shoot,
   ST_Stun_Pose,
   ST_Stun_Recover,
   ST_Stun_Start,
   ST_Comp_Idle,
   ST_Comp_Poke_Back,
   ST_Comp_Poke_Front,
   ST_Comp_Poke_Left,
   ST_Comp_Poke_Right
 };*/