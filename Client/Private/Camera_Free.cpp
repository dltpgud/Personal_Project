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

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

_int CCamera_Free::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;


//    _uint iState = static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_State();
//
//
//if (CPlayer::STATE_ASSAULTRIFlLE_SHOOT == iState || CPlayer::STATE_HEAVYCROSSBOW_SHOOT == iState || CPlayer::STATE_HENDGUN_SHOOT == iState || CPlayer::STATE_MissileGatling_SHOOT == iState)
//{ // 반동을 이렇게 해서 잡으면 움직이면서 총을 못쏨 이상태 일떄 따로 카메라를 움직이게 해야함
//
//    
//}
//else {
// _vector vEye = { static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_CameraBone()->_41,
//         static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_CameraBone()->_42,
//         static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_CameraBone()->_43,
//   static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Get_CameraBone()->_44 };
// 
// _vector Eye = XMVector3TransformCoord(vEye, m_pGameInstance->Get_Player()->Get_Transform()->Get_WorldMatrix());
// 
// m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, Eye);
//
//  _float4 At;
// XMStoreFloat4(&At, m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)
//     + m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_LOOK));
// m_pTransformCom->LookAt(XMLoadFloat4(&At));
// }

 m_pTransformCom->Set_MoveSpeed( m_pGameInstance->Get_Player()->Get_Transform()->Get_MoveSpeed());
 m_pTransformCom->Set_RotSpeed(  m_pGameInstance->Get_Player()->Get_Transform()->Get_RotSpeed());

if (m_pGameInstance->Get_DIKeyState(DIK_W))
{
    m_pTransformCom->Go_Straight(fTimeDelta);
}
if (m_pGameInstance->Get_DIKeyState(DIK_S))
{
    m_pTransformCom->Go_Backward(fTimeDelta);
}

if (m_pGameInstance->Get_DIKeyState(DIK_A))
{
    m_pTransformCom->Go_Left(fTimeDelta);

}

if (m_pGameInstance->Get_DIKeyState(DIK_D))
{
    
    m_pTransformCom->Go_Right(fTimeDelta);

}

if (m_pGameInstance->Get_DIKeyDown(DIK_TAB))
{
    if (!m_bturn)
    {
        m_bturn = true;
    }
    else
        m_bturn = false;
}

if (true == m_bturn)
{
    Mouse_Fix();
    _long MouseMove = {0};

    if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.05f);
    }

    if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
    {
        m_pTransformCom->Turn(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_RIGHT),
                              fTimeDelta * MouseMove * 0.05f);
    }
}




if (m_pGameInstance->Get_DIKeyDown(DIK_SPACE))
    m_bjump = true;

  if (m_bjump)
  {
  
      m_pTransformCom->Go_jump(fTimeDelta, 2.f, &m_bjump);
  }

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

void CCamera_Free::Mouse_Fix()
{
    POINT ptMouse;
    ptMouse = {g_iWinSizeX >> 1, g_iWinSizeY >> 1};

    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
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
