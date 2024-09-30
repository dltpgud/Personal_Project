#include "Transform.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

void CTransform::Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
    _vector vRight = Get_TRANSFORM(TRANSFORM_RIGHT);
    _vector vUp = Get_TRANSFORM(TRANSFORM_UP);
    _vector vLook = Get_TRANSFORM(TRANSFORM_LOOK);

    Set_TRANSFORM(TRANSFORM_RIGHT, XMVector3Normalize(vRight) * fScaleX);
    Set_TRANSFORM(TRANSFORM_UP, XMVector3Normalize(vUp) * fScaleY);
    Set_TRANSFORM(TRANSFORM_LOOK, XMVector3Normalize(vLook) * fScaleZ);
}

void CTransform::LookAt(_fvector vAt)
{
    _float3 vScaled = Get_Scaled();

    _vector vLook = vAt - Get_TRANSFORM(TRANSFORM_POSITION);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector vUp = XMVector3Cross(vLook, vRight);

    Set_TRANSFORM(TRANSFORM_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
    Set_TRANSFORM(TRANSFORM_UP, XMVector3Normalize(vUp) * vScaled.y);
    Set_TRANSFORM(TRANSFORM_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Go_Straight(_float fTimeDelta)
{
    _vector vLook = Get_TRANSFORM(TRANSFORM_LOOK);
    _vector vPosition = Get_TRANSFORM(TRANSFORM_POSITION);

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
    _vector vRight = Get_TRANSFORM(CTransform::TRANSFORM_RIGHT);
    _vector vPosition = Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
    _vector vRight = Get_TRANSFORM(TRANSFORM_RIGHT);
    _vector vPosition = Get_TRANSFORM(TRANSFORM_POSITION);

    vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
    _vector vLook = Get_TRANSFORM(TRANSFORM_LOOK);
    _vector vPosition = Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
    _vector vUp = Get_TRANSFORM(TRANSFORM_UP);
    _vector vPosition = Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
    _vector vUp = Get_TRANSFORM(TRANSFORM_UP);
    _vector vPosition = Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
    _vector vRight = Get_TRANSFORM(TRANSFORM_RIGHT);
    _vector vUp = Get_TRANSFORM(TRANSFORM_UP);
    _vector vLook = Get_TRANSFORM(TRANSFORM_LOOK);

    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

    Set_TRANSFORM(TRANSFORM_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_TRANSFORM(TRANSFORM_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_TRANSFORM(TRANSFORM_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

HRESULT CTransform::Initialize_Prototype(void* pTransformDesc)
{
    if (nullptr != pTransformDesc)
    {
        TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pTransformDesc);

        m_fSpeedPerSec = pDesc->fSpeedPerSec;
        m_fRotationPerSec = pDesc->fRotationPerSec;
    }

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

    }
    return S_OK;
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pTransformDesc)
{
    CTransform* pInstance = new CTransform(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pTransformDesc)))
    {
        MSG_BOX("Failed to Created : CTransform");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
    return nullptr;
}

void CTransform::Free()
{
    __super::Free();
}
