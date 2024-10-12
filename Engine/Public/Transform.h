#pragma once

#include "Component.h"
/* 월드 공간에서의 객체의 상태를 표현하기위한 행렬. */
/* 표현 : 월드행렬을 들고 있음으로서 월드공간에서의 right, up, look, position을 저장하고 있는 기능. */
/* 이 벡터들을 이용해서 월드공간에서의 상태 변환을 수행하는 기능. */

BEGIN(Engine)
class ENGINE_DLL CTransform final : public CComponent
{
public:
    enum TRANSFORM
    {
        TRANSFORM_RIGHT,
        TRANSFORM_UP,
        TRANSFORM_LOOK,
        TRANSFORM_POSITION,
        TRANSFORM_END
    };
    typedef struct TRANSFORM_DESC
    {
        _float fSpeedPerSec{};
        _float fRotationPerSec{};
        _vector RIGHT{};
        _vector UP{};
        _vector LOOK{};
        _vector POSITION{};
        _float JumpPower{};
    } TRANSFORM_DESC;

private:
    CTransform(){};
    CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CTransform() = default;

public:
    void Set_TRANSFORM(TRANSFORM eTRANSFORM, _fvector vState)
    { /*XMStore- 보관하는 함수*/
        XMStoreFloat4((_float4*)&m_WorldMatrix.m[eTRANSFORM][0], vState);
    }
    void Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);
    void LookAt(_fvector vAt);
    void Go_Straight(_float fTimeDelta);
    void Go_Left(_float fTimeDelta);
    void Go_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
    void Go_Backward(_float fTimeDelta);
    void Go_Up(_float fTimeDelta);
    void Go_Down(_float fTimeDelta);
    void Go_jump(_float fTimeDelta , _float YPos, _bool* Jumpcheck);
    void Stop_Move();
    void Rotation_to_Player();

    /* 현재 상태를 기준으로 추가로 더 회전한다. */
    void Turn(_fvector vAxis, _float fTimeDelta);
    void Turn(_bool bX, _bool bY, _bool bZ, _float fTimeDelta);

    /* 항등회전 상태를 기준으로 지정한 각도만큼 회전한다. */
    void Rotation(_float fX, _float fY, _float fZ);
public:
    _vector Get_TRANSFORM(TRANSFORM eTRANSFORM)
    {
        /*XMLoad- 저장용행렬을 연산용으로*/
        return XMLoadFloat4x4(&m_WorldMatrix).r[eTRANSFORM];
    }
    _matrix Get_WorldMatrix_Inverse()
    {
        return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
    }

     _float4x4* Get_WorldMatrixPtr()  
     {
        return &m_WorldMatrix;
     }

    _matrix Get_WorldMatrix()
    {
        return XMLoadFloat4x4(&m_WorldMatrix);
    }
    _float3 Get_Scaled()
    {
        return _float3(XMVectorGetX(XMVector3Length(Get_TRANSFORM(
                           TRANSFORM_RIGHT))), // 길이는 _vector을 리턴하고 _float 타입을 리턴함으로 get한다
                       XMVectorGetX(XMVector3Length(Get_TRANSFORM(TRANSFORM_UP))),
                       XMVectorGetX(XMVector3Length(Get_TRANSFORM(TRANSFORM_LOOK))));
    }

    void Set_MoveSpeed(_float Speed)
    {
        m_fSpeedPerSec = Speed;
    }
    void Set_RotSpeed(_float Speed)
    {
        m_fRotationPerSec = Speed;
    }

    _float Get_MoveSpeed()
    {
        return m_fSpeedPerSec;
    }
    _float Get_RotSpeed()
    {
        return m_fRotationPerSec;
    }

    _float Get_JumpPower()
    {
        return m_JumpPower;
    }
public:
    HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public:
    virtual HRESULT Initialize_Prototype(void* pTransformDesc);
    virtual HRESULT Initialize(void* pArg) override;

private:
    _float4x4 m_WorldMatrix = {};
    _float m_fSpeedPerSec = {};
    _float m_fRotationPerSec = {};

    _float m_JumpPower{};
    _float m_fTimeSum{};
public:
    static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pTransformDesc);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};
END
