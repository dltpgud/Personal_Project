#include "stdafx.h"
#include "Player.h"
#include "GameInstance.h"
#include "Body_Player.h"
#include "Weapon.h"
#include "PlayerUI.h"
#include "PlayerEffectUI.h"
#include "Player_StateMachine.h"
CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CActor{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype) : CActor{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    CActor::Actor_DESC Desc{};

    Desc.iNumPartObjects = PART_END;
    Desc.fSpeedPerSec = 4.f; 
    Desc.fRotationPerSec = XMConvertToRadians(60.f);
    Desc.JumpPower = 20.5f; 
 
    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    m_FixY = 2.f;
    m_bOnCell = true;

    m_fMAXHP = 500.f;
    m_fHP = m_fMAXHP;

    m_pPlayerUI = static_cast<CPlayerUI*>(m_pGameInstance->Find_Clone_UIObj(L"playerHP"));
    m_pPlayerUI->Set_PlayerMaxHP(m_fMAXHP);
    m_pPlayerUI->Set_PlayerHP(m_fMAXHP);

    CPlayer_StateMachine::PLAYER_STATEMACHINE_DESC pDesc{};
    pDesc.pParentObject = this;
    pDesc.pCurWeapon = static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Get_Weapon();
    m_pStateMachine = CPlayer_StateMachine::Create(&pDesc);
    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    if (false == m_bUpdate)
        return ;

    
    if (m_pGameInstance->MouseFix())
    {
        _float Y = XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::T_UP));

        _long MouseMove = {0};

        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
        {
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.05f);
        }

        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
        {
            m_pTransformCom->Turn(m_pTransformCom->Get_TRANSFORM(CTransform::T_RIGHT), fTimeDelta * MouseMove * 0.05f);
        }
    }

    m_pStateMachine->StateMachine_Playing(fTimeDelta);
   // if (m_bchange)
   // {
   //     m_iWeaponType == CWeapon::HendGun ? m_Type = T00 : m_Type = T01;
   //    
   //     m_Type == T00 ? m_iState = STATE_SWITCHIN : m_iState = STATE_SWITCHIN2;
   //     m_bchange = false;
   // }
   //
   // if (m_bHit == true) {
   //     m_fHitTime += fTimeDelta;
   //     if (m_fHitTime > 0.3f)
   //     {
   //         m_bHit = false;
   //         m_fHitTime = 0.f;
   //     }
   //
   // }
   // if (m_bHitLock == true)
   // {
   //     m_Type == T00 ? m_iState = STATE_STUN : m_iState = STATE_STUN2;
   // }
   //
   //if (false == m_bHitLock && false == m_bFallLock)
   //{    
   //    if(true == m_bKeyinPut)
   //    Key_Input(fTimeDelta);
   // 
   //}


    __super::Priority_Update(fTimeDelta);
    
}

void CPlayer::Update(_float fTimeDelta)
{
    if (false == m_bUpdate)
        return ;

    //if (false == m_bJump) {
    //    if (m_pNavigationCom->ISFall())
    //    {
    //        Set_onCell(false);
    //        if (XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION)) >= -6.5f)
    //        {
    //            m_bFallLock = true;
    //            m_pTransformCom->Go_Move(CTransform::DOWN ,fTimeDelta);
    //        }
    //
    //        if (XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION)) < -6.5f) {
    //            if (false == m_bBurnSound) {
    // 
    //                m_pGameInstance->Play_Sound(L"ST_Player_Burn_Trigger.ogg", CSound::SOUND_HIT, 1.f);
    //                m_pGameInstance->PlayBGM(CSound::SOUND_HIT, L"ST_Player_Burn_Loop.ogg", 0.5f);
    //                m_bBurnSound = true;
    //            }
    //            m_pGameInstance->Set_OpenUI_Inverse(UIID_PlayerWeaPon_Aim, UIID_InteractiveUI);
    //            m_DemageCellTime += fTimeDelta;
    //            m_bFallLock = false;
    //            HIt_Routine();
    //            m_fHP -= 10.f/100.f;
    //            if (m_DemageCellTime > 1.f) {
    //                Set_onCell(true);
    //                m_bBurnSound = false;
    //                m_pGameInstance->StopSound(CSound::SOUND_HIT);
    //                m_pGameInstance->Play_Sound(L"ST_Player_Burn_Stop.ogg", CSound::SOUND_HIT, 1.f);
    //                
    //                m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, m_pNavigationCom->Get_SafePos());
    //                m_pNavigationCom->Find_CurrentCell(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION));
    //                m_DemageCellTime = 0.f;
    //            }
    //        }
    //    }
    //}
    //
    __super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
//    Is_onDemageCell(fTimeDelta);

    __super::Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{

    return S_OK;
}

void CPlayer::HIt_Routine()
{
    //m_pStateMachine->Set_ADD_Flage(CPlayer_StateNode::MOV_HIT);
    m_pPlayerUI->Set_PlayerHP(m_fHP);
    if (m_pGameInstance->Get_iCurrentLevel() != LEVEL_BOSS) {
        if (m_bBurnSound == false)
            m_pGameInstance->Play_Sound(L"ST_Player_Flash_Stop.ogg", CSound::SOUND_HIT, 1.f);
    }
    m_eUIState = STATE_UI_HIT;
    m_bHit = true;
    m_pGameInstance->Set_OpenUI(UIID_PlayerState, true);
}

void CPlayer::Stun_Routine()
{
    if (false == m_bJump)
    {
        m_bHitLock = true;

        m_pGameInstance->Play_Sound(L"ST_Player_Stun_Loop.wav", CSound::SOUND_PlAYER_STURN, 0.5f);
    }
}

_float CPlayer::Weapon_Damage()
{
    return static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Damage();
}


const _float4x4* CPlayer::Get_CameraBone()
{
    return static_cast<CBody_Player*>(m_PartObjects[PART_BODY])->Get_SocketMatrix("Camera");
}

void CPlayer::WeaponCallBack(_int WeaPonType, _uint AnimIdx, _int Duration, function<void()> func)
{
    static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Weapon_CallBack(WeaPonType, AnimIdx, Duration, func);
}

void CPlayer::BodyCallBack(_int Body,_uint AnimIdx, _int Duration, function<void()> func)
{
    static_cast<CBody_Player*>(m_PartObjects[PART_BODY]) ->BodyCallBack(Body,AnimIdx, Duration, func);
}

//void CPlayer::Key_Input(_float fTimeDelta)
//{
//    
//   if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT))
//   {
//
//       if (m_iState < STATE_HENDGUN_RELOAD)
//       {
//           if (m_iState != STATE_SWITCHIN && m_iState != STATE_SWITCHIN2)
//           {
//               m_Type == T00 ? m_iState = STATE_SPRINT : m_iState = STATE_SPRINT2;
//               m_pGameInstance->Set_OpenUI(UIID_PlayerState, true);
//               
//               if (m_eUIState != STATE_UI_HEALTH)
//                 m_eUIState = STATE_UI_SPRINT;
//
//               m_pTransformCom->Set_MoveSpeed(40.f);
//           }
//       }
//   }
//   else
//   {
//
//       m_pTransformCom->Set_MoveSpeed(20.f);
//       if (false == m_bHit) {
//           if (m_eUIState != STATE_UI_HEALTH)
//           m_pGameInstance->Set_OpenUI(UIID_PlayerState, false);
//       }
//
//       if(m_eUIState != STATE_UI_HEALTH)
//       m_eUIState = STATE_UI_IDlE;
//
//       if (m_iState < STATE_HENDGUN_RELOAD)
//       {
//           if (m_iState != STATE_SWITCHIN && m_iState != STATE_SWITCHIN2)
//           {
//               if (false == m_bJump)
//                   m_Type == T00 ? m_iState = STATE_IDLE : m_iState = STATE_IDLE2;
//           }
//       }
//   }
//
//   if (m_pGameInstance->Get_DIKeyState(DIK_W))
//   {
//       if (m_iState < STATE_HENDGUN_RELOAD)
//       {
//           if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
//           {
//               if (false == m_bJump)
//               {               
//                   m_Type == T00 ? m_iState = STATE_RUN : m_iState = STATE_RUN2;
//               }
//           }
//       }
//       m_pTransformCom->Go_Move(CTransform::GO, fTimeDelta, m_pNavigationCom);
//      
//   }
//   if (m_pGameInstance->Get_DIKeyState(DIK_S))
//   {
//       if (m_iState < STATE_HENDGUN_RELOAD)
//       {
//           if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
//           {
//               if (false == m_bJump)
//               {
//                   m_Type == T00 ? m_iState = STATE_RUN : m_iState = STATE_RUN2;
//               }
//           }
//       }
//       m_pTransformCom->Go_Move(CTransform::BACK ,fTimeDelta, m_pNavigationCom);
//   }
//
//   if (m_pGameInstance->Get_DIKeyState(DIK_A))
//   {
//       if (m_iState < STATE_HENDGUN_RELOAD)
//       {
//           if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
//           {
//               if (false == m_bJump)
//               {
//                   m_Type == T00 ? m_iState = STATE_RUN : m_iState = STATE_RUN2;
//               }
//           }
//       }
//
//       m_pTransformCom->Go_Move(CTransform::LEFT,fTimeDelta, m_pNavigationCom);
//   }
//
//   if (m_pGameInstance->Get_DIKeyState(DIK_D))
//   {
//       if (m_iState < STATE_HENDGUN_RELOAD)
//       {
//           if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
//           {
//               if (false == m_bJump)
//               {
//                   m_Type == T00 ? m_iState = STATE_RUN : m_iState = STATE_RUN2;
//               }
//           }
//       }
//       m_pTransformCom->Go_Move(CTransform::RIGHT, fTimeDelta, m_pNavigationCom);
//   }

//
//   if (m_pGameInstance->Get_DIKeyDown(DIK_R))
//   {
//       if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
//       {
//           m_pGameInstance->Set_OpenUI(UIID_PlayerShooting, false); 
//
//           if (CWeapon::HendGun == m_iWeaponType && T00 == m_Type)
//               m_iState = STATE_HENDGUN_RELOAD;
//
//           if (T01 == m_Type)
//           {
//               if (CWeapon::AssaultRifle == m_iWeaponType)
//                   m_iState = STATE_ASSAULTRIFlLE_RELOAD;
//
//               if (CWeapon::MissileGatling == m_iWeaponType)
//                   m_iState = STATE_MissileGatling_RELOAD;
//
//               if (CWeapon::HeavyCrossbow == m_iWeaponType)
//                   m_iState = STATE_HEAVYCROSSBOW_RELOAD;
//           }
//
//       }
//   }
//
//   if (m_iState != STATE_HENDGUN_RELOAD && m_iState != STATE_ASSAULTRIFlLE_RELOAD &&
//       m_iState != STATE_MissileGatling_RELOAD && m_iState != STATE_HEAVYCROSSBOW_RELOAD)
//   {
//       if (m_pGameInstance->Get_DIMouseState(DIM_LB))
//       {
//
//           if (CWeapon::HendGun == m_iWeaponType && T00 == m_Type)
//               m_iState = STATE_HENDGUN_SHOOT;
//
//     
//           if (T01 == m_Type)
//           {
//               if (CWeapon::AssaultRifle == m_iWeaponType)
//                   m_iState = STATE_ASSAULTRIFlLE_SHOOT;
//
//               if (CWeapon::MissileGatling == m_iWeaponType)
//                   m_iState = STATE_MissileGatling_SHOOT;
//
//               if (CWeapon::HeavyCrossbow == m_iWeaponType)
//                   m_iState = STATE_HEAVYCROSSBOW_SHOOT;
//           }
//
//       }
//   }
//   
//
//   if (m_pGameInstance->Get_DIKeyDown(DIK_SPACE))
//   {
//       m_bJump = true;
//       m_bOnCell = false;
//       m_iJumpCount++;
//   }
//
//   if (m_bJump)
//   {
//       if (false == m_bDoubleJump) {
//           m_Type == T00 ? m_iState = STATE_JUMP_RUN_LOOP : m_iState = STATE_JUMP_RUN_LOOP2;
//       }
//           m_pTransformCom->Go_jump(fTimeDelta, m_fY, &m_bJump);
//
//           if (false == m_bJumpSound) {
//               m_pGameInstance->Play_Sound(L"ST_Player_Jump.ogg", CSound::SOUND_BGM, 1.f);
//               m_bJumpSound = true;
//           }
//
//
//       if (m_iJumpCount >= 2)
//       {
//           m_bDoubleJump = true;
//       }
//       if (true == m_bDoubleJump && m_bJump == true)
//       {
//
//          if(false == m_bDoubleJumpSound)
//          { m_pGameInstance->Play_Sound(L"ST_Player_DoubleJump.ogg", CSound::SOUND_BGM, 0.5f);
//             m_bDoubleJumpSound = true;
//          }
//
//           m_pTransformCom->Go_Doublejump(fTimeDelta);
//           m_Type == T00 ? m_iState = STATE_JUMP_RUN_LOW : m_iState = STATE_JUMP_RUN_LOW2;
//       }
//
//       if (!m_bJump)
//       {
//           m_bJumpSound = false;
//           m_bDoubleJumpSound = false;
//           m_iJumpCount = 0;
//           m_bDoubleJump = false;
//           m_bOnCell = true;
//           if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
//               m_Type == T00 ? m_iState = STATE_IDLE : m_iState = STATE_IDLE2;
//       }
//   }


void CPlayer::Choose_Weapon(const _uint& WeaponNum)
{
    m_bchange = true;
    static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Choose_Weapon(WeaponNum);
 //   m_pStateMachine->Set_NextNodeIndex(NODE_SWICH);
}

_uint CPlayer::Get_Bullet()
{
    return static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Get_Bullet();
}

_uint CPlayer::Get_MaxBullet()
{
    return static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Get_MaxBullet();
}

void CPlayer::Set_HitLock(_bool Lock)
{
    m_bHitLock = Lock;
}

void CPlayer::Set_PartObj_Set_Anim(_int Part,_int Index, _bool IsLoop)
{
    switch (Part)
    {
    case PART_BODY:
        static_cast<CBody_Player*>(m_PartObjects[Part])-> Set_Animation(static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Get_Weapon_Body_Type(), Index, IsLoop);
        break;

    case PART_WEAPON: static_cast<CWeapon*>(m_PartObjects[Part])->Set_Animation(Index, IsLoop);
        break;
    }
}

_bool CPlayer::Set_PartObj_Play_Anim(_int Part, _float fTimeDelta, _float fPlayAniTime)
{
    switch (Part)
    {
    case PART_BODY: 
        return static_cast<CBody_Player*>(m_PartObjects[Part])->Play_Animation(fTimeDelta * fPlayAniTime);
         break;

    case PART_WEAPON:
        return static_cast<CWeapon*>(m_PartObjects[Part])->Play_Animation(fTimeDelta * fPlayAniTime); 
         break;
    }

    return true;
}

HRESULT CPlayer::Add_Components()
{
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};
    OBBDesc.vExtents = _float3(0.5f, 0.75f, 0.5f);
    OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);
    OBBDesc.vRotation = {};
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider_OBB"), 
                                      reinterpret_cast<CComponent**>(&m_pColliderCom),&OBBDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
                                      reinterpret_cast<CComponent**>(&m_pNavigationCom))))
       return E_FAIL;
    
    return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
    CBody_Player::BODY_PLAYER_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_Player"), PART_BODY, &BodyDesc)))
        return E_FAIL;

    CWeapon::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pSocketMatrix = static_cast<CBody_Player*>(m_PartObjects[PART_BODY])->Get_SocketMatrix("Weapon_Attachement");

    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Weapon"), PART_WEAPON, &WeaponDesc)))
        return E_FAIL;

    CPlayerEffectUI::CPlayerEffectUI_DESC Desc{};
    Desc.State = &m_eUIState;
    Desc.iDeleteLevel = LEVEL_STATIC;
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"Runstate", L"Prototype GameObject_PlayerEffectUI",&Desc)))
        return E_FAIL;

   return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayer");
        Safe_Release(pInstance);
    }

   return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CPlayer");
        Safe_Release(pInstance);
    }

   return pInstance;
}



void CPlayer::Free()
{
	__super::Free();


	Safe_Release(m_pStateMachine);
}

