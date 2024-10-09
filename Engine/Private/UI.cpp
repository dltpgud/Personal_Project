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
        m_fSizeX = pDesc->fSizeX;
        m_fSizeY = pDesc->fSizeY;
        m_UIID = pDesc->UID;
        m_iDepth = pDesc->iDepth;
        m_bPrUpdate = pDesc->PrUpdate;
        m_bUpdate = pDesc->Update;
        m_bLateUpdate = pDesc->LateUpdate;
        m_Hoverst = pDesc->Hoverst;
        m_iDepth = pDesc->iDepth;
        if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;
    }
    else if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    // 컨텍스트 디바이스는 원본 이니셜에서 설정해 주지 못함, 원본이 Loader(서브 스레드)에서 만들어 지면서 호출되기 때문
    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

    /* 뷰스페이스 상의 화면에 보여줄 영역(뷰볼륨)을 설정한다. */
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f));

    return S_OK;
}

_int CUI::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CUI::Update(_float fTimeDelta)
{
}

void CUI::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI::Render()
{

    m_pTransformCom->Set_TRANSFORM(
        CTransform::TRANSFORM_POSITION,
        XMVectorSet(m_fX + -ViewportDesc.Width * 0.5f, -m_fY + ViewportDesc.Height * 0.5f, 0.f, 1.f));


     return S_OK;
}

void CUI::Set_UI_Pos(void* pArg)
{
    if (nullptr != pArg)
    {
        CUI_DESC* pDesc = static_cast<CUI_DESC*>(pArg);

        m_fX = pDesc->fX;
        m_fY = pDesc->fY;
        m_fSizeX = pDesc->fSizeX;
        m_fSizeY = pDesc->fSizeY;
        m_UIID = pDesc->UID;
        m_iDepth = pDesc->iDepth;
        m_bPrUpdate = pDesc->PrUpdate;
        m_bUpdate = pDesc->Update;
        m_bLateUpdate = pDesc->LateUpdate;
        m_Hoverst = pDesc->Hoverst;
        m_iDepth = pDesc->iDepth;
    }

    // 컨텍스트 디바이스는 원본 이니셜에서 설정해 주지 못함, 원본이 Loader(서브 스레드)에서 만들어 지면서 호출되기 때문

    m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformCom->Set_TRANSFORM(
        CTransform::TRANSFORM_POSITION,
        XMVectorSet(m_fX - ViewportDesc.Width * 0.5f, -m_fY + ViewportDesc.Height * 0.5f, 0.f, 1.f));
}

void CUI::Set_UI_shaking(_float fShakingTime, _float fPowerX, _float fPowerY)
{

    m_fShakingTime = fShakingTime;
    m_fShakingPower_X = fPowerX;
    m_fShakingPower_Y = fPowerY;
    m_IsShaking = true;
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
