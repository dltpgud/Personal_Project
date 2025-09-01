#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "MonsterHP.h"
CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CActor{pDevice, pContext}
{
}

CMonster::CMonster(const CMonster& Prototype) : CActor{Prototype}
{
}

HRESULT CMonster::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
    CActor::Actor_DESC* Desc = static_cast<CActor::Actor_DESC*>(pArg);
    Desc->iNumPartObjects = PART_END;
    Desc->Object_Type = GAMEOBJ_TYPE::ACTOR;
    Desc->iType = CActor::TYPE_MONSTER;
    if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;


   if (FAILED(m_pGameInstance->Add_Monster(this)))
        return E_FAIL;

    m_pTransformCom->Set_Rotation_to_Player();
   
    return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    return ;
}

void CMonster::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMonster::Late_Update(_float fTimeDelta)
{
     if (FAILED(m_pGameInstance->Add_Monster(this)))
           return;
    
    __super::Late_Update(fTimeDelta);
}

HRESULT CMonster::Render()
{
    __super::Render();
    return S_OK; 
}

void CMonster::Compute_Length()
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION);

    _vector vDir = vPlayerPos - vPos;

    m_fAttackLength = XMVectorGetX(XMVector3Length(vDir));
}

void CMonster::Compute_Angle()
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION);

    _vector vDir = vPlayerPos - vPos;

    vDir = XMVector3Normalize(vDir);
    _vector vLook{}, vRight{};

    vLook = XMVector3Normalize(m_pTransformCom->Get_TRANSFORM(CTransform::T_LOOK));

    vRight = XMVector3Normalize(m_pTransformCom->Get_TRANSFORM(CTransform::T_RIGHT));

    _float fFrontBack = XMVectorGetX(XMVector3Dot(vLook, vDir));

    _float fLeftRight = XMVectorGetX(XMVector3Dot(vRight, vDir));

    if (fFrontBack >= 0.0f) 
    {
        if (fabsf(fLeftRight) <= 0.382f)
            m_iAttackAngleType = AT_FRONT;
        else if (fLeftRight > 0.0f)
            m_iAttackAngleType = AT_RIGHT;
        else
            m_iAttackAngleType = AT_LEFT;
    }
    else 
    {
        if (fabsf(fLeftRight) <= 0.382f) 
            m_iAttackAngleType = AT_BACK;
        else if (fLeftRight > 0.0f)
            m_iAttackAngleType = AT_RIGHT;
        else
            m_iAttackAngleType = AT_LEFT;
    }
}

_bool CMonster::IsLookAtPlayer(_float angle)
{
    _vector PlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
    _vector PlayerForward = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_LOOK);

    _vector dir = m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION) - PlayerPos;

    dir = XMVector3Normalize(dir);
    PlayerForward = XMVector3Normalize(PlayerForward);

    _float dot = XMVectorGetX(XMVector3Dot(PlayerForward, dir));

    _float maxAngle = XMConvertToRadians(angle);

    if (dot > cosf(maxAngle)) {
        return true;
    }

    return false;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster* pInstance = new CMonster(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
    CMonster* pInstance = new CMonster(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonster::Free()
{
    __super::Free();
}
