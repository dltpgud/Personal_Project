#include "stdafx.h"
#include "JetFly.h"
#include "GameInstance.h"
#include "Body_GunPawn.h"
#include "Weapon.h"
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

    CActor::Actor_DESC* Desc = static_cast<Actor_DESC*>(pArg);
    Desc->iNumPartObjects = PART_END;
    Desc->fSpeedPerSec = 5.f;
    Desc->fRotationPerSec = XMConvertToRadians(60.f);
    Desc->JumpPower = 3.f;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
                if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    m_iState = ST_Idle;

    m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, Desc->POSITION);


    m_fMAXHP = 100.f;
    m_fHP = m_fMAXHP;
    return S_OK;
}

_int CJetFly::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    if (m_iState != ST_Hit_Front && m_iState != ST_Sragger)
        m_pTransformCom->Rotation_to_Player();

    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CJetFly::Update(_float fTimeDelta)
{
    if (m_PartObjects[PART_BODY]->Get_Finish())
        m_iState = ST_Idle;

    if(m_iState != ST_Hit_Front && m_iState != ST_Sragger)
        NON_intersect(fTimeDelta);

  
    if (m_iState == ST_Hit_Front) {
        if (XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)) >= 0.f) {
            m_pTransformCom->Set_MoveSpeed(10.f);
            m_pTransformCom->Go_Down(fTimeDelta);
        }
    }

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
   
    m_iState = ST_Sragger;

}

void CJetFly::Dead_Routine()
{
    m_iState = ST_Hit_Front;
}

void CJetFly::NON_intersect(_float fTimedelta)
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPlayerPos - vPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));
    if (40.f > fLength)
    {
        if (25.f < fLength) {
            
            _float3 fDir{};
            XMStoreFloat3(&fDir, vDir);

                m_iState = ST_Walk_Front;
            m_pTransformCom->Go_Straight(fTimedelta);
        }

        if (25.f > fLength)
        {
            m_iState = ST_Shoot;
           
            if (15.f > fLength)
            {
                m_iState = ST_Walk_Back;
                m_pTransformCom->Go_Backward(fTimedelta);
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

  if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_JetFly"), PART_BODY, &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CJetFly::Bind_ShaderResources()
{
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
