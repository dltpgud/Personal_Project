#include "stdafx.h"
#include "Camera_Tool.h"
#include "GameInstance.h"

CCamera_Tool::CCamera_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Tool::CCamera_Tool(const CCamera_Tool& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CCamera_Tool::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Tool::Initialize(void* pArg)
{
    CAMERA_Tool_DESC* pDesc = static_cast<CAMERA_Tool_DESC*>(pArg);

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Tool::Priority_Update(_float fTimeDelta)
{

    if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
        m_pTransformCom->Go_Move(CTransform::GO,fTimeDelta);

    if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
        m_pTransformCom->Go_Move(CTransform::BACK, fTimeDelta);

    if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)  
        m_pTransformCom->Go_Move(CTransform::LEFT, fTimeDelta);

    if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
        m_pTransformCom->Go_Move(CTransform::RIGHT, fTimeDelta);

    if (m_pGameInstance->Get_DIKeyDown(DIK_TAB))
    {

        if (!m_bturn)
            m_bturn = true;

        else
            m_bturn = false;

    }

    if (true == m_bturn) 
    {
        _long		MouseMove = { 0 };

        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
        {
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
        }

        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
        {
            m_pTransformCom->Turn(m_pTransformCom->Get_TRANSFORM(CTransform::T_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
        }

        Mouse_Fix();

    }
    __super::Priority_Update(fTimeDelta);
}

void CCamera_Tool::Update(_float fTimeDelta)
{

}

void CCamera_Tool::Late_Update(_float fTimeDelta)
{

}

HRESULT CCamera_Tool::Render()
{
    return S_OK;
}

void CCamera_Tool::Mouse_Fix()
{
    POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
}

CCamera_Tool* CCamera_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Tool* pInstance = new CCamera_Tool(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCamera_Tool");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Tool::Clone(void* pArg)
{
    CCamera_Tool* pInstance = new CCamera_Tool(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CCamera_Tool");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Tool::Free()
{
    __super::Free();
}
