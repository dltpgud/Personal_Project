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

 CActor::Actor_DESC* Desc = static_cast<CActor::Actor_DESC*>(pArg);
    Desc->iNumPartObjects = PART_END;
    Desc->fSpeedPerSec =  5.f;
    Desc->fRotationPerSec = XMConvertToRadians(90.f);
    Desc->JumpPower = 3.f;
    Desc->Object_Type = CGameObject::GAMEOBJ_TYPE::ACTOR;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;

   m_iState = ST_Idle;
   m_fMAXHP = 100.f;
   m_fHP = m_fMAXHP;
   m_bOnCell = true;

   if (FAILED(Add_Components()))
        return E_FAIL;

   if (FAILED(Add_PartObjects()))
        return E_FAIL;

   if (FAILED(m_pGameInstance->Add_Monster(this)))
        return E_FAIL;

    m_pPartHP = static_cast <CMonsterHP*>(m_PartObjects[PART_HP]);
    return S_OK;
}

void CBoomBot::Priority_Update(_float fTimeDelta)
{
    if (1.f == static_cast<CBody_BoomBot*>(m_PartObjects[PART_BODY])->Get_interver())
        m_bDead = true;

    if (m_iState != ST_Hit_Front)
        m_iRim = RIM_LIGHT_DESC::STATE_NORIM;

    if (m_pPartHP != nullptr) {
        m_pPartHP->Set_Monster_HP(m_fHP);
    }

    if (m_iState != ST_Hit_Front && m_iState != ST_Aim_Down)
        m_pTransformCom->Rotation_to_Player(fTimeDelta);

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
        m_pTransformCom->Go_Move(CTransform::BACK ,fTimeDelta, m_pNavigationCom);

    __super::Priority_Update(fTimeDelta);
    return ;
}

void CBoomBot::Update(_float fTimeDelta)
{
    
    __super::Update(fTimeDelta);
}

void CBoomBot::Late_Update(_float fTimeDelta)
{

   if(FAILED( m_pGameInstance->Add_Monster(this)))
       return ;
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

void CBoomBot::Dead_Routine(_float fTimeDelta)
{
    m_iState = ST_Aim_Down;

    if (m_pPartHP != nullptr) {
        Erase_PartObj(PART_HP);
        m_pPartHP = nullptr;
    }
}

void CBoomBot::NON_intersect(_float fTimedelta)
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION);

    _vector vDir = vPlayerPos - vPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));

    if (20.f > fLength && 11.f < fLength)
    {
        _float3 fDir{};
        XMStoreFloat3(&fDir, vDir);

        m_iState = ST_Run_Front;
        m_pTransformCom->Go_Move(CTransform::GO, fTimedelta, m_pNavigationCom);
    }
    if (11.f >= fLength )
        m_iState = ST_Shoot;
    if (10.f > fLength)
    {
        m_iState = ST_Idle;
    }
    if (9.f > fLength)
    {
        m_iState = ST_Run_Back;

        m_pTransformCom->Go_Move(CTransform::BACK ,fTimedelta, m_pNavigationCom);
    }
    if (20.f <fLength)
    m_iState = ST_Idle;

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
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_BoomBot"), PART_BODY, &BodyDesc)))
        return E_FAIL;


    CMonsterHP::CMonsterHP_DESC HpDesc{};
    HpDesc.fMaxHP = m_fMAXHP;
    HpDesc.fHP = m_fHP;
    HpDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
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
