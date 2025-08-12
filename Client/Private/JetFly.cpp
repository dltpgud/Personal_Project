#include "stdafx.h"
#include "JetFly.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "MonsterHP.h"
#include "Body_JetFly.h"
CJetFly::CJetFly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster{pDevice, pContext}
{
}

CJetFly::CJetFly(const CJetFly& Prototype) : CMonster{Prototype}
{
}

HRESULT CJetFly::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CJetFly::Initialize(void* pArg)
{
    CActor::Actor_DESC* Desc = static_cast<CActor::Actor_DESC*>(pArg);
    Desc->fSpeedPerSec = 5.f;
    Desc->fRotationPerSec = XMConvertToRadians(90.f);
    Desc->JumpPower = 3.f;
    if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;

    m_fMAXHP = 100.f;       
    m_fHP = m_fMAXHP;
    m_bOnCell = { true };
    m_FixY = 5.f;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    m_iState = ST_MOVE;

    return S_OK;
}

void CJetFly::Priority_Update(_float fTimeDelta)
{
    Compute_Length();
    if (m_iState != ST_HIT)
        m_iRim = RIM_LIGHT_DESC::STATE_NORIM;

    
        __super::Priority_Update(fTimeDelta);
    return ;
}

void CJetFly::Update(_float fTimeDelta)
{
        __super::Update(fTimeDelta);
}

void CJetFly::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CJetFly::Render()
{
    __super::Render();
    return S_OK; 
}

void CJetFly::HIt_Routine()
{
     Compute_Angle();
     static_cast<CBody_JetFly*>(m_PartObjects[PART_BODY])->ChangeState(ST_HIT);
     m_iRim = RIM_LIGHT_DESC::STATE_RIM;
     static_cast<CMonsterHP*>(m_PartObjects[PART_HP])->Set_Monster_HP(m_fHP);
     static_cast<CMonsterHP*>(m_PartObjects[PART_HP])->Set_HitStart(true);
}

void CJetFly::Dead_Routine()
{
    static_cast<CBody_JetFly*>(m_PartObjects[PART_BODY])->ChangeState(ST_DEAD);
  
    Erase_PartObj(PART_HP);
}

HRESULT CJetFly::Add_Components()
{ 
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

HRESULT CJetFly::Add_PartObjects()
{
    CBody_JetFly::CBody_JetFly_Desc BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pRimState = &m_iRim;
    BodyDesc.pParentObj = this;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_JetFly"), PART_BODY, &BodyDesc)))
        return E_FAIL;

    CMonsterHP::CMonsterHP_DESC HpDesc{};
    HpDesc.fMaxHP = m_fMAXHP;
    HpDesc.fHP = m_fHP;
    HpDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_MonsterHP"), PART_HP, &HpDesc)))
      return E_FAIL;

    return S_OK;
}

CJetFly* CJetFly::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CJetFly* pInstance = new CJetFly(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CJetFly");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CJetFly::Clone(void* pArg)
{
    CJetFly* pInstance = new CJetFly(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CJetFly");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CJetFly::Free()
{
    __super::Free();
}
