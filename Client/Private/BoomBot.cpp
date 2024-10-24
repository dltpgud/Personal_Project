#include "stdafx.h"
#include "BoomBot.h"
#include "GameInstance.h"
#include "Body_BoomBot.h"
#include "Weapon.h"
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

   

    m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, Desc->POSITION);


    m_fMAXHP = 100.f;
    m_fHP = m_fMAXHP;
    return S_OK;
}

_int CBoomBot::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

   // if (m_iState != ST_Hit_Front && m_iState != ST_Sragger)
        m_pTransformCom->Rotation_to_Player();

    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CBoomBot::Update(_float fTimeDelta)
{
   // if (static_cast<CBody_BoomBot*>(m_PartObjects[PART_BODY])->Get_Finish())
   
//    if(m_iState != ST_Hit_Front && m_iState != ST_Sragger)
 //       NON_intersect(fTimeDelta);

  
//    if (m_iState == ST_Hit_Front) {
 //       if (XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)) >= 0.f) {
   //         m_pTransformCom->Set_MoveSpeed(10.f);
  //          m_pTransformCom->Go_Down(fTimeDelta);
   //     }
   /// }

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
  
}

void CBoomBot::Dead_Routine()
{
    
}

void CBoomBot::NON_intersect(_float fTimedelta)
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

          
            m_pTransformCom->Go_Straight(fTimedelta);
        }

        if (25.f > fLength)
        {
          
           
            if (15.f > fLength)
            {
     
                m_pTransformCom->Go_Backward(fTimedelta);
            }
        }
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

  if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_BoomBot"), PART_BODY, &BodyDesc)))
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
