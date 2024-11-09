#include "stdafx.h"
#include "SceneCamera.h"
#include "GameInstance.h"

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
    m_DATA_TYPE = CGameObject::DATA_CAMERA;
    m_vStopPos = pDesc->vStopPos;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    _vector vDir = m_vStopPos - m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
    _float fLength = XMVectorGetX(XMVector3Length(vDir));
     m_fRunTime = fLength / pDesc->fSpeedPerSec;

     return S_OK;
}

_int CSceneCamera::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    m_TimeSum += fTimeDelta;

    if (m_fRunTime > m_TimeSum)
        m_pTransformCom->Go_Straight(fTimeDelta);
    else
        m_bIsCamEnd = true;

    __super::Priority_Update(fTimeDelta);

    return OBJ_NOEVENT;
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
