#include "stdafx.h"
#include "BoomBot.h"
#include "GameInstance.h"
#include "Body_BoomBot.h"
#include "Weapon.h"
#include "MonsterHP.h"
#include "Player.h"
CBoomBot::CBoomBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster{pDevice, pContext}
{
}

CBoomBot::CBoomBot(const CBoomBot& Prototype) : CMonster{Prototype}
{
}

HRESULT CBoomBot::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CBoomBot::Initialize(void* pArg)
{
    CActor::Actor_DESC* Desc = static_cast<CActor::Actor_DESC*>(pArg);
    Desc->fSpeedPerSec = 9.f;
    Desc->fRotationPerSec = XMConvertToRadians(120.f);
    Desc->JumpPower = 3.f;
    Desc->iHP = 100;
    Desc->bOnCell = true;
    Desc->iState = ST_IDLE;
    Desc->fFixY = 0.f;
   if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;

   if (FAILED(Add_Components()))
        return E_FAIL;

   if (FAILED(Add_PartObjects()))
        return E_FAIL;

    return S_OK;
}

void CBoomBot::Priority_Update(_float fTimeDelta)
{
    Compute_Length();
 
    __super::Priority_Update(fTimeDelta);
    return ;
}

void CBoomBot::Update(_float fTimeDelta)
{
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
   Compute_Angle();
   static_cast<CBody_BoomBot*>(m_PartObjects[PART_BODY])->ChangeState(ST_HIT);
   m_iHP -= static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_Weapon_Info().Damage;
   static_cast <CMonsterHP*>(m_PartObjects[PART_HP])->Set_Monster_HP(m_iHP);
   static_cast <CMonsterHP*>(m_PartObjects[PART_HP])->Set_HitStart(true);
}

void CBoomBot::Dead_Routine()
{
    static_cast<CBody_BoomBot*>(m_PartObjects[PART_BODY])->ChangeState(ST_DEAD);
   
    Erase_PartObj(PART_HP);
}

HRESULT CBoomBot::Add_Components()
{  
    CBounding_Sphere::BOUND_SPHERE_DESC		CBounding_Sphere{};

    _float3 Center{}, Extents{};
    CBounding_Sphere.fRadius = 1.f;
    CBounding_Sphere.vCenter = _float3(0.f, 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CBounding_Sphere)))
        return E_FAIL;

    CNavigation::NAVIGATION_DESC		Desc{};

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

void CBoomBot::Set_State(_uint State)
{
    static_cast<CBody_BoomBot*>(m_PartObjects[PART_BODY])->ChangeState(State);
}

HRESULT CBoomBot::Add_PartObjects()
{ 
    CBody_BoomBot::CBody_BoomBot_Desc BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pParentObj = this;
    BodyDesc.pRimState = &m_iRim;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_BoomBot"), PART_BODY, &BodyDesc)))
        return E_FAIL;

    CMonsterHP::CMonsterHP_DESC HpDesc{};
    HpDesc.iMaxHP = m_iMAXHP;
    HpDesc.iHP = m_iHP;
    HpDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_MonsterHP"), PART_HP, &HpDesc)))
       return E_FAIL;

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
