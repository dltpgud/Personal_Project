#include "stdafx.h"
#include "BillyBoom.h"
#include "GameInstance.h"
#include "Body_BillyBoom.h"
#include "BossIntroBG.h"

CBillyBoom::CBillyBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CActor{pDevice, pContext}
{
}

CBillyBoom::CBillyBoom(const CBillyBoom& Prototype) : CActor{Prototype}
{
}

HRESULT CBillyBoom::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBillyBoom::Initialize(void* pArg)
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
   m_FixY = 1.f;
   m_DATA_TYPE = CGameObject::DATA_MONSTER;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;


    return S_OK;
}

_int CBillyBoom::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

   // if (m_iState != ST_Stagger_Front)
   //     m_iRim = RIM_LIGHT_DESC::STATE_NORIM;

    if (m_PartObjects[PART_BODY]->Get_Finish())
         {
                  m_iState = ST_Idle;
         }

    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CBillyBoom::Update(_float fTimeDelta)
{

  //  if (m_PartObjects[PART_BODY]->Get_Finish())
  //  {
  //          m_iState = ST_Idle;
  //  }

    if (2 != m_pNavigationCom->Get_CurrentCell_Type())
    {
    //    if (m_iState != ST_Stagger_Front && m_iState != ST_Stun_Start) {
            m_pTransformCom->Rotation_to_Player();
      //      NON_intersect(fTimeDelta);
    //    }
    }
    else
        m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);

    __super::Update(fTimeDelta);
}

void CBillyBoom::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);
}

HRESULT CBillyBoom::Render()
{


    __super::Render();
    return S_OK; 
}

void CBillyBoom::HIt_Routine()
{

    m_iRim = RIM_LIGHT_DESC::STATE_RIM;

}

void CBillyBoom::Dead_Routine()
{
    m_iState = ST_Stun_Start;


}

void CBillyBoom::NON_intersect(_float fTimedelta)
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPlayerPos - vPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));


}

void CBillyBoom::Intro_Routine(_float fTimedelta)
{

    if (true == m_bIntro)
    {
        m_iState = ST_Intro;

       _vector vWorldPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
        CBossIntroBG::CBossIntroBG_DESC IntroDesc{};
        IntroDesc.fX = XMVectorGetX(vWorldPos);
        IntroDesc.fY = XMVectorGetY(vWorldPos);
        IntroDesc.fZ = XMVectorGetZ(vWorldPos);
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, CGameObject::MAP, L"Prototype_GameObject_Boss_InstroUI",
            nullptr, 0, &IntroDesc)))
            return;

        m_bIntro = false;
    }

}  

HRESULT CBillyBoom::Add_Components()
{    /* For.Com_Collider_AABB */
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};

    _float3 Center{}, Extents{};
    OBBDesc.vExtents = _float3(3.f, 3.3f, 3.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);
    OBBDesc.vRotation = { 0.f, 0.f, 0.f };
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider_OBB"),
        reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;


    CNavigation::NAVIGATION_DESC		Desc{};
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBillyBoom::Add_PartObjects()
{
    /* For.Body */
    CBody_BillyBoom::CBody_BillyBoom_Desc BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pRimState = &m_iRim;
 
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_BillyBoom"), PART_BODY, &BodyDesc)))
        return E_FAIL;


 
    return S_OK;
}

HRESULT CBillyBoom::Bind_ShaderResources()
{
    return S_OK;
}

CBillyBoom* CBillyBoom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBillyBoom* pInstance = new CBillyBoom(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBillyBoom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBillyBoom::Clone(void* pArg)
{
    CBillyBoom* pInstance = new CBillyBoom(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBillyBoom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBillyBoom::Free()
{
    __super::Free();

}
