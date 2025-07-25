#include "stdafx.h"
#include "SceneCamera.h"
#include "GameInstance.h"
#include "Player.h"
#include "BillyBoom.h"
#include "Pade.h"
CSceneCamera::CSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCamera{pDevice, pContext}
{
}

CSceneCamera::CSceneCamera(const CSceneCamera& Prototype) : CCamera{Prototype}
{
}

HRESULT CSceneCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSceneCamera::Initialize(void* pArg)
{
    CSceneCamera_DESC* pDesc = static_cast<CSceneCamera_DESC*>(pArg);
    pDesc->Object_Type = CGameObject::GAMEOBJ_TYPE::CAMERA;
    m_vStopPos = pDesc->vStopPos;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    _vector vDir = m_vStopPos - m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION);
    _float fLength = XMVectorGetX(XMVector3Length(vDir));
     m_fRunTime = fLength / pDesc->fSpeedPerSec;
     static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_Key(false);
     return S_OK;
}

void CSceneCamera::Priority_Update(_float fTimeDelta)
{
    if (m_bDead) {
  
    //    static_cast<CPade*>(m_pGameInstance->Get_UI(LEVEL_STATIC, CUI::UIID_Pade))->Set_Pade(false);
        return ;
    }

    if (false == m_bfade)
    {
       static_cast<CPade*>(m_pGameInstance->Find_Clone_UIObj(L"Fade"))->Set_Pade(false);
        m_pGameInstance->Set_OpenUI(CUI::UIID_Pade, true);
      if(true ==  static_cast<CPade*>(m_pGameInstance->Find_Clone_UIObj(L"Fade"))->FinishPade())
          m_bfade = true;
    }

    if (true == m_bfade)
    {
        m_TimeSum += fTimeDelta;

        if (m_fRunTime > m_TimeSum) {
            if (false == m_bintroSound) {
                m_pGameInstance->Play_Sound(L"ST_BillyBoom_Presence.ogg", CSound::CHANNELID::SOUND_BGM, 0.3f);
                m_bintroSound = true;
            }
            m_pTransformCom->Go_Move(CTransform::GO,fTimeDelta);
            
        }
        else
            m_bIsCamEnd = true;


        if (true == m_bIsCamEnd) {

         dynamic_cast<CBillyBoom*>(
                m_pGameInstance->Find_CloneGameObject(LEVEL_BOSS, TEXT("Layer_Monster"), CGameObject::ACTOR))
                ->Intro_Routine(fTimeDelta);
        }

        __super::Priority_Update(fTimeDelta);
    }
}

void CSceneCamera::Update(_float fTimeDelta)
{
}

void CSceneCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CSceneCamera::Render()
{
    return S_OK;
}

CSceneCamera* CSceneCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSceneCamera* pInstance = new CSceneCamera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSceneCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSceneCamera::Clone(void* pArg)
{
    CSceneCamera* pInstance = new CSceneCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CSceneCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSceneCamera::Free()
{
    __super::Free();
}
