#include "stdafx.h"
#include "SceneCamera.h"
#include "GameInstance.h"
#include "Player.h"
#include "BillyBoom.h"
#include "Fade.h"
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
    pDesc->Object_Type = GAMEOBJ_TYPE::CAMERA;
    m_vStopPos = pDesc->vStopPos;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    _vector vDir = m_vStopPos - m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION);
    _float fLength = XMVectorGetX(XMVector3Length(vDir));
     m_fRunTime = fLength / pDesc->fSpeedPerSec;

     static_cast<CFade*>(m_pGameInstance->Find_Clone_UIObj(L"Fade"))->Set_Fade(false);
     return S_OK;
}

void CSceneCamera::Priority_Update(_float fTimeDelta)
{
  m_TimeSum += fTimeDelta;

  if (m_fRunTime > m_TimeSum) {
      m_pTransformCom->Go_Move(CTransform::GO,fTimeDelta);
  }
  else
  {  
      if (false == m_bintro)
      {
          dynamic_cast<CBillyBoom*>(
              m_pGameInstance->Find_CloneGameObject(LEVEL_BOSS, TEXT("Layer_Monster"), GAMEOBJ_TYPE::ACTOR))
              ->Change_State(CBillyBoom::ST_INTRO);
          m_bintro = true;
      }
  }

  __super::Priority_Update(fTimeDelta);
   
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
