#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

void CTransform::Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
    _vector vRight = Get_TRANSFORM(T_RIGHT);
    _vector vUp = Get_TRANSFORM(T_UP);
    _vector vLook = Get_TRANSFORM(T_LOOK);

    Set_TRANSFORM(T_RIGHT, XMVector3Normalize(vRight) * fScaleX);
    Set_TRANSFORM(T_UP, XMVector3Normalize(vUp) * fScaleY);
    Set_TRANSFORM(T_LOOK, XMVector3Normalize(vLook) * fScaleZ);
}

void CTransform::LookAt(_fvector vAt)
{
    _float3 vScaled = Get_Scaled();

    _vector vLook = vAt - Get_TRANSFORM(T_POSITION);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector vUp = XMVector3Cross(vLook, vRight);

    Set_TRANSFORM(T_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
    Set_TRANSFORM(T_UP, XMVector3Normalize(vUp) * vScaled.y);
    Set_TRANSFORM(T_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Go_Move(MOVE MoveType, _float fTimeDelta, CNavigation* pNavigation, _bool Demage)
{
    TRANSFORM Move{};
    _int Dir{};

    switch (MoveType)
    {
    case Engine::CTransform::GO: Move = T_LOOK; break;
    case Engine::CTransform::BACK: Move = T_LOOK; Dir = 1; break;
    case Engine::CTransform::RIGHT: Move = T_RIGHT; break;
    case Engine::CTransform::LEFT: Move = T_RIGHT; Dir = 1; break;
    case Engine::CTransform::UP: Move = T_UP; break;
    case Engine::CTransform::DOWN: Move = T_UP; Dir = 1; break;
    default: break;
    }

    _vector vTrans = Get_TRANSFORM(Move);

    _vector vPosition = Get_TRANSFORM(T_POSITION);

    Dir == 1 ? vTrans *= -1 : vTrans;

    _vector vAfterPos = vPosition + XMVector3Normalize(vTrans) * m_fSpeedPerSec * fTimeDelta;

    _vector Slide{};
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &Slide, Demage)) {
       vPosition += Slide;
    }
    else
       vPosition = vAfterPos;
  
    Set_TRANSFORM(T_POSITION, vPosition);
}

void CTransform::Go_jump(_float fTimeDelta, _float YPos, _bool* Jumpcheck,  CNavigation* pNavigation)
{
    m_fTimeSum += fTimeDelta * 2.8f;
 
    _vector vPosition = Get_TRANSFORM(CTransform::T_POSITION);

    _vector vUp = Get_TRANSFORM(CTransform::T_UP);

    _vector vAfterPos = vPosition + XMVector3Normalize(vUp) * (m_JumpPower - m_fTimeSum) * fTimeDelta * m_fSpeedPerSec;

    _vector slide{}; 
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &slide))
    {
       vAfterPos += slide;
    }
          
     Set_TRANSFORM(CTransform::T_POSITION, vAfterPos);

    _float3 Position;
    XMStoreFloat3(&Position, Get_TRANSFORM(CTransform::T_POSITION));
    if (Position.y <= YPos)
    {
        Position.y = YPos;
       Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(Position.x, Position.y, Position.z, 1.f));
        m_fTimeSum = 0.f;
        m_fTimeSumDouble = 0.f;
        *Jumpcheck = false;
    }
}

void CTransform::Go_Doublejump(_float fTimeDelta, CNavigation* pNavigation)
{
    
    m_fTimeSumDouble += fTimeDelta * 2.8f;

    _vector vPosition = Get_TRANSFORM(CTransform::T_POSITION);

    _vector vUp = Get_TRANSFORM(CTransform::T_UP);

    _vector vAfterPos = vPosition + XMVector3Normalize(vUp) * (m_JumpPower*1.5f - m_fTimeSumDouble) * fTimeDelta * m_fSpeedPerSec;

    _vector slide{};
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &slide))
    {
        vAfterPos += slide;
    }

    Set_TRANSFORM(CTransform::T_POSITION, vAfterPos);

}

void CTransform::GO_Dir(_float fTimeDelta, _vector vDir, CNavigation* pNavigation, _bool* bStop )
{
    _vector vPosition = Get_TRANSFORM(T_POSITION);

    _vector vAfterPos = vPosition + vDir * m_fSpeedPerSec * fTimeDelta;

    _vector Slide{};
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &Slide))
    {
        vPosition += XMVectorZero();

        if(bStop != nullptr)
        *bStop = true;
    }
    else
        vPosition = vAfterPos;


    Set_TRANSFORM(T_POSITION, vPosition);
}

void CTransform::Go_jump_Dir(_float fTimeDelta, _vector Dir, _float YPos, CNavigation* pNavigation, _bool* bStop )
{

    _vector vPosition = Get_TRANSFORM(CTransform::T_POSITION);

    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) *0.09f;

    _vector vAfterPos = vPosition + Dir * fTimeDelta * m_fSpeedPerSec*2.f - vUp;

    _vector Slide{};
    if (nullptr != pNavigation && false == pNavigation->isMove(vAfterPos, vPosition, &Slide))
    {
        vPosition += XMVectorZero();

        if (bStop != nullptr)
            *bStop = false;
    }
    else
        vPosition = vAfterPos;

     Set_TRANSFORM(CTransform::T_POSITION, vPosition);

    if ( XMVectorGetY(Get_TRANSFORM(CTransform::T_POSITION)) <= YPos)
    {
        XMVectorSetY(Get_TRANSFORM(CTransform::T_POSITION), YPos);
        if (bStop != nullptr)
           *bStop = false;
    }
}

void CTransform::Stop_Move()
{
    Set_TRANSFORM(CTransform::T_POSITION, Get_TRANSFORM(CTransform::T_POSITION));
}

void CTransform::Rotation_to_Player(_float fTimeDelta)
{
    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
    _vector vPos = Get_TRANSFORM(CTransform::T_POSITION);

    _vector vDir = vPlayerPos - vPos;
    _vector vNewDir = XMVectorSetY(vDir, 0.f);
    _vector TargetDir = XMVector3Normalize(vNewDir);

    _vector vLook = XMVector3Normalize(Get_TRANSFORM(CTransform::T_LOOK));

    _float angleBetween = XMVectorGetX(XMVector3AngleBetweenNormals(vLook, TargetDir));

    _vector vCross = XMVector3Cross(vLook, TargetDir);
    _float crossY = XMVectorGetY(vCross);


    _float rotationAngle = (crossY >= 0 ? 1.f : -1.f) * min(angleBetween, m_fRotationPerSec * fTimeDelta);
  
    _vector rotationAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f); 
    _matrix RotationMatrix = XMMatrixRotationAxis(rotationAxis, rotationAngle);

    // 새로운 방향 벡터 계산
    Set_TRANSFORM(T_RIGHT, XMVector3TransformNormal(Get_TRANSFORM(T_RIGHT), RotationMatrix));
    Set_TRANSFORM(T_UP, XMVector3TransformNormal(Get_TRANSFORM(T_UP), RotationMatrix));
    Set_TRANSFORM(T_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
    
}

void CTransform::Set_Rotation_to_Player()
{
    _matrix met = XMMatrixLookAtLH(Get_TRANSFORM(CTransform::T_POSITION),
        m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION), XMVectorSet(0.f, 1.f, 0.f, 0.f));

    _matrix Wmet = XMMatrixInverse(nullptr, met);

    _float4x4 wmet{};

    XMStoreFloat4x4(&wmet, Wmet);
    Set_TRANSFORM(CTransform::T_RIGHT, XMVectorSet(wmet._11, 0.f, wmet._13, wmet._14));
    Set_TRANSFORM(CTransform::T_UP, XMVectorSet(0.f, 1.f, 0.f, wmet._24));
    Set_TRANSFORM(CTransform::T_LOOK, XMVectorSet(wmet._31, 0.f, wmet._33, wmet._34));
}

void CTransform::Move_TagetAstar(CNavigation* pNavigation, _float fTimedelta)
{
    vector<_uint> vec = pNavigation->Get_PathPoints();
    if (vec.empty())
        return;

    if (m_CurrentPathIndex >= vec.size())
    {
        m_CurrentPathIndex = 0; // 경로 완료
        return;
    }

    const _uint targetCellIndex = vec[m_CurrentPathIndex];
    const _vector currentPos = Get_TRANSFORM(CTransform::T_POSITION);
    const _vector targetPos = pNavigation->Get_TagetPos(targetCellIndex);

    const _vector dir = targetPos - currentPos;
    _vector dirXZ = XMVectorSetY(dir, 0.f);
    const _float distSq = XMVectorGetX(XMVector3LengthSq(dirXZ));

    constexpr _float thresholdSq = 1.f * 1.f;
    if (distSq < thresholdSq)
    {
        ++m_CurrentPathIndex;
        return;
    }

    _vector vLook = XMVector3Normalize(XMVectorSetY(Get_TRANSFORM(CTransform::T_LOOK), 0.f));
    _vector moveDir = XMVectorSetY(dir, 0.f);
    moveDir = XMVector3Normalize(moveDir);

    const _float angleBetween = XMVectorGetX(XMVector3AngleBetweenNormals(vLook, moveDir));
    const _float maxAngle = m_fRotationPerSec * fTimedelta;

    const _vector cross = XMVector3Cross(vLook, moveDir);
    const _float sign = (XMVectorGetY(cross) >= 0.f) ? 1.f : -1.f;
    const _float rotationAngle = sign * min(angleBetween, maxAngle);

    const _matrix rotMat = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), rotationAngle);

    Set_TRANSFORM(T_RIGHT, XMVector3TransformNormal(Get_TRANSFORM(T_RIGHT), rotMat));
    Set_TRANSFORM(T_UP, XMVector3TransformNormal(Get_TRANSFORM(T_UP), rotMat));
    Set_TRANSFORM(T_LOOK, XMVector3TransformNormal(vLook, rotMat));

    const _vector moveDelta = XMVector3Normalize(dir) * (m_fSpeedPerSec * fTimedelta);
    _vector vAfterPos = currentPos + moveDelta;
    _vector vSlide{};
    if (pNavigation && !pNavigation->isMove(vAfterPos, currentPos, &vSlide))
        Set_TRANSFORM(T_POSITION, currentPos + vSlide);
    else 
        Set_TRANSFORM(T_POSITION, vAfterPos);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
    _vector vRight = Get_TRANSFORM(T_RIGHT);
    _vector vUp    = Get_TRANSFORM(T_UP);
    _vector vLook  = Get_TRANSFORM(T_LOOK);

    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

    _vector vNewUp = XMVector3TransformNormal(vUp, RotationMatrix);
    if (XMVectorGetY(vNewUp) <= 0)
        return;

    Set_TRANSFORM(T_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_TRANSFORM(T_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_TRANSFORM(T_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_bool bX, _bool bY, _bool bZ, _float fTimeDelta)
{
    _vector		vRight = Get_TRANSFORM(T_RIGHT);
    _vector		vUp    = Get_TRANSFORM(T_UP);
    _vector		vLook  = Get_TRANSFORM(T_LOOK);

    _float		fRotationSpeed = m_fRotationPerSec * fTimeDelta;

    _vector		vQuaternion = XMQuaternionRotationRollPitchYaw(bX * fRotationSpeed, bY * fRotationSpeed, bZ * fRotationSpeed);

    _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

    Set_TRANSFORM(T_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_TRANSFORM(T_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_TRANSFORM(T_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}


void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
    _float3		vScaled = Get_Scaled();

    _vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
    _vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
    _vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

    _vector		vQuaternion = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

    _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

    Set_TRANSFORM(T_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_TRANSFORM(T_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_TRANSFORM(T_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

HRESULT CTransform::Initialize_Prototype(void* pTransformDesc)
{    
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    if (nullptr != pTransformDesc)
    {
        TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pTransformDesc);

        m_fSpeedPerSec = pDesc->fSpeedPerSec;
        m_fRotationPerSec = pDesc->fRotationPerSec;
        m_JumpPower = pDesc->JumpPower;;

        XMStoreFloat4x4(&m_WorldMatrix, pDesc->WorldMatrix);
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
