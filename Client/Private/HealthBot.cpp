#include "stdafx.h"
#include "GameInstance.h"
#include "HealthBot.h"
#include "Body_HealthBot.h"
#include "Player_HpUI.h"
#include "Player.h"
CHealthBot::CHealthBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CActor{pDevice, pContext}
{
}

CHealthBot::CHealthBot(const CHealthBot& Prototype) : CActor{Prototype}
{
}

HRESULT CHealthBot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHealthBot::Initialize(void* pArg)
{
    CActor::Actor_DESC* Desc = static_cast<CActor::Actor_DESC*>(pArg);
    Desc->iNumPartObjects = PART_END;
    Desc->fRotationPerSec = XMConvertToRadians(60.f);
    Desc->Object_Type = GAMEOBJ_TYPE::NPC;
    Desc->bOnCell = true;
    Desc->iState = ST_IDLE;

    if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    m_pInteractiveUI = static_cast<CInteractiveUI*>(m_pGameInstance->Find_Clone_UIObj(L"Interactive"));
    Safe_AddRef(m_pInteractiveUI);

    if (FAILED(Init_CallBack()))
        return E_FAIL;
  
    return S_OK;
}

void CHealthBot::Priority_Update(_float fTimeDelta)
{
    if (false == HasState(ST_DEAD))
        m_pTransformCom->Rotation_to_Player(fTimeDelta);

    __super::Priority_Update(fTimeDelta);
}

void CHealthBot::Update(_float fTimeDelta)
{
    if (HasState(FLAG_INTERACTUI) && m_pInteractiveUI->Get_Interactive(this)) // 확인 여부
    {
        static_cast<CPlayer*>(m_pGameInstance->Get_Player())->SetFlag(CPlayer::FLAG_HEALTH, true);

        m_iState &= ~(ST_IDLE | ST_DEAD);
        m_iState |= ST_INTERACT;

        m_iRim = RIM_LIGHT_DESC::STATE_NORIM;
    }
    else if (HasState(FLAG_INTERACTUI) && !(m_iState & ST_DEAD)) // 확인 안됐으면
    {
        m_iRim = RIM_LIGHT_DESC::STATE_RIM;
        SetState(FLAG_OPENUI, true);
    }

    if ((m_iState & ST_INTERACT) && HasState(FLAG_INTERACTUI))
    {
        m_pGameInstance->Set_OpenUI(true, TEXT("WeaPon_Aim"));
        m_pGameInstance->Set_OpenUI(false, TEXT("Interactive"), this);
    }

    __super::Update(fTimeDelta);
}


void CHealthBot::Late_Update(_float fTimeDelta)
{
    if (HasState(ST_INTERACT) && static_cast<CBody_HealthBot*>(m_PartObjects[PART_BODY])->Get_Finish())
    {
        m_iRim = RIM_LIGHT_DESC::STATE_NORIM;

        m_iState &= ~(ST_IDLE | ST_INTERACT | FLAG_OPENUI);
        m_iState |= ST_DEAD;
    }

    if (FAILED(m_pGameInstance->Add_Interctive(this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CHealthBot::Render()
{
    __super::Render();
    return S_OK; 
}

HRESULT CHealthBot::Add_Components()
{ 
    CBounding_OBB::BOUND_OBB_DESC OBBDesc{};
    _float3 Center{}, Extents{};
    OBBDesc.vExtents = _float3(1.f, 1.3f, 3.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);
    OBBDesc.vRotation = { 0.f,0.f,0.f };

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CHealthBot::Add_PartObjects()
{
    CBody_HealthBot::BODY_HEALTHBOT_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.fSpeedPerSec = 0.f;
    BodyDesc.fRotationPerSec = 0.f;
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pRimState = &m_iRim;
    if (FAILED(__super::Add_PartObject(TEXT("Prototype_GameObject_Body_HealthBot"), PART_BODY, &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CHealthBot::Init_CallBack()
{
    m_pColliderCom->SetTriggerCallback(
     [this](CActor* other, _bool bColliding, _bool bPlayer)
     {
        if (bColliding && bPlayer)
        {
            if ( false == HasState(ST_DEAD)) // 충돌 중
            {
                m_pInteractiveUI->Set_Text(L"체력 회복");
                m_pInteractiveUI->Set_OnwerPos(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION));
                m_pGameInstance->Set_OpenUI(true, TEXT("Interactive"), this);
                m_iState |= FLAG_INTERACTUI;
            }
        }
        else if (bPlayer)
        {
            m_iRim = RIM_LIGHT_DESC::STATE_NORIM;
            SetState(FLAG_INTERACTUI, false);
            if (HasState(FLAG_OPENUI))
            {
                m_pGameInstance->Set_OpenUI(true, TEXT("WeaPon_Aim"));
                m_pGameInstance->Set_OpenUI(false, TEXT("Interactive"), this);
                SetState(FLAG_OPENUI, false);
            }
        }
     });

    return S_OK;
}

CHealthBot* CHealthBot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHealthBot* pInstance = new CHealthBot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHealthBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHealthBot::Clone(void* pArg)
{
    CHealthBot* pInstance = new CHealthBot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CHealthBot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHealthBot::Free()
{
    __super::Free();

    Safe_Release(m_pInteractiveUI);
}
