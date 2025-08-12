#include "UI.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CUI::CUI(const CUI& Prototype) : CGameObject{Prototype}
{
}

HRESULT CUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        CUI_DESC* pDesc = static_cast<CUI_DESC*>(pArg);

        m_fX = pDesc->fX;
        m_fY = pDesc->fY;
        m_fZ = pDesc->fZ;
        m_fSizeX = pDesc->fSizeX;
        m_fSizeY = pDesc->fSizeY;
        m_UIID = pDesc->UID;
        m_iDeleteLevel = pDesc->iDeleteLevel;
        m_bUpdate = pDesc->Update; 
        if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;
    }
    else if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f));

    return S_OK;
}

void CUI::Priority_Update(_float fTimeDelta)
{
}

void CUI::Update(_float fTimeDelta)
{
}

void CUI::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI::Render()
{
    return S_OK;
}

void CUI::Set_UI_Pos(void* pArg)
{
    if (nullptr != pArg)
    {
        CUI_DESC* pDesc = static_cast<CUI_DESC*>(pArg);

        m_fX = pDesc->fX;
        m_fY = pDesc->fY;
        m_fZ = pDesc->fZ;
        m_fSizeX = pDesc->fSizeX;
        m_fSizeY = pDesc->fSizeY;
    }

    m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_TRANSFORM(
        CTransform::T_POSITION,
        XMVectorSet(m_fX - ViewportDesc.Width * 0.5f, -m_fY + ViewportDesc.Height * 0.5f, m_fZ, 1.f));
}

void CUI::Set_UI_shaking(_float fShakingTime, _float fPowerX, _float fPowerY)
{
    m_fShakingTime = fShakingTime;
    m_fShakingPower_X = fPowerX;
    m_fShakingPower_Y = fPowerY;
}

_bool CUI::Check_Deleate(_uint Level)
{
    return m_iDeleteLevel == Level;
}


void CUI::UI_shaking(_float fTimeDelta)
{
    if (m_fShakingTime > 0.f)
    {
        m_fShakingTime -= fTimeDelta;
        m_fShaking_X = (_float)((rand() % 21)) * m_fShakingPower_X * 0.1f;
        m_fShaking_Y = (_float)((rand() % 21)) * m_fShakingPower_Y * 0.1f;

        if (m_fShakingTime <= 0.f)
        {
            m_fShakingTime = 0;
            m_fShaking_X = 0.f;
            m_fShaking_Y = 0.f;
        }
    }
}

void CUI::Free()
{
    __super::Free();
}
