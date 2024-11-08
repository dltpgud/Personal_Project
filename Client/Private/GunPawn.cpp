#include "stdafx.h"
#include "GunPawn.h"
#include "GameInstance.h"
#include "Body_GunPawn.h"
#include "Weapon.h"
#include "MonsterHP.h"
CGunPawn::CGunPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CActor{pDevice, pContext}
{
}

CGunPawn::CGunPawn(const CGunPawn& Prototype) : CActor{Prototype}
{
}

HRESULT CGunPawn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGunPawn::Initialize(void* pArg)
{

    CActor::Actor_DESC Desc{};
    Desc.iNumPartObjects = PART_END;
    Desc.fSpeedPerSec = 3.f;
    Desc.fRotationPerSec = XMConvertToRadians(60.f);
    Desc.JumpPower = 3.f;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fMAXHP = 100.f;
    m_fHP = m_fMAXHP;
    m_bOnCell = true;
    m_DATA_TYPE = CGameObject::DATA_MONSTER;
    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    m_iState = ST_IDLE;

    m_pPartHP = static_cast<CMonsterHP*>(m_PartObjects[PART_HP]);

    return S_OK;
}

_int CGunPawn::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    if (m_iState != ST_STUN_START)
        m_iRim = RIM_LIGHT_DESC::STATE_NORIM;

    if (m_pPartHP != nullptr)
    {
        m_pPartHP->Set_Monster_HP(m_fHP);
        m_pTransformCom->Other_set_Pos(m_pPartHP->Get_Transform(), CTransform::FIX_Y, 3.f);
    }

    if (m_iState != ST_PRESHOOT && m_iState != ST_STUN_START)
        m_pTransformCom->Rotation_to_Player();

    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CGunPawn::Update(_float fTimeDelta)
{
    if (m_PartObjects[PART_BODY]->Get_Finish())
    {
        if (m_pPartHP != nullptr)
            m_pPartHP->Set_Hit(false);

        m_iState = ST_IDLE;
    }

    if (m_iState != ST_PRESHOOT && m_iState != ST_STUN_START)
        NON_intersect(fTimeDelta);

    __super::Update(fTimeDelta);
}

void CGunPawn::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CGunPawn::Render()
{
    __super::Render();
    return S_OK;
}

void CGunPawn::HIt_Routine()
{

    m_iState = ST_STUN_START;

    m_iRim = RIM_LIGHT_DESC::STATE_RIM;
    m_pPartHP->Set_HitStart(true);
    m_pPartHP->Set_Hit(true);
    m_pPartHP->Set_bLateUpdaet(true);
}

void CGunPawn::Dead_Routine()
{
    m_iState = ST_PRESHOOT;

    if (m_pPartHP != nullptr)
    {
        Erase_PartObj(PART_HP);
        m_pPartHP = nullptr;
    }
}

void CGunPawn::NON_intersect(_float fTimedelta)
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPlayerPos - vPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));


    if (30.f > fLength)
    {
        if (20.f < fLength)
        {
            m_iState = ST_RUN_LEFT;
        }

        if (m_iState == ST_RUN_BACK_FRONT)
        {
            m_pTransformCom->Go_Straight(fTimedelta, m_pNavigationCom);
        }

        if (m_iState == ST_RUN_BACK)
        {
            m_pTransformCom->Go_Backward(fTimedelta, m_pNavigationCom);
        }
        if (m_iState == ST_RUN_LEFT)
        {
            m_pTransformCom->Go_Left(fTimedelta, m_pNavigationCom);
        }

        if (m_iState == ST_RUN_RIGHT)
        {
            m_pTransformCom->Go_Right(fTimedelta, m_pNavigationCom);
        }

        if (15.f > fLength)
        {

           if (m_iState != ST_RUN_RIGHT)
                m_iState = ST_GRENADE_PRESHOOT;
            if (true == static_cast<CBody_GunPawn*>(m_PartObjects[PART_BODY])->Get_bRun())
            {
                m_iState = ST_RUN_RIGHT;
            }

            if (7.f > fLength)
            {
                m_iState = ST_RUN_BACK;
            }
        }
    }
    else
        m_iState = ST_IDLE;
}

HRESULT CGunPawn::Add_Components()
{ /* For.Com_Collider_AABB */
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};

    _float3 Center{}, Extents{};
    OBBDesc.vExtents = _float3(1.f, 1.3f, 0.5f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);
    OBBDesc.vRotation = {0.f, 0.f, 0.f};
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider_OBB"),
                                      reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    CNavigation::NAVIGATION_DESC Desc{};

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
                                      reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGunPawn::Add_PartObjects()
{
    /* For.Body */
    CBody_GunPawn::BODY_GUNPAWN_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pRimState = &m_iRim;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_GunPawn"), PART_BODY, &BodyDesc)))
        return E_FAIL;

    CMonsterHP::CMonsterHP_DESC HpDesc{};
    HpDesc.fMaxHP = m_fMAXHP;
    HpDesc.fHP = m_fHP;

    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_MonsterHP"), PART_HP, &HpDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGunPawn::Bind_ShaderResources()
{
    return S_OK;
}

CGunPawn* CGunPawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGunPawn* pInstance = new CGunPawn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGunPawn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGunPawn::Clone(void* pArg)
{
    CGunPawn* pInstance = new CGunPawn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CGunPawn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGunPawn::Free()
{
    __super::Free();
}
