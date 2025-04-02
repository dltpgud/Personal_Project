#include "stdafx.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI.h"
#include "ShootEffect.h"
#include "ShootingUI.h"
#include "PlayerBullet.h"
#include "ShockWave.h"
#include"Shock.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon& Prototype) : CPartObject{ Prototype }
{
}

HRESULT CWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

    m_pParentState = pDesc->pParentState;
    m_pSocketMatrix = pDesc->pSocketMatrix;
    m_pType = pDesc->pType;
    m_pWeapon = 0;
    m_iCurMotion = Idle;

    m_iMaxBullet[CWeapon::HendGun] = 15;
    m_iMaxBullet[CWeapon::AssaultRifle] = 30;
    m_iMaxBullet[CWeapon::MissileGatling] = 20;
    m_iMaxBullet[CWeapon::HeavyCrossbow] = 7;

    m_iFirstBullet[CWeapon::HendGun] = 15;
    m_iFirstBullet[CWeapon::AssaultRifle] = 30;
    m_iFirstBullet[CWeapon::MissileGatling] = 20;
    m_iFirstBullet[CWeapon::HeavyCrossbow] = 7;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(0.01f, 0.01f, 0.01f);

    m_pWeapon_SocketMatrix[HendGun]        = m_pModelCom[HendGun]->Get_BoneMatrix("W_Trigger");
    m_pWeapon_SocketMatrix[AssaultRifle]   = m_pModelCom[AssaultRifle]->Get_BoneMatrix("W_Bayonet");
    m_pWeapon_SocketMatrix[MissileGatling] = m_pModelCom[MissileGatling]->Get_BoneMatrix("W_Rotator");
    m_pWeapon_SocketMatrix[HeavyCrossbow]  = m_pModelCom[HeavyCrossbow]->Get_BoneMatrix("W_Front_Middle");
    m_fEmissivePower = 1.f;
    m_fPlayAniTime = 1.f;
  


    CShootingUI::CShootingUI_DESC SDesc{};
    SDesc.iWeaPonTYPE = &m_pWeapon;
    SDesc.iWeaPonState = &m_iCurMotion;
    SDesc.fX = g_iWinSizeX * 0.7f;
    SDesc.fY = g_iWinSizeY * 0.57f;
    SDesc.fSizeX = 100.f;
    SDesc.fSizeY = 100.f;
    SDesc.UID = CUI::UIID_PlayerShooting;
    m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, CGameObject::UI, L"Prototype GameObject_ShootingUI",nullptr,0,&SDesc,0);
    m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, false);

    m_ShootingUI = static_cast<CShootingUI*>( m_pGameInstance->Get_UI(LEVEL_STATIC, CUI::UIID_PlayerShooting));


    return S_OK;
}

_int CWeapon::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;
    return OBJ_NOEVENT;
}

void CWeapon::Update(_float fTimeDelta)
{

    *m_pType == CPlayer::T00 ? Type0_Update(fTimeDelta) : Type2_Update(fTimeDelta);
    m_pGameInstance->UI_shaking(CUI::UIID_PlayerHP, fTimeDelta);
    m_pGameInstance->UI_shaking(CUI::UIID_PlayerWeaPon, fTimeDelta);
    m_pGameInstance->UI_shaking(CUI::UIID_PlayerWeaPon_Aim, fTimeDelta);
    m_pGameInstance->UI_shaking(CUI::UIID_BossHP, fTimeDelta);
  
    if (0 >= m_iMaxBullet[m_iBullet])
    {
        m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, false);
    }
}

void CWeapon::Late_Update(_float fTimeDelta)
{
   _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
  
    for (size_t i = 0; i < 3; i++)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]); // 항등으로 만들어서 넣겠다..
  
    XMStoreFloat4x4(&m_WorldMatrix,
         m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
 

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CWeapon::Render()
{
    
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom[m_pWeapon]->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom[m_pWeapon]->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
            "g_DiffuseTexture")))
            return E_FAIL;

        if (m_pWeapon != MissileGatling) {
            if (FAILED(m_pModelCom[m_pWeapon]->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_EMISSIVE, 0,
                "g_EmissiveTexture")))
                return E_FAIL;
        }
        
        if (FAILED(m_pModelCom[m_pWeapon]->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (m_pWeapon != MissileGatling) {
            if (FAILED(m_pShaderCom->Begin(3)))
                return E_FAIL;
        }
        else 
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

        m_pModelCom[m_pWeapon]->Render(i);
    }

    return S_OK;
}

void CWeapon::Type0_Update(_float fTimeDelta)
{
    _bool bMotionChange = { false }, bLoop = { false };
    if (*m_pParentState == CPlayer::STATE_HENDGUN_RELOAD && m_iCurMotion != Reload)
    {
        m_fEmissivePower = 2.f;
        m_iBullet = CWeapon::HendGun;
        m_iMaxBullet[m_iBullet] = 15;
        m_iCurMotion = Reload;
        bMotionChange = true;
        bLoop = false;
    }

   m_pModelCom[m_pWeapon]->Callback(0, 15,
                                         [this]()
                                         {
                                         if (false == m_bSound)
                                             {
                                                 m_pGameInstance->Play_Sound(L"ST_Handgun_Reload.ogg",
                                                                             CSound::SOUND_BGM, 1.f);
                                                 m_bSound = true;
                                             }

                                         });

   m_pModelCom[m_pWeapon]->Callback(0, 20,
                                         [this]()
                                         {
                                             m_ShootingUI->Set_RandomPos(false, true, false);
                                             m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, true);
                                           
                                         }); 
   
   m_pModelCom[m_pWeapon]->Callback(0, 40,
                                        [this]()
                                        {
                                            m_ShootingUI->Set_RandomPos(false, false, false);
                                            m_ShootingUI->Set_PosClack(-200.f, 80.f);
                                        }); 
 
   m_pModelCom[m_pWeapon]->Callback(2, 1,
                                        [this]()
                                        {
                                                 m_ShootingUI->Set_RandomPos(true, false, false);
                                              m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, true);
                                        });


    if (*m_pParentState == CPlayer::STATE_HENDGUN_SHOOT && m_iCurMotion != Shoot)
    {
        m_fEmissivePower = 1.5f;
        m_fWeaPonOffset = { 1.7f,2.0f,3.0f };
        Make_Bullet(m_fWeaPonOffset);
        m_iCurMotion = Shoot;
        m_fDamage = 10;

        m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon, 0.2f, 0.2f, 0.2f);
        m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerHP, 0.2f, -0.2f, 0.2f);
        m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon_Aim, 0.2f, 0.2f, -0.2f);
        m_pGameInstance->Set_UI_shaking(CUI::UIID_BossHP, 0.2f, 0.2f, -0.2f);
        bMotionChange = true;
        bLoop = false;
    }

    if (bMotionChange)
        m_pModelCom[m_pWeapon]->Set_Animation(m_iCurMotion, bLoop);

    if (true == m_pModelCom[m_pWeapon]->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, false);
        static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_IDLE);
        m_iCurMotion = Idle;
        m_pModelCom[m_pWeapon]->Set_Animation(m_iCurMotion, true);
        m_bSound = false;
    }

}

void CWeapon::Type2_Update(_float fTimeDelta)
{
    _bool bMotionChange = { false }, bLoop = { false };

    if (AssaultRifle == m_pWeapon)
    {
        if (*m_pParentState == CPlayer::STATE_ASSAULTRIFlLE_RELOAD && m_iCurMotion != Reload)
        {
            m_fEmissivePower = 2.f;
            m_fPlayAniTime = 1.2f;
            m_iBullet = CWeapon::AssaultRifle;
            m_iMaxBullet[m_iBullet] = 30;
            m_iCurMotion = Reload;
            bMotionChange = true;
            bLoop = false;
        }

        if (*m_pParentState == CPlayer::STATE_ASSAULTRIFlLE_SHOOT && m_iCurMotion != Shoot)
        {
            m_fWeaPonOffset = { 1.1f,0.8f,1.2f };
            Make_Bullet(m_fWeaPonOffset);
            m_fEmissivePower = 1.5f;
            m_iCurMotion = Shoot;
            m_fDamage = 15.f;
            m_fPlayAniTime = 2.f;
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon, 0.2f, 0.3f, 0.3f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerHP, 0.2f, -0.3f, 0.3f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon_Aim, 0.2f, 0.3f, -0.3f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_BossHP, 0.2f, 0.3f, -0.3f);
            bMotionChange = true;
            bLoop = false;
        }
    }
    else if (MissileGatling == m_pWeapon)
    {

        if (*m_pParentState == CPlayer::STATE_MissileGatling_RELOAD && m_iCurMotion != Reload)
        {     m_fPlayAniTime = 1.2f;
            m_iBullet = CWeapon::MissileGatling;
            m_iMaxBullet[m_iBullet] = 20;
            m_iCurMotion = Reload;
            bMotionChange = true;
            bLoop = false;
        }

        if (*m_pParentState == CPlayer::STATE_MissileGatling_SHOOT && m_iCurMotion != Shoot)
        {
            m_fWeaPonOffset = { 1.f,1.f,1.5f };
            m_pGameInstance->Play_Sound(L"ST_MissileGatling_PF_Shoot.ogg", CSound::SOUND_BGM, 0.5f);
            Make_Bullet(m_fWeaPonOffset);
            Make_Bullet(m_fWeaPonOffset);
            Make_Bullet(m_fWeaPonOffset);
            Make_Bullet(m_fWeaPonOffset);
            m_iCurMotion = Shoot;
            m_fDamage = 20.f;
            m_fPlayAniTime = 2.f;
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon, 0.2f, 0.6f, 0.6f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerHP, 0.2f, -0.6f, 0.6f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon_Aim, 0.2f, 0.6f, -0.6f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_BossHP, 0.2f, 0.6f, -0.6f);
            bMotionChange = true;
            bLoop = false;
        }
    }
    else if (HeavyCrossbow == m_pWeapon)
    {
        m_fPlayAniTime = 1.f;
        if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_RELOAD && m_iCurMotion != Reload)
        {
            m_fEmissivePower = 2.f;
            m_iBullet = CWeapon::HeavyCrossbow;
            m_iMaxBullet[m_iBullet] = 7;
            m_iCurMotion = Reload;
        
            bMotionChange = true;
            bLoop = false;
        }

        if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_SHOOT && m_iCurMotion != Shoot)
        {
            m_fEmissivePower = 1.5f;
            m_fWeaPonOffset = { 1.f,1.f,1.5f };
            Make_Bullet(m_fWeaPonOffset);
            m_iCurMotion = Shoot;
            m_fDamage = 90.f;
       
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon, 0.2f, 1.f, 1.f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerHP, 0.2f, -1.f, 1.f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon_Aim, 0.2f, 1.f, -1.f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_BossHP, 0.2f, 1.f, -1.f);
            bMotionChange = true;
            bLoop = false;
        }
    }

  

    if (*m_pParentState == CPlayer::STATE_ASSAULTRIFlLE_RELOAD)
    {
        m_pModelCom[m_pWeapon]->Callback(0, 15,
                                         [this]()
                                         {
                                             if (false == m_bSound)
                                             {
                                                 m_pGameInstance->Play_Sound(L"ST_AssaultRifle_Reload.ogg",
                                                                             CSound::SOUND_BGM, 1.f);
                                                 m_bSound = true;
                                             }
                                         });

        m_pModelCom[m_pWeapon]->Callback(0, 20,
                                         [this]()
                                         {
                                             m_ShootingUI->Set_RandomPos(false, true, false);
                                             m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, true);
                                         });

         m_pModelCom[m_pWeapon]->Callback(0, 78,
                                         [this]()
                                         {
                                                m_ShootingUI->Set_RandomPos(false, false, false);
                                                m_ShootingUI->Set_PosClack(-250.f, 60.f);
                                         });

    }
    if (*m_pParentState == CPlayer::STATE_ASSAULTRIFlLE_SHOOT) {

           m_pModelCom[m_pWeapon]->Callback(2, 1,
                                         [this]()
                                         {
                                             m_ShootingUI->Set_RandomPos(true, false, false);
                                             m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, true);
                                         });
    }


    if (*m_pParentState == CPlayer::STATE_MissileGatling_RELOAD)
    {
        m_pModelCom[m_pWeapon]->Callback(0, 15,
                                         [this]()
                                         {
                                             if (false == m_bSound)
                                             {
                                                 m_pGameInstance->Play_Sound(L"ST_MissileGatling_Reload.ogg",
                                                                             CSound::SOUND_BGM, 0.5f);
                                                 m_bSound = true;
                                             }
                                         });
        m_pModelCom[m_pWeapon]->Callback(0, 30,
                                         [this]()
                                         {
                                             m_ShootingUI->Set_RandomPos(false, true, false);
                                             m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, true);
                                         });
    }
    if (*m_pParentState == CPlayer::STATE_MissileGatling_SHOOT)
    {
        m_pModelCom[m_pWeapon]->Callback(2, 1,
                                         [this]()
                                         {
                                             m_ShootingUI->Set_RandomPos(true, false, false);
                                             m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, true);
                                         });
    }

    if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_RELOAD)
    {
        m_pModelCom[m_pWeapon]->Callback(0, 25,
                                         [this]()
                                         {
                                             if (false == m_bSound)
                                             {
                                                 m_pGameInstance->Play_Sound(L"ST_HeavyCrossbow_Reload.ogg",
                                                                             CSound::SOUND_BGM, 0.5f);
                                                 m_bSound = true;
                                             }
                                         }); 

        m_pModelCom[m_pWeapon]->Callback(0, 30,
                                         [this]()
                                         {
                                             m_ShootingUI->Set_RandomPos(false, true, false);
                                             m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, true);
                                         });
    }

    if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_SHOOT)
    {
        m_pModelCom[m_pWeapon]->Callback(2, 1,
                                         [this]()
                                         {
                                             m_ShootingUI->Set_RandomPos(true, false, false);
                                             m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, true);
                                         });
    }


 

    if (bMotionChange)
        m_pModelCom[m_pWeapon]->Set_Animation(m_iCurMotion, bLoop);

    if (true == m_pModelCom[m_pWeapon]->Play_Animation(fTimeDelta * m_fPlayAniTime))
    {
        m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerShooting, false);
        static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_IDLE2);
        m_iCurMotion = Idle;
        m_pModelCom[m_pWeapon]->Set_Animation(m_iCurMotion, true);
        m_bSound = false;
    }
  

}



void CWeapon::Choose_Weapon(const _uint& WeaponNum)
{
    m_pWeapon = WeaponNum;

    m_iBullet = static_cast<WeaPoneType>(WeaponNum);
    m_pModelCom[m_pWeapon]->Set_Animation(Idle, false);
}

HRESULT CWeapon::Make_Bullet(_float3 Offset)
{
    if (m_pWeapon >= WeaPoneType_END || m_pWeapon < HendGun)
        return E_FAIL;

    m_iMaxBullet[m_iBullet] -= 1;

    switch (m_pWeapon)
    {
    case CWeapon::HendGun:
        m_pGameInstance->Play_Sound(L"ST_Handgun_PF_Shoot.ogg", CSound::SOUND_BGM, 0.5f);
        break;
    case CWeapon::AssaultRifle:
        m_pGameInstance->Play_Sound(L"ST_AssaultRifle_PF_Shoot.ogg", CSound::SOUND_BGM, 0.5f);
        break;
    case CWeapon::HeavyCrossbow:
        m_pGameInstance->Play_Sound(L"ST_HeavyCrossbow_SF_Shoot_A.ogg", CSound::SOUND_BGM, 0.5f);
        break;
    }

    if (0 >= m_iMaxBullet[m_iBullet])
    {
        switch (m_pWeapon)
        {
        case CWeapon::HendGun : 
           m_pGameInstance->Get_Player()->Set_State(CPlayer::STATE_HENDGUN_RELOAD);
            break;
        case CWeapon::AssaultRifle:
          m_pGameInstance->Get_Player()->Set_State(CPlayer::STATE_ASSAULTRIFlLE_RELOAD);
            break;
        case CWeapon::MissileGatling:
           m_pGameInstance->Get_Player()->Set_State(CPlayer::STATE_MissileGatling_RELOAD);
            break;
        case CWeapon::HeavyCrossbow:
            m_pGameInstance->Get_Player()->Set_State(CPlayer::STATE_HEAVYCROSSBOW_RELOAD);
            break;
        }
    }
    else
    m_pGameInstance->Player_To_Monster_Ray_Collison_Check();
   
    _float3	vPickPos{};
    _vector At{};

    if (true == m_pGameInstance->IsPicked(&vPickPos, true)) 
    {
        At = XMLoadFloat3(&vPickPos);
    }
    if (false == m_pGameInstance->IsPicked(&vPickPos, true))
    {
      At = XMLoadFloat4(m_pGameInstance->Get_CamPosition()) + XMLoadFloat4(m_pGameInstance->Get_CamLook()) * 10.f;
    }

    _vector Hend_Local_Pos = { m_pWeapon_SocketMatrix[m_pWeapon]->_41 * Offset.x, m_pWeapon_SocketMatrix[m_pWeapon]->_42 * Offset.y,
             m_pWeapon_SocketMatrix[m_pWeapon]->_43 * Offset.z, m_pWeapon_SocketMatrix[m_pWeapon]->_44 };

    _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(&m_WorldMatrix));

    _vector Local_Pos = { m_pWeapon_SocketMatrix[m_pWeapon]->_41 * Offset.x, m_pWeapon_SocketMatrix[m_pWeapon]->_42 * Offset.y+5.f,
          m_pWeapon_SocketMatrix[m_pWeapon]->_43 * Offset.z, m_pWeapon_SocketMatrix[m_pWeapon]->_44 };

   CShootEffect::CShootEffect_DESC pDesc{};
    pDesc.vPos = vHPos;
    pDesc.vTgetPos = { m_WorldMatrix._41, m_WorldMatrix._42,  m_WorldMatrix._43,  m_WorldMatrix._44 };
    pDesc.Local = Local_Pos;
    pDesc.WorldPtr = &m_WorldMatrix;
    pDesc.iWeaponType = m_pWeapon;
    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), CGameObject::SKILL, L"Prototype GameObject_ShootEffect", nullptr, 0, &pDesc, 0);
 
    CPlayerBullet::CPlayerBullet_DESC Desc{};
    Desc.fSpeedPerSec = 150.f;
    Desc.pTagetPos = At;
    Desc.vPos = vHPos;
    Desc.iSkillType = m_pWeapon;
    Desc.fDamage = &m_fDamage;
    Desc.Local = Hend_Local_Pos;
    Desc.WorldPtr = &m_WorldMatrix;
    Desc.iWeaponType = m_pWeapon;
    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), CGameObject::SKILL, L"Prototype GameObject_PlayerBullet", nullptr, 0, &Desc, 0);

    return S_OK;
}

HRESULT CWeapon::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HendGun"), TEXT("Com_Model"),
        reinterpret_cast<CComponent**>(&m_pModelCom[HendGun]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_AssaultRifle"), TEXT("Com_Model2"),
        reinterpret_cast<CComponent**>(&m_pModelCom[AssaultRifle]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MissileGatling"),
        TEXT("Com_Model3"),
        reinterpret_cast<CComponent**>(&m_pModelCom[MissileGatling]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HeavyCrossbow"), TEXT("Com_Model4"),
        reinterpret_cast<CComponent**>(&m_pModelCom[HeavyCrossbow]))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
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
    if (m_pWeapon != MissileGatling) {
        if (FAILED(m_pShaderCom->Bind_Float("g_EmissivePower", m_fEmissivePower)))
            return E_FAIL;
    }
 
    return S_OK;
}

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon* pInstance = new CWeapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
    CWeapon* pInstance = new CWeapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon::Free()
{
    __super::Free();

    for (size_t i = 0; i < WeaPoneType_END; i++) Safe_Release(m_pModelCom[i]);

}
