#include "stdafx.h"
#include "Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
    : CCamera{ pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
    : CCamera{ Prototype }
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


   
    if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
        m_pTransformCom->Go_Straight(fTimeDelta);

    if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
        m_pTransformCom->Go_Backward(fTimeDelta);

    if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
        m_pTransformCom->Go_Left(fTimeDelta);

    if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
        m_pTransformCom->Go_Right(fTimeDelta);

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
