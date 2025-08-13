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
        T_RIGHT,
        T_UP,
        T_LOOK,
        T_POSITION,
        T_END
    };

    enum MOVE
    {
        GO,
        BACK,
        RIGHT,
        LEFT,
        UP,
        DOWN,
        M_END
    };
  
    typedef struct TRANSFORM_DESC
    {
        _float fSpeedPerSec{};
        _float fRotationPerSec{};
        _matrix WorldMatrix = XMMatrixIdentity();
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
    void Go_Move(MOVE MoveType , _float fTimeDelta, class CNavigation* pNavigation = nullptr, _bool Demage = false);
    void Go_jump(_float fTimeDelta , _float YPos, _bool* Jumpcheck,_int* isFall, class CNavigation* pNavigation = nullptr);
    void CTransform::StartJump()
    {
        m_fJumpVelocity = m_JumpPower;
        m_bIsLanding = false; // 점프 시작 시 착지 상태 초기화
    }

    void Reset_DoubleJumpTime()
    {
        if (m_bCanDoubleJump)
        {
            m_fJumpVelocity = m_JumpPower * 0.9f;
            m_bCanDoubleJump = false;
            m_bIsLanding = false; // 더블 점프 시작 시 착지 상태 초기화
        }
    }
    void GO_Dir(_float fTimeDelta,  _vector vDir, CNavigation* pNavigation = nullptr, _bool* bStop = nullptr);
    void Go_jump_Dir(_float fTimeDelta, _vector Dir, _float YPos, CNavigation* pNavigation = nullptr, _bool* bStop = nullptr);
    void Stop_Move();
    void Rotation_to_Player(_float fTimeDelta);
    void Set_Rotation_to_Player();
    _bool FollowPath(CNavigation* pNavigation, _float fTimedelta);
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

    void Set_WorldMatrix(_fmatrix mat)
    {
        return XMStoreFloat4x4(&m_WorldMatrix,mat);
    }

    _float3 Get_Scaled()
    {
        return _float3(XMVectorGetX(XMVector3Length(Get_TRANSFORM(T_RIGHT))),
                       XMVectorGetX(XMVector3Length(Get_TRANSFORM(T_UP))),
                       XMVectorGetX(XMVector3Length(Get_TRANSFORM(T_LOOK))));
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

private:
    _float m_fGravity = 9.8f;
    _float4x4 m_WorldMatrix = {};
    _float m_fSpeedPerSec = {};
    _float m_fRotationPerSec = {};

    _bool m_bCanDoubleJump{true};
    _float m_JumpPower{};
    _float m_fJumpVelocity = 0.f;
    _bool m_bIsLanding = false; // 착지 중인지 확인
  
    _int m_CurrentPathIndex{0}; // 경로 인덱스 초기화
 public:
    static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pTransformDesc);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};
END
