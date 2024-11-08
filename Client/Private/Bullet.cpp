#include "stdafx.h"
#include "Bullet.h"
#include "GameInstance.h"

CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CBullet::CBullet(const CBullet& Prototype) : CGameObject{Prototype}
{
}

HRESULT CBullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
    if (nullptr != pArg) {
        CBULLET_DESC* pDesc = static_cast<CBULLET_DESC*>(pArg);
        m_pTagetPos = pDesc->pTagetPos;
        m_vPos = pDesc->vPos;
        m_vPlayerAt = pDesc->vPlayerAt;
        m_iWeaponType = pDesc->iWeaponType;
        m_fFall_Y = pDesc->Fall_Y;
        m_pDamage = pDesc->Damage;
        m_fLifeTime = pDesc->LifTime;
        m_DATA_TYPE = pDesc->bSturn;
        if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;
    }else
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, m_vPos);

    _vector Dir = m_pTagetPos - m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
    m_vDir = XMVector3Normalize(Dir);


    if (0.f == m_fLifeTime)
        m_fLifeTime = 1.f;
    return S_OK;
}

_int CBullet::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        return OBJ_DEAD;
    }  
    
    m_fDeadTime += fTimeDelta;

    if (m_fDeadTime > m_fLifeTime)
             m_bDead = true;

    _bool bJump{};
    switch (m_iWeaponType)
    {
    case PLATER_BULLET ::TYPE_HENDGUN : 
          m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
        break;

    case PLATER_BULLET::TYPE_ASSAULTRIFLE:
        m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
        break;

    case PLATER_BULLET::TYPE_MISSILEGATLING:
        m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
        break;

    case PLATER_BULLET::TYPE_HEAVYCROSSBOW:
        m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
        break;

    case MONSTER_BULLET::TYPE_GUNPAWN :
        m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
        break;

    case MONSTER_BULLET::TYPE_BOOMBOT:
        m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
    ///    m_pTransformCom->Go_jump_Dir(fTimeDelta, m_vDir, *m_fFall_Y, &bJump);
        break;

    case MONSTER_BULLET::TYPE_JETFLY:
         m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
        break;

    case MONSTER_BULLET::TYPE_MECANOBOT:
        m_pTransformCom->GO_Dir(fTimeDelta, m_vDir);
        break;

    }

    return OBJ_NOEVENT;

}

void CBullet::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CBullet::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CBullet::Render()
{
    __super::Render();
    return S_OK;
}

_float CBullet::Get_Scalra_float()
{
    return *m_pDamage;
}

HRESULT CBullet::Add_Components()
{
  
    CBounding_Sphere::BOUND_SPHERE_DESC		CBounding_Sphere{};


    _float3 Center{}, Extents{};
    CBounding_Sphere.fRadius = 0.1f;
    CBounding_Sphere.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CBounding_Sphere)))
        return E_FAIL;

    return S_OK;
}



CBullet* CBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBullet* pInstance = new CBullet(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBullet::Clone(void* pArg)
{
    CBullet* pInstance = new CBullet(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBullet");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBullet::Free()
{
    __super::Free();

}
