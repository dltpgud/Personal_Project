#pragma once

#include "Base.h"

class CPipeLine final : public CBase
{
public:
    enum TRANSFORM_STATE
    {
        D3DTS_VIEW,
        D3DTS_PROJ,
        D3DTS_END
    };

private:
    CPipeLine();
    virtual ~CPipeLine() = default;

public:
    const _float4x4* Get_TransformFloat4x4(TRANSFORM_STATE eState)
    {
        return &m_TransMatrix[eState]; // 저장용
    }

    const _float4x4* Get_TransformFloat4x4_Inverse(TRANSFORM_STATE eState) {
        return &m_TransMatrixInverse[eState];
    }
    const _float4x4* Get_ShadowTransformFloat4x4_Inverse(TRANSFORM_STATE eState)
    {
        return &m_ShadowTransMatrixInverse[eState];
    }
    _matrix Get_TransformMatrix_Inverse(TRANSFORM_STATE eState) {
        return XMLoadFloat4x4(&m_TransMatrixInverse[eState]);
    }

    _matrix Get_TransformMatrix(TRANSFORM_STATE eState)
    {
        return XMLoadFloat4x4(&m_TransMatrix[eState]); // 연산용
    }

    const _float4x4* Get_ShadowTransformFloat4x4(TRANSFORM_STATE eState)
    {
        return &m_ShadowTransMatrix[eState]; // 저장용
    }

    _matrix Get_ShadowTransformMatrix(TRANSFORM_STATE eState)
    {
        return XMLoadFloat4x4(&m_ShadowTransMatrix[eState]); // 연산용
    }

    void Set_Camfar(_float fFar)
    {
        m_vCamfar = fFar;
    }

    const _float* Get_Camfar()
    {
        return &m_vCamfar;
    }

    const _float4* Get_CamPosition()
    {
        return &m_vCamPosition;
    }

    const _float4* Get_CamLook()
    {
        return &m_vCamLook;
    }

public: /* Setter */
    void Set_TransformMatrix(TRANSFORM_STATE eState, _fmatrix TransformMatrix)
    {
        XMStoreFloat4x4(&m_TransMatrix[eState], TransformMatrix);
    }

    void Set_ShadowTransformMatrix(TRANSFORM_STATE eState, _fmatrix TransformMatrix)
    {
        XMStoreFloat4x4(&m_ShadowTransMatrix[eState], TransformMatrix);
    }

public:
    HRESULT Update();

private:
    _float4x4 m_TransMatrix[D3DTS_END];
    _float4x4 m_TransMatrixInverse[D3DTS_END];
    _float4 m_vCamPosition{};
    _float4 m_vCamLook{};
    _float m_vCamfar{};
    _float4x4 m_ShadowTransMatrix[D3DTS_END];
    _float4x4 m_ShadowTransMatrixInverse[D3DTS_END];

public:
    static CPipeLine* Create();
    virtual void Free() override;
};
