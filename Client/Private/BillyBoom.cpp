#include "stdafx.h"
#include "BillyBoom.h"
#include "GameInstance.h"
#include "Body_BillyBoom.h"
#include "BossIntroBG.h"
#include "BossHP.h"
#include "Particle_Explosion.h"
#include"Pade.h"
CBillyBoom::CBillyBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CActor{pDevice, pContext}
{
}

CBillyBoom::CBillyBoom(const CBillyBoom& Prototype) : CActor{Prototype}
{
}

HRESULT CBillyBoom::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBillyBoom::Initialize(void* pArg)
{

    CActor::Actor_DESC Desc{};
    Desc.iNumPartObjects = PART_END;
    Desc.fSpeedPerSec =  7.f;
    Desc.fRotationPerSec = XMConvertToRadians(120.f);
    Desc.JumpPower = 3.f;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    m_iState = ST_Idle;
   m_fMAXHP = 1000.f;
   m_fHP = m_fMAXHP;
   m_bOnCell = true;
   m_FixY = 0.5f;
   m_DATA_TYPE = CGameObject::DATA_MONSTER;





    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    return S_OK;
}

_int CBillyBoom::Priority_Update(_float fTimeDelta)
{
    if (m_bDead) {
        if (m_bPade == false) {
            static_cast<CPade*>(m_pGameInstance->Get_UI(LEVEL_STATIC, CUI::UIID_Pade))->Set_Pade(true);
            m_pGameInstance->Set_OpenUI(CUI::UIID_Pade, true);
            m_bPade = true;
        }
         if (static_cast<CPade*>(m_pGameInstance->Get_UI(LEVEL_STATIC, CUI::UIID_Pade))->FinishPade())
             {
                 m_pGameInstance->Set_Open_Bool(true);
                 return OBJ_DEAD;
             }
    }

    if(m_iState != ST_Comp_Idle)
    Set_State_From_Body();

     if(true == m_bIntro)
    m_pTransformCom->Set_Rotation_to_Player();

    if (true == m_bStart) {

        m_pTransformCom->Rotation_to_Player(fTimeDelta);
        m_bStart = false;
    }

    if (nullptr != m_pHP) {
        m_pHP->Set_BossHP(m_fHP);
    }
 
    if (true == m_bHit)
    {
        m_fHitTimeSum += fTimeDelta;

        if(m_iState == ST_Idle)
            m_iState = ST_Comp_Poke_Front;

        if (m_fHitTimeSum > 0.5f)
        {
            m_bHit = false;
            m_fHitTimeSum = 0.f;
        }
    }

    if(m_iState == ST_Idle)
        m_bStart = true;

    if (false == m_bHit)
        m_iRim = RIM_LIGHT_DESC::STATE_NORIM;

    if (m_PartObjects[PART_BODY]->Get_Finish())
    {
        m_iState = ST_Idle;
            m_bSkill = false;
            m_bHit = false;
    }   

    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CBillyBoom::Update(_float fTimeDelta)
{
         


 if (m_iState != ST_Intro && m_bIntro == false) {
  
     if (m_bSkill != true) {
        NON_intersect(fTimeDelta); 
        Change_State(fTimeDelta); 
     }
 }

    __super::Update(fTimeDelta);

}

void CBillyBoom::Late_Update(_float fTimeDelta)
{
    if (m_iSkillTime >= 3) {
        m_iState = ST_Idle;
        m_iSkillTime = 0;
        for (_int i = 0; i < 3; i++) {
            m_iPrAttack[i] = -1;
        }
    }



    if (FAILED(m_pGameInstance->Add_Monster(this)))
        return;
    __super::Late_Update(fTimeDelta);
}

HRESULT CBillyBoom::Render()
{
    __super::Render();
    return S_OK; 
}

void CBillyBoom::HIt_Routine()
{
    m_bHit = true;
    m_iRim = RIM_LIGHT_DESC::STATE_RIM;
  
}

void CBillyBoom::Dead_Routine(_float fTimeDelta)
{
    m_bSkill = true;
    m_bHit = true;
    m_iState = ST_Comp_Idle;
    m_bOnCell = false;

    m_DeadTimeSum += fTimeDelta;

    if (false == m_bDeadSound) {
        m_pGameInstance->Play_Sound(L"ST_BillyBoom_Outro.ogg", CSound::SOUND_EFFECT, 1.f);
        m_bDeadSound = true;
    }

    if (m_DeadTimeSum > 2.f)
    {    m_iRim = RIM_LIGHT_DESC::STATE_RIM;
        if (false == m_bBoom)
        {

            CParticle_Explosion::CParticle_Explosion_Desc Desc{};
            Desc.pParentMatrix = m_pTransformCom->Get_WorldMatrix();

            m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, CGameObject::SKILL, L"Prototype_GameObject_Particle_Explosion", nullptr, 0, &Desc, 0);
            m_bBoom = true;
        }   
        m_pTransformCom->Set_MoveSpeed(2.f);
        m_pTransformCom->Go_Down(fTimeDelta);
    }

    if (XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)) < -20.f)
        m_bDead = true;
}

void CBillyBoom::Stun_Routine()
{
    m_iState = ST_Stun_Start;
}

void CBillyBoom::NON_intersect(_float fTimedelta)
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPlayerPos - vPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));



    if (fLength < 40.f)
    {

        if ( true == m_bHit)
            m_iState = ST_Comp_Poke_Front;
        else
        {
            m_iState = ST_Run_Front;
            m_pTransformCom->Go_Straight(fTimedelta, m_pNavigationCom, true);
            m_bStart = true;
        }

         if (fLength < 30.f)
         {
             m_bSkill = false;
         }
         if (fLength < 25.f)
         {
             m_iState = ST_Idle;
         }


    }
    else {
        m_bSkill = false;

    }
}

void CBillyBoom::Intro_Routine(_float fTimedelta)
{

    if (true == m_bIntro)
    {
        m_iState = ST_Intro;

     
        
        CBossHPUI::CBossHPUI_DESC  Desc{};
        Desc.fMaxHP = m_fMAXHP;
        Desc.fHP = m_fHP;
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, CGameObject::UI, L"Prototype_GameObject_Boss_HP",
            nullptr, 0, &Desc)))
            return;

        m_pGameInstance->Set_OpenUI(CUI::UIID_BossHP, false);
            
        m_pHP = static_cast< CBossHPUI*>(m_pGameInstance->Get_UI(LEVEL_BOSS, CUI::UIID_BossHP));
        m_pHP->Set_BossMaxHP(m_fMAXHP);

       _vector vWorldPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
        CBossIntroBG::CBossIntroBG_DESC IntroDesc{};
        IntroDesc.fX = XMVectorGetX(vWorldPos);
        IntroDesc.fY = XMVectorGetY(vWorldPos);
        IntroDesc.fZ = XMVectorGetZ(vWorldPos);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, CGameObject::MAP, L"Prototype_GameObject_Boss_InstroUI",
            nullptr, 0, &IntroDesc)))
            return;

        m_bIntro = false;
    }
}

void CBillyBoom::Change_State(_float fTimedelta)
{
    if (m_iState != ST_Idle)
        return;
    _int RandomAttack{};

   while (true)
   {
       _bool isSame = false;
       RandomAttack = rand() % 4;
  
       for (_int i = 0; i < 4; i++)
       {
           if (m_iPrAttack[i] == RandomAttack)
           isSame = true;
       }
  
       if (!isSame)
       {
           m_iPrAttack[m_iSkillTime] = RandomAttack;
  
           m_iSkillTime++;
           break;
       }
   }
  
     switch (RandomAttack)
     {
     case 0:
         Set_Barre_Shoot(fTimedelta);
         break;
   
     case 1:
         Set_Laser(fTimedelta);
         break;
   
     case 2:
         Set_ShockWave(fTimedelta);
         break;

     case 3:
         Set_Bash(fTimedelta);
         break;
     }

}

void CBillyBoom::Set_Barre_Shoot(_float fTimedelta)
{
    static_cast<CBody_BillyBoom*>(m_PartObjects[PART_BODY])->SetDir();
    m_iState = ST_Barre_In;
    m_bSkill = true;
    m_bStart = true;
}

void CBillyBoom::Set_Laser(_float fTimedelta)
{
    m_iState = ST_Laser_In;
    m_bSkill = true;
}

void CBillyBoom::Set_ShockWave(_float fTimedelta)
{
    static_cast<CBody_BillyBoom*>(m_PartObjects[PART_BODY])->SetDir();
    m_iState = ST_ShockWave_In;
    m_bSkill = true;
}

void CBillyBoom::Set_Bash(_float fTimedelta)
{
    m_iState = ST_Bash_PreShoot;
    m_bSkill = true;
}

void CBillyBoom::Set_State_From_Body()
{
    switch (static_cast<CBody_BillyBoom*>(m_PartObjects[PART_BODY])->Get_BodyState())
    {
    case ST_Bash_Shoot:
        m_iState = ST_Bash_Shoot;
        break;

    case ST_ShockWave_PreShoot:
        m_iState = ST_ShockWave_PreShoot;
        break;

    case ST_ShockWave_Shoot:
        m_iState = ST_ShockWave_Shoot;
        break;

    case ST_Laser_PreShoot:
        m_iState = ST_Laser_PreShoot;
        break;


    case ST_Laser_ShootLoop:
        m_iState = ST_Laser_ShootLoop;
        break;

    case ST_Barre_PreShoot:
        m_iState = ST_Barre_PreShoot;
        break;

    case ST_Barre_Shoot:
        m_iState = ST_Barre_Shoot;
        break;
    }  
}

HRESULT CBillyBoom::Add_Components()
{    
    /* For.Com_Collider_AABB */
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};

    OBBDesc.vExtents = _float3(3.f, 3.3f, 3.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);
    OBBDesc.vRotation = { 0.f, 0.f, 0.f };
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider_OBB"),
        reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;


    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBillyBoom::Add_PartObjects()
{
    /* For.Body */
    CBody_BillyBoom::CBody_BillyBoom_Desc BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pRimState = &m_iRim;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_BillyBoom"), PART_BODY, &BodyDesc)))
        return E_FAIL;

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
}
