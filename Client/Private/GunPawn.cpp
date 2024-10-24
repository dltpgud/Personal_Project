#include "stdafx.h"
#include "GunPawn.h"
#include "GameInstance.h"
#include "Body_GunPawn.h"
#include "Weapon.h"
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

    CActor::Actor_DESC* Desc = static_cast<Actor_DESC*>(pArg);
    Desc->iNumPartObjects = PART_END;
    Desc->fSpeedPerSec = 3.f;
    Desc->fRotationPerSec = XMConvertToRadians(60.f);
    Desc->JumpPower = 3.f;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
                if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    m_iState = ST_IDLE;

    m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, Desc->POSITION);


    m_fMAXHP = 100.f;
    m_fHP = m_fMAXHP;
    return S_OK;
}

_int CGunPawn::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    if (m_iState != ST_PRESHOOT && m_iState != ST_STUN_START)
       m_pTransformCom->Rotation_to_Player();
    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CGunPawn::Update(_float fTimeDelta)
{
    if (static_cast <CBody_GunPawn*>(m_PartObjects[PART_BODY])->Get_Finish())
        m_iState = ST_IDLE;

    if(m_iState != ST_PRESHOOT && m_iState != ST_STUN_START)
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

}

void CGunPawn::Dead_Routine()
{
    m_iState = ST_PRESHOOT;
}

void CGunPawn::NON_intersect(_float fTimedelta)
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPlayerPos - vPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));
    if (50.f > fLength)
    {
        if (20.f < fLength) {

            _float3 fDir{};
            XMStoreFloat3(&fDir, vDir);

            m_iState = ST_RUN_LEFT;
        }
        if (m_iState == ST_RUN_BACK_FRONT)
        {
            m_pTransformCom->Go_Straight(fTimedelta);
        }
        if (m_iState == ST_RUN_BACK)
        {
            m_pTransformCom->Go_Backward(fTimedelta);
        }
        if (m_iState == ST_RUN_LEFT)
        {
            m_pTransformCom->Go_Left(fTimedelta);
        }
        if (m_iState == ST_RUN_RIGHT)
        {


            m_pTransformCom->Go_Right(fTimedelta);
        }

        if (20.f > fLength)
        {
            m_iState = ST_GRENADE_PRESHOOT;


            if (12.f > fLength)
                m_iState = ST_RUN_BACK;
        }
    }
    else
        m_iState = ST_IDLE;
}

HRESULT CGunPawn::Add_Components()
{    /* For.Com_Collider_AABB */
    CBounding_OBB::BOUND_OBB_DESC		OBBDesc{};


    _float3 Center{}, Extents{};
    OBBDesc.vExtents = _float3(1.f, 1.3f, 0.5f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);
    OBBDesc.vRotation = { 0.f,0.f,0.f };
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
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

  if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_GunPawn"), PART_BODY, &BodyDesc)))
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
