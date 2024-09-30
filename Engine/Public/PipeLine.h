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

    _matrix Get_TransformMatrix(TRANSFORM_STATE eState)
    {
        return XMLoadFloat4x4(&m_TransMatrix[eState]); // 연산용
    }

    const _float4* Get_CamPosition()
    {
        return &m_vCamPosition;
    }

public: /* Setter */
    void Set_TransformMatrix(TRANSFORM_STATE eState, _fmatrix TransformMatrix)
    {
        XMStoreFloat4x4(&m_TransMatrix[eState], TransformMatrix);
    }

public:
    HRESULT Update();

private:
    _float4x4 m_TransMatrix[D3DTS_END];
    _float4x4 m_TransMatrixInverse[D3DTS_END];
    _float4 m_vCamPosition;

public:
    static CPipeLine* Create();
    virtual void Free() override;
};
