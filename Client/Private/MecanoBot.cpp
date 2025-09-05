#include "stdafx.h"
#include "MecanoBot.h"
#include "GameInstance.h"
#include "Body_MecanoBot.h"
#include "Weapon.h"
#include "MonsterHP.h"
#include "Player.h"
CMecanoBot::CMecanoBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster{pDevice, pContext}
{
}

CMecanoBot::CMecanoBot(const CMecanoBot& Prototype) : CMonster{Prototype}
{
}

HRESULT CMecanoBot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMecanoBot::Initialize(void* pArg)
{
    CActor::Actor_DESC* Desc = static_cast<CActor::Actor_DESC*>(pArg);
    Desc->fSpeedPerSec =  7.f;
    Desc->fRotationPerSec = XMConvertToRadians(90.f);
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

void CMecanoBot::Priority_Update(_float fTimeDelta)
{
    Compute_Length();

    __super::Priority_Update(fTimeDelta);
}

void CMecanoBot::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMecanoBot::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CMecanoBot::Render()
{
    __super::Render();
    return S_OK; 
}

void CMecanoBot::HIt_Routine()
{
    Compute_Angle();
    static_cast<CBody_MecanoBot*>(m_PartObjects[PART_BODY])->ChangeState(ST_HIT);
    m_iHP -= static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_Weapon_Info().Damage;
    static_cast<CMonsterHP*>(m_PartObjects[PART_HP])->Set_Monster_HP(m_iHP);
    static_cast<CMonsterHP*>(m_PartObjects[PART_HP])->Set_HitStart(true);
}

void CMecanoBot::Dead_Routine()
{
    static_cast<CBody_MecanoBot*>(m_PartObjects[PART_BODY])->ChangeState(ST_DEAD);
    Erase_PartObj(PART_HP); 
}

void CMecanoBot::Set_State(_uint State)
{
    static_cast<CBody_MecanoBot*>(m_PartObjects[PART_BODY])->ChangeState(State);
}

HRESULT CMecanoBot::Add_Components()
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

HRESULT CMecanoBot::Add_PartObjects()
{
    CBody_MecanoBot::CBody_MecanoBot_Desc BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pRimState = &m_iRim;
    BodyDesc.pParentObj = this;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_MecanoBot"), PART_BODY, &BodyDesc)))
        return E_FAIL;

    CMonsterHP::CMonsterHP_DESC HpDesc{};
    HpDesc.iMaxHP = m_iMAXHP;
    HpDesc.iHP = m_iHP;
    HpDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_MonsterHP"), PART_HP, &HpDesc)))
        return E_FAIL;

    return S_OK;
}

CMecanoBot* CMecanoBot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMecanoBot* pInstance = new CMecanoBot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMecanoBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMecanoBot::Clone(void* pArg)
{
    CMecanoBot* pInstance = new CMecanoBot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CMecanoBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMecanoBot::Free()
{
    __super::Free();
}
