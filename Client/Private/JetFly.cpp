#include "stdafx.h"
#include "JetFly.h"
#include "GameInstance.h"
#include "Body_GunPawn.h"
#include "Weapon.h"
#include "MonsterHP.h"
CJetFly::CJetFly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CActor{pDevice, pContext}
{
}

CJetFly::CJetFly(const CJetFly& Prototype) : CActor{Prototype}
{
}

HRESULT CJetFly::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CJetFly::Initialize(void* pArg)
{

    CActor::Actor_DESC Desc{};
    Desc.iNumPartObjects = PART_END;
    Desc.fSpeedPerSec = 5.f;
    Desc.fRotationPerSec = XMConvertToRadians(60.f);
    Desc.JumpPower = 3.f;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
    m_fMAXHP = 100.f;       
    m_fHP = m_fMAXHP;
    m_bOnCell = { false };

    m_DATA_TYPE = CGameObject::DATA_MONSTER;
    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    m_iState = ST_Idle;

    m_pPartHP = static_cast <CMonsterHP*>(m_PartObjects[PART_HP]);
    return S_OK;
}

_int CJetFly::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    if (m_iState != ST_Sragger)
        m_iRim = RIM_LIGHT_DESC::STATE_NORIM;

    if (m_pPartHP != nullptr ) {
        m_pPartHP->Set_Monster_HP(m_fHP);
        m_pTransformCom->Other_set_Pos(m_pPartHP->Get_Transform(), CTransform::FIX_Y, 2.f);
    }


    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CJetFly::Update(_float fTimeDelta)
{
    if (m_PartObjects[PART_BODY]->Get_Finish()) {
        if (m_pPartHP != nullptr)
            m_pPartHP->Set_Hit(false);
        m_iState = ST_Idle;
    }
    if (2 != m_pNavigationCom->Get_CurrentCell_Type())
    {
        if (m_iState != ST_Hit_Front && m_iState != ST_Sragger) {
            m_pTransformCom->Rotation_to_Player(fTimeDelta);
            NON_intersect(fTimeDelta);
        }
    }
    if (m_iState == ST_Hit_Front) {

        _float3 fPos{};
         Height_On_Cell(&fPos);

        if (XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)) >= m_fY) {
            m_pTransformCom->Set_MoveSpeed(10.f);
            m_pTransformCom->Go_Down(fTimeDelta, m_pNavigationCom);
        }
    }

        __super::Update(fTimeDelta);
}

void CJetFly::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_Monster(this)))
        return;
    __super::Late_Update(fTimeDelta);
}

HRESULT CJetFly::Render()
{
    __super::Render();
    return S_OK; 
}

void CJetFly::HIt_Routine()
{
   
    m_iState = ST_Sragger;

    m_iRim = RIM_LIGHT_DESC::STATE_RIM;
    
    m_pPartHP->Set_HitStart(true);
    m_pPartHP->Set_Hit(true);
    m_pPartHP->Set_bLateUpdaet(true);
}

void CJetFly::Dead_Routine()
{
    m_iState = ST_Hit_Front;

    if (m_pPartHP != nullptr) {
        Erase_PartObj(PART_HP);
        m_pPartHP = nullptr;
    }
}



void CJetFly::NON_intersect(_float fTimedelta)
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPlayerPos - vPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));
    if (40.f > fLength)
    {
        if (20.f < fLength) {
            
            _float3 fDir{};
             XMStoreFloat3(&fDir, vDir);

                m_iState = ST_Walk_Front;
            m_pTransformCom->Go_Straight(fTimedelta, m_pNavigationCom);
        }

        if (20.f >= fLength)
        {
            m_iState = ST_Shoot;
           
            if (15.f > fLength)
            {
                m_iState = ST_Walk_Back;
                m_pTransformCom->Go_Backward(fTimedelta, m_pNavigationCom);
            }
        }
    }
    else
        m_iState = ST_Idle;
}

HRESULT CJetFly::Add_Components()
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

HRESULT CJetFly::Add_PartObjects()
{
    /* For.Body */
    CBody_GunPawn::BODY_GUNPAWN_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pRimState = &m_iRim;
  if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_JetFly"), PART_BODY, &BodyDesc)))
        return E_FAIL;

  CMonsterHP::CMonsterHP_DESC HpDesc{};
  HpDesc.fMaxHP = m_fMAXHP;
  HpDesc.fHP = m_fHP;

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
