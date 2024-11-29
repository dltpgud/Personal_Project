#include "stdafx.h"
#include "GameInstance.h"
#include "HealthBot.h"
#include "Body_HealthBot.h"
#include "PlayerUI.h"
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

    CActor::Actor_DESC Desc{};
    Desc.iNumPartObjects = PART_END;
    Desc.fSpeedPerSec = 3.f;
    Desc.fRotationPerSec = XMConvertToRadians(60.f);
    Desc.JumpPower = 3.f;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
                if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    m_iState = ST_Idle;

  
    m_fMAXHP = 100.f;
    m_fHP = m_fMAXHP;
    m_bOnCell = true;
    m_DATA_TYPE = CGameObject::DATA_NPC;
    m_pInteractiveUI = static_cast <CInteractiveUI*>(m_pGameInstance->Get_UI(LEVEL_STATIC, CUI::UIID_InteractiveUI));
    m_CPlayerUI = static_cast <CPlayerUI*>(m_pGameInstance->Get_UI(LEVEL_STATIC, CUI::UIID_PlayerHP));
    return S_OK;
}

_int CHealthBot::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CHealthBot::Update(_float fTimeDelta)
{
    if (false == m_bInteract)
    {
        m_pTransformCom->Rotation_to_Player(fTimeDelta);
        intersect(fTimeDelta);
    }



    if (true == m_bHealth)
    {
        m_fTimeSum += fTimeDelta;
        if (true == m_bSound) {
            m_pGameInstance->Play_Sound(L"ST_Healbot_Use_09_15_2023.ogg", CSound::SOUND_BGM, 1.f);
            m_bSound = false;
        }
        if (m_fTimeSum > 1.f) {

            static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_UIState(CPlayer::STATE_UI_IDlE);
            m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerState, false);
            m_fTimeSum = 0.f;
            m_bHealth = false;
        
        }
    }
   
    __super::Update(fTimeDelta);
}

void CHealthBot::Late_Update(_float fTimeDelta)
{
    if (true == m_bInteract && true == m_PartObjects[PART_BODY]->Get_Finish())
        m_iState = ST_Dead;
    __super::Late_Update(fTimeDelta);
}

HRESULT CHealthBot::Render()
{
    __super::Render();
    return S_OK; 
}


void CHealthBot::intersect(_float fTimedelta)
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPlayerPos - vPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));
   
        if (fLength <= 5.f)
        {

            m_pInteractiveUI->Set_Text(L"체력 회복");

            m_iRim = RIM_LIGHT_DESC::STATE_RIM;
            m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_InteractiveUI, CUI::UIID_PlayerWeaPon_Aim);

            if (m_pInteractiveUI->Get_Interactive())
            {
                static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_UIState(CPlayer::STATE_UI_HEALTH);
                m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerState, true);
                m_bHealth = true;
                m_iState = ST_Interactive;
                m_bInteract = true;
                m_iRim = RIM_LIGHT_DESC::STATE_NORIM;
                m_pInteractiveUI->Set_Interactive(false);
            }
            m_bOpenUI = true;
        }
        else {

            m_iRim = RIM_LIGHT_DESC::STATE_NORIM;
            m_iState = ST_Idle;

            if (m_bOpenUI == true) {
                m_pGameInstance->Set_OpenUI_Inverse(CUI::UIID_PlayerWeaPon_Aim, CUI::UIID_InteractiveUI);
                m_bOpenUI = false;
            }
        }



    if ( m_bInteract == true && fLength <= 5.f)
    {

        m_CPlayerUI->Set_HPGage(100);
        m_pGameInstance->Set_OpenUI_Inverse( CUI::UIID_PlayerWeaPon_Aim, CUI::UIID_InteractiveUI);
    }



}

HRESULT CHealthBot::Add_Components()
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

HRESULT CHealthBot::Add_PartObjects()
{
    /* For.Body */
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

}
