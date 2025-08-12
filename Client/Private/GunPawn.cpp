#include "stdafx.h"
#include "GunPawn.h"
#include "GameInstance.h"
#include "Body_GunPawn.h"
#include "Weapon.h"
#include "MonsterHP.h"
CGunPawn::CGunPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CMonster{pDevice, pContext}
{
}

CGunPawn::CGunPawn(const CGunPawn& Prototype) : CMonster{Prototype}
{
}

HRESULT CGunPawn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGunPawn::Initialize(void* pArg)
{
    CActor::Actor_DESC* Desc = static_cast<CActor::Actor_DESC*>(pArg);
    Desc->fSpeedPerSec = 10.f;
    Desc->fRotationPerSec = XMConvertToRadians(90.f);
    Desc->JumpPower = 3.f;
    if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;

    m_fMAXHP = 100.f;
    m_fHP = m_fMAXHP;
    m_bOnCell = true;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    m_iState = ST_MOVE;

    static_cast<CMonsterHP*>(m_PartObjects[PART_HP])
    ->Get_Transform()->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(0.f, 3.f, 0.f, 1.f));

    return S_OK;
}

void CGunPawn::Priority_Update(_float fTimeDelta)
{
    Compute_Length();

    if (m_iState != ST_HIT)
        m_iRim = RIM_LIGHT_DESC::STATE_NORIM;

    __super::Priority_Update(fTimeDelta);
    return ;
}

void CGunPawn::Update(_float fTimeDelta)
{
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
    Compute_Angle();
    static_cast<CBody_GunPawn*>(m_PartObjects[PART_BODY])->ChangeState(ST_HIT);
    m_iRim = RIM_LIGHT_DESC::STATE_RIM;
    static_cast<CMonsterHP*>(m_PartObjects[PART_HP])->Set_Monster_HP(m_fHP);
    static_cast<CMonsterHP*>(m_PartObjects[PART_HP])->Set_HitStart(true);
}

void CGunPawn::Dead_Routine()
{
    static_cast<CBody_GunPawn*>(m_PartObjects[PART_BODY])->ChangeState(ST_DEAD);
 
    Erase_PartObj(PART_HP);
}

HRESULT CGunPawn::Add_Components()
{
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
    CBody_GunPawn::BODY_GUNPAWN_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pRimState = &m_iRim;
    BodyDesc.pParentObj = this;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_GunPawn"), PART_BODY, &BodyDesc)))
        return E_FAIL;

    CMonsterHP::CMonsterHP_DESC HpDesc{};
    HpDesc.fMaxHP = m_fMAXHP;
    HpDesc.fHP = m_fHP;
    HpDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_MonsterHP"), PART_HP, &HpDesc)))
        return E_FAIL;

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
