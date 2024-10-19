#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
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

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
    _vector vLook = Get_TRANSFORM(TRANSFORM_LOOK);
    _vector vPosition = Get_TRANSFORM(TRANSFORM_POSITION);

    _vector vAfterPos = vPosition + XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    _vector Slide{};
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &Slide)) {
        vPosition += Slide;
    }
    else
        vPosition = vAfterPos;
  
    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
    _vector vRight = Get_TRANSFORM(CTransform::TRANSFORM_RIGHT);
    _vector vPosition = Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vAfterPos = vPosition + XMVector3Normalize(-vRight) * m_fSpeedPerSec * fTimeDelta;

    _vector Slide{};
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &Slide)) {
        if (XMVector3Equal(Slide, XMVectorZero()))
            return;
        vPosition += Slide;
    }
    else
        vPosition = vAfterPos;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
    _vector vRight = Get_TRANSFORM(TRANSFORM_RIGHT);
    _vector vPosition = Get_TRANSFORM(TRANSFORM_POSITION);

    _vector vAfterPos = vPosition + XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    _vector Slide{};
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &Slide))
        {
            if (XMVector3Equal(Slide, XMVectorZero()))
                return;
            vPosition += Slide;
        }
    else
        vPosition = vAfterPos;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
    _vector vLook = Get_TRANSFORM(TRANSFORM_LOOK);
    _vector vPosition = Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vAfterPos = vPosition + XMVector3Normalize(-vLook) * m_fSpeedPerSec * fTimeDelta;

    _vector Slide{};
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &Slide))
    {
        if (XMVector3Equal(Slide, XMVectorZero()))
            return;
        vPosition += Slide;
    }
    else
        vPosition = vAfterPos;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta, CNavigation* pNavigation)
{
    _vector vUp = Get_TRANSFORM(TRANSFORM_UP);
    _vector vPosition = Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vAfterPos = vPosition + XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

    _vector Slide{};
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &Slide))
    {
        if (XMVector3Equal(Slide, XMVectorZero()))
            return;
        vPosition += Slide;
    }
    else
        vPosition = vAfterPos;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta, CNavigation* pNavigation)
{
    _vector vUp = Get_TRANSFORM(TRANSFORM_UP);
    _vector vPosition = Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vAfterPos = vPosition + XMVector3Normalize(-vUp) * m_fSpeedPerSec * fTimeDelta;

    _vector Slide{};
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &Slide))
    {
        if (XMVector3Equal(Slide, XMVectorZero()))
            return;
        vPosition += Slide;
    }
    else
        vPosition = vAfterPos;

    Set_TRANSFORM(TRANSFORM_POSITION, vPosition);
}

void CTransform::Go_jump(_float fTimeDelta, _float YPos, _bool* Jumpcheck,  CNavigation* pNavigation)
{

    m_fTimeSum += fTimeDelta * 9.8f;
 

    _vector vPosition = Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

      Go_Straight(fTimeDelta, pNavigation);

    _vector vUp = Get_TRANSFORM(CTransform::TRANSFORM_UP);

 /*
    vPosition += XMVector3Normalize(vUp) * (m_JumpPower - m_fTimeSum) * fTimeDelta * m_fSpeedPerSec;
  if (nullptr != pNavigation && false == pNavigation->isMove(vPosition)) {
        return;
    }
    
   */   
     Set_TRANSFORM(CTransform::TRANSFORM_POSITION, vPosition);


    _float3 Position;
    XMStoreFloat3(&Position, Get_TRANSFORM(CTransform::TRANSFORM_POSITION));
    if (Position.y <= YPos)
    {
        Position.y = YPos;
       Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(Position.x, Position.y, Position.z, 1.f));
        m_fTimeSum = 0.f;
        *Jumpcheck = false;
    }
}

void CTransform::Stop_Move()
{
    Set_TRANSFORM(CTransform::TRANSFORM_POSITION, Get_TRANSFORM(CTransform::TRANSFORM_POSITION));
}

void CTransform::Rotation_to_Player()
{
   _matrix met = XMMatrixLookAtLH(Get_TRANSFORM(CTransform::TRANSFORM_POSITION), 
       m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION), XMVectorSet(0.f, 1.f, 0.f, 0.f));


   _matrix Wmet = XMMatrixInverse(nullptr, met);

   _float4x4 wmet{};

   XMStoreFloat4x4(&wmet, Wmet);
   Set_TRANSFORM(CTransform::TRANSFORM_RIGHT, XMVectorSet(wmet._11, 0.f, wmet._13, wmet._14));
   Set_TRANSFORM(CTransform::TRANSFORM_UP, XMVectorSet(0.f, 1.f, 0.f, wmet._24));
   Set_TRANSFORM(CTransform::TRANSFORM_LOOK, XMVectorSet(wmet._31, 0.f, wmet._33, wmet._34));
}


void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
    _vector vRight = Get_TRANSFORM(TRANSFORM_RIGHT);
    _vector vUp = Get_TRANSFORM(TRANSFORM_UP);
    _vector vLook = Get_TRANSFORM(TRANSFORM_LOOK);

    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

    _vector vNewUp = XMVector3TransformNormal(vUp, RotationMatrix);
    if (XMVectorGetY(vNewUp) < 0)
        return;

    Set_TRANSFORM(TRANSFORM_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_TRANSFORM(TRANSFORM_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_TRANSFORM(TRANSFORM_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_bool bX, _bool bY, _bool bZ, _float fTimeDelta)
{
    _vector		vRight = Get_TRANSFORM(TRANSFORM_RIGHT);
    _vector		vUp    = Get_TRANSFORM(TRANSFORM_UP);
    _vector		vLook  = Get_TRANSFORM(TRANSFORM_LOOK);

    _float		fRotationSpeed = m_fRotationPerSec * fTimeDelta;

    _vector		vQuaternion = XMQuaternionRotationRollPitchYaw(bX * fRotationSpeed, bY * fRotationSpeed, bZ * fRotationSpeed);

    _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

    Set_TRANSFORM(TRANSFORM_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_TRANSFORM(TRANSFORM_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_TRANSFORM(TRANSFORM_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
    _float3		vScaled = Get_Scaled();

    _vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
    _vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
    _vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

    _vector		vQuaternion = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

    _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

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
         m_JumpPower = pDesc->JumpPower;
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
