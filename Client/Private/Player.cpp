#include "stdafx.h"
#include "Player.h"
#include "GameInstance.h"
#include "Body_Player.h"
#include "Weapon.h"
#include "PlayerUI.h"
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
    Desc.fSpeedPerSec = 15.f;
    Desc.fRotationPerSec = XMConvertToRadians(60.f);
    Desc.JumpPower = 2.4f;

    m_Type = T00;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
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

    m_pPlayerUI = static_cast<CPlayerUI*>(m_pGameInstance->Get_UI(LEVEL_STATIC, CUI::UIID_PlayerHP));
    m_pPlayerUI->Set_PlayerMaxHP(m_fMAXHP);
    m_pPlayerUI->Set_PlayerHP(m_fMAXHP);
    return S_OK;
}

_int CPlayer::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

     m_pPlayerUI->Set_PlayerHP(m_fHP);

    if (m_bchange)
    {
        m_iWeaponType == CWeapon::HendGun ? m_Type = T00 : m_Type = T01;
        m_Type == T00 ? m_iState = STATE_SWITCHIN : m_iState = STATE_SWITCHIN2;
        m_bchange = false;
    }


    if (m_bHitLock == true)
    {
        m_Type == T00 ? m_iState = STATE_STUN : m_iState = STATE_STUN2;

    }
   
    Is_onDemageCell();
    

    if (false == m_bHitLock && false == m_bFallLock)
    {
        Key_Input(fTimeDelta);
    }
    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CPlayer::Update(_float fTimeDelta)
{
    if (false == m_bJump) {
        if (m_pNavigationCom->ISFall())
        {
            Set_onCell(false);
            if (XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)) >= -7.5f)
            {
                m_bFallLock = true;
                m_pTransformCom->Go_Down(fTimeDelta);
            }

            if (XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)) < -7.5f) {
                m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_PlayerWeaPon_Aim, CUI::UIID_InteractiveUI);
                m_DemageCellTime += fTimeDelta;
                m_bFallLock = false;
                HIt_Routine();
                if (m_DemageCellTime > 1.f) {
                    Set_onCell(true);
                 
                    m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, m_pNavigationCom->Get_SafePos());
                    m_pNavigationCom->Find_CurrentCell(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION));
                    m_DemageCellTime = 0.f;
                }
            }
        }
    }
    __super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
    if (false == m_bJump)
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
            return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
   __super::Render();
#endif
    return S_OK;
}

void CPlayer::HIt_Routine()
{

}

void CPlayer::Stun_Routine()
{
    m_bHitLock = true;
}

_float CPlayer::Weapon_Damage()
{
    return static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Damage();
}


const _float4x4* CPlayer::Get_CameraBone()
{
    return static_cast<CBody_Player*>(m_PartObjects[PART_BODY])->Get_SocketMatrix("Camera");
}

void CPlayer::Key_Input(_float fTimeDelta)
{

    if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT))
    {

        if (m_iState < STATE_HENDGUN_RELOAD)
        {
            if (m_iState != STATE_SWITCHIN && m_iState != STATE_SWITCHIN2)
            {
                m_Type == T00 ? m_iState = STATE_SPRINT : m_iState = STATE_SPRINT2;
                m_pTransformCom->Set_MoveSpeed(30.f);
            }
        }
    }
    else
    {
        m_pTransformCom->Set_MoveSpeed(15.f);

        if (m_iState < STATE_HENDGUN_RELOAD)
        {
            if (m_iState != STATE_SWITCHIN && m_iState != STATE_SWITCHIN2)
            {
                if (false == m_bJump)
                    m_Type == T00 ? m_iState = STATE_IDLE : m_iState = STATE_IDLE2;
            }
        }
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_W))
    {
        if (m_iState < STATE_HENDGUN_RELOAD)
        {
            if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
            {
                if (false == m_bJump)
                {
                    m_Type == T00 ? m_iState = STATE_RUN : m_iState = STATE_RUN2;
                }
            }
        }

        m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_S))
    {
        if (m_iState < STATE_HENDGUN_RELOAD)
        {
            if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
            {
                if (false == m_bJump)
                {
                    m_Type == T00 ? m_iState = STATE_RUN : m_iState = STATE_RUN2;
                }
            }
        }
        m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_A))
    {
        if (m_iState < STATE_HENDGUN_RELOAD)
        {
            if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
            {
                if (false == m_bJump)
                {
                    m_Type == T00 ? m_iState = STATE_RUN : m_iState = STATE_RUN2;
                }
            }
        }
        m_pTransformCom->Go_Left(fTimeDelta, m_pNavigationCom);
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_D))
    {
        if (m_iState < STATE_HENDGUN_RELOAD)
        {
            if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
            {
                if (false == m_bJump)
                {
                    m_Type == T00 ? m_iState = STATE_RUN : m_iState = STATE_RUN2;
                }
            }
        }
        m_pTransformCom->Go_Right(fTimeDelta, m_pNavigationCom);
    }

    if (m_pGameInstance->Get_DIKeyDown(DIK_TAB))
    {
        if (!m_bturn)
        {
            m_bturn = true;
        }
        else
            m_bturn = false;
    }

    if (true == m_bturn)
    {
        _float Y = XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_UP));

        m_pGameInstance->MouseFix();

        _long MouseMove = { 0 };

        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
        {
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.05f);
        }

        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
        {
            m_pTransformCom->Turn(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_RIGHT),
                fTimeDelta * MouseMove * 0.05f);
        }
    }

    if (m_pGameInstance->Get_DIKeyDown(DIK_R))
    {
        if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
        {

            if (CWeapon::HendGun == m_iWeaponType && T00 == m_Type)
                m_iState = STATE_HENDGUN_RELOAD;

            if (T01 == m_Type)
            {
                if (CWeapon::AssaultRifle == m_iWeaponType)
                    m_iState = STATE_ASSAULTRIFlLE_RELOAD;

                if (CWeapon::MissileGatling == m_iWeaponType)
                    m_iState = STATE_MissileGatling_RELOAD;

                if (CWeapon::HeavyCrossbow == m_iWeaponType)
                    m_iState = STATE_HEAVYCROSSBOW_RELOAD;
            }

        }
    }



    if (m_iState != STATE_HENDGUN_RELOAD && m_iState != STATE_ASSAULTRIFlLE_RELOAD &&
        m_iState != STATE_MissileGatling_RELOAD && m_iState != STATE_HEAVYCROSSBOW_RELOAD)
    {
        if (m_pGameInstance->Get_DIMouseState(DIM_LB))
        {

            if (CWeapon::HendGun == m_iWeaponType && T00 == m_Type)
                m_iState = STATE_HENDGUN_SHOOT;

            if (T01 == m_Type)
            {
                if (CWeapon::AssaultRifle == m_iWeaponType)
                    m_iState = STATE_ASSAULTRIFlLE_SHOOT;

                if (CWeapon::MissileGatling == m_iWeaponType)
                    m_iState = STATE_MissileGatling_SHOOT;

                if (CWeapon::HeavyCrossbow == m_iWeaponType)
                    m_iState = STATE_HEAVYCROSSBOW_SHOOT;
            }

        }
    }

    if (m_pGameInstance->Get_DIKeyDown(DIK_SPACE))
    {
        m_bJump = true;
        m_bOnCell = false;
        m_iJumpCount++;
    }

    if (m_bJump)
    {
        if (false == m_bDoubleJump) {
            m_Type == T00 ? m_iState = STATE_JUMP_RUN_LOOP : m_iState = STATE_JUMP_RUN_LOOP2;
        }
        m_pTransformCom->Go_jump(fTimeDelta, m_fY, &m_bJump);

        if (m_iJumpCount >= 2)
        {
            m_bDoubleJump = true;
        }
        if (true == m_bDoubleJump && m_bJump == true)
        {
            m_pTransformCom->Go_Doublejump(fTimeDelta);
            m_Type == T00 ? m_iState = STATE_JUMP_RUN_LOW : m_iState = STATE_JUMP_RUN_LOW2;
        }

        if (!m_bJump)
        {
            m_iJumpCount = 0;
            m_bDoubleJump = false;
            m_bOnCell = true;
            if (m_iState != STATE_SPRINT && m_iState != STATE_SPRINT2)
                m_Type == T00 ? m_iState = STATE_IDLE : m_iState = STATE_IDLE2;
        }
    }

}

void CPlayer::Choose_Weapon(const _uint& WeaponNum)
{
    m_iWeaponType = WeaponNum;
    m_bchange = true;
    static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Choose_Weapon(WeaponNum);
}

_uint CPlayer::Get_Bullet()
{
    return static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Get_Bullte();
}

_uint CPlayer::Get_MaxBullte()
{
    return static_cast<CWeapon*>(m_PartObjects[PART_WEAPON])->Get_MaxBullte();
}

void CPlayer::Set_HitLock(_bool Lock)
{
    m_bHitLock = Lock;
}


HRESULT CPlayer::Add_Components()
{ /* For.Com_Collider_AABB */
    CBounding_AABB::BOUND_AABB_DESC AABBDesc{};

    AABBDesc.vExtents = _float3(0.5f, 0.75f, 0.5f);
    AABBDesc.vCenter = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom),
        &AABBDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
       reinterpret_cast<CComponent**>(&m_pNavigationCom))))
       return E_FAIL;
    
    return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
    /* For.Body */
    CBody_Player::BODY_PLAYER_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pType = &m_Type;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_Player"), PART_BODY, &BodyDesc)))
        return E_FAIL;

    /* For.Weapon*/
    CWeapon::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.fSpeedPerSec = 0.f;
    WeaponDesc.fRotationPerSec = 0.f;
    WeaponDesc.pParentState = &m_iState;
    WeaponDesc.pSocketMatrix =
        static_cast<CBody_Player*>(m_PartObjects[PART_BODY])->Get_SocketMatrix("Weapon_Attachement");
    WeaponDesc.pType = &m_Type;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Weapon"), PART_WEAPON, &WeaponDesc)))
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
}

