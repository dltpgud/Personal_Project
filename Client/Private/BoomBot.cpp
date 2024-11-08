#include "stdafx.h"
#include "BoomBot.h"
#include "GameInstance.h"
#include "Body_BoomBot.h"
#include "Weapon.h"
#include "MonsterHP.h"
CBoomBot::CBoomBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CActor{pDevice, pContext}
{
}

CBoomBot::CBoomBot(const CBoomBot& Prototype) : CActor{Prototype}
{
}

HRESULT CBoomBot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoomBot::Initialize(void* pArg)
{

    CActor::Actor_DESC Desc{};
    Desc.iNumPartObjects = PART_END;
    Desc.fSpeedPerSec =  5.f;
    Desc.fRotationPerSec = XMConvertToRadians(60.f);
    Desc.JumpPower = 3.f;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

   m_iState = ST_Idle;
   m_fMAXHP = 100.f;
   m_fHP = m_fMAXHP;
   m_bOnCell = true;

   m_DATA_TYPE = CGameObject::DATA_MONSTER;
    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

 

    m_pPartHP = static_cast <CMonsterHP*>(m_PartObjects[PART_HP]);
    return S_OK;
}

_int CBoomBot::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;


    if (m_iState != ST_Hit_Front)
        m_iRim = RIM_LIGHT_DESC::STATE_NORIM;

    if (m_pPartHP != nullptr) {
        m_pPartHP->Set_Monster_HP(m_fHP);
        m_pTransformCom->Other_set_Pos(m_pPartHP->Get_Transform(), CTransform::FIX_Y, 2.f);
    }


    if (m_iState != ST_Hit_Front && m_iState != ST_Aim_Down)
        m_pTransformCom->Rotation_to_Player();

    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CBoomBot::Update(_float fTimeDelta)
{
    if (m_PartObjects[PART_BODY]->Get_Finish())
    {
        if (m_pPartHP != nullptr)
            m_pPartHP->Set_Hit(false);

            m_iState = ST_Idle;
    }


    if (2 != m_pNavigationCom->Get_CurrentCell_Type())
    {
        if (m_iState != ST_Hit_Front && m_iState != ST_Aim_Down)
            NON_intersect(fTimeDelta);
    }
    else
        m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);
    __super::Update(fTimeDelta);
}

void CBoomBot::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CBoomBot::Render()
{
    __super::Render();
    return S_OK; 
}

void CBoomBot::HIt_Routine()
{
    m_iState = ST_Hit_Front;

    m_iRim = RIM_LIGHT_DESC::STATE_RIM;
    m_pPartHP->Set_HitStart(true);
    m_pPartHP->Set_Hit(true);
    m_pPartHP->Set_bLateUpdaet(true);
}

void CBoomBot::Dead_Routine()
{
    m_iState = ST_Aim_Down;

    if (m_pPartHP != nullptr) {
        Erase_PartObj(PART_HP);
        m_pPartHP = nullptr;
    }
}

void CBoomBot::NON_intersect(_float fTimedelta)
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPlayerPos - vPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));

   

    if (20.f > fLength && 10.f < fLength)
    {
        _float3 fDir{};
        XMStoreFloat3(&fDir, vDir);

        m_iState = ST_Run_Front;
        m_pTransformCom->Go_Straight(fTimedelta, m_pNavigationCom);  
    }
    if (10.f >= fLength)
        m_iState = ST_Idle;
    if (8.f > fLength)
    {
        m_iState = ST_Shoot;
        m_pTransformCom->Go_Backward(fTimedelta, m_pNavigationCom);
    }
    else {
            m_iState = ST_Idle;
    }

}

HRESULT CBoomBot::Add_Components()
{    /* For.Com_Collider_AABB */
    CBounding_Sphere::BOUND_SPHERE_DESC		CBounding_Sphere{};


    _float3 Center{}, Extents{};
    CBounding_Sphere.fRadius = 1.f;
    CBounding_Sphere.vCenter = _float3(0.f, 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CBounding_Sphere)))
        return E_FAIL;


    CNavigation::NAVIGATION_DESC		Desc{};

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoomBot::Add_PartObjects()
{
    /* For.Body */
    CBody_BoomBot::CBody_BoomBot_Desc BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pRimState = &m_iRim;
    BodyDesc.Fall_Y = &m_fY;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_BoomBot"), PART_BODY, &BodyDesc)))
        return E_FAIL;


    CMonsterHP::CMonsterHP_DESC HpDesc{};
    HpDesc.fMaxHP = m_fMAXHP;
    HpDesc.fHP = m_fHP;

  if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_MonsterHP"), PART_HP, &HpDesc)))
      return E_FAIL;


    return S_OK;
}

HRESULT CBoomBot::Bind_ShaderResources()
{
    return S_OK;
}

CBoomBot* CBoomBot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBoomBot* pInstance = new CBoomBot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBoomBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoomBot::Clone(void* pArg)
{
    CBoomBot* pInstance = new CBoomBot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBoomBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoomBot::Free()
{
    __super::Free();

}
