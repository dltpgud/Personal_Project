#include "stdafx.h"
#include "Camera_Free.h"
#include "GameInstance.h"
#include "Player.h"
CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCamera{pDevice, pContext}
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype) : CCamera{Prototype}
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
    CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Player());
    return S_OK;
}

_int CCamera_Free::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;
  _vector vEye = { 
               m_pPlayer->Get_CameraBone()->_41,
               m_pPlayer->Get_CameraBone()->_42,
               m_pPlayer->Get_CameraBone()->_43,
               m_pPlayer->Get_CameraBone()->_44 };
 
  _vector Eye = XMVector3TransformCoord(vEye, m_pGameInstance->Get_Player()->Get_Transform()->Get_WorldMatrix());
  
   m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, Eye);

   _float4 At;
   XMStoreFloat4(&At, Eye+
       + m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_LOOK));
   m_pTransformCom->LookAt(XMLoadFloat4(&At));
   

    __super::Priority_Update(fTimeDelta);

    return OBJ_NOEVENT;
}

void CCamera_Free::Update(_float fTimeDelta)
{
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
    return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCamera_Free");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
    CCamera_Free* pInstance = new CCamera_Free(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CCamera_Free");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Free::Free()
{
    __super::Free();
}
