#pragma once
#include "ComputeShader.h"

BEGIN(Engine)

class CTrail_Compute final : public CComputeShader
{
private:
    explicit CTrail_Compute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CTrail_Compute(const CTrail_Compute& rhs);
    virtual ~CTrail_Compute() = default;

public:
    virtual HRESULT Initialize_Proto() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    // 실제로 Compute Shader 실행하는 함수
    HRESULT Dispatch(const std::vector<_float3>& vLatestPositions, _float fDeltaTime);

public:
    // GPU 버퍼 설정용
    HRESULT Set_LatestPosBuffer(ID3D11Buffer* pBuffer, UINT iElementCount);
    HRESULT Set_Headers(ID3D11Buffer* pBuffer, UINT iElementCount);
    HRESULT Set_Points(ID3D11Buffer* pBuffer, UINT iElementCount);

    // 파라미터 세팅
    void Set_Params(_float fMaxLife, _float fSpawnDist, _float fSmooth);

private:
    // TrailUpdate용 구조체
    struct CB_TRAIL
    {
        _float g_DeltaTime;
        _float g_MaxLife;
        _float g_SpawnDist;
        _uint g_NumTrails;
        _float g_Smooth;
        _float3 pad;
    };

private:
    HRESULT Ready_ComputeShader();
    HRESULT Ready_ConstantBuffer();

private:
    // Constant Buffer
    ID3D11Buffer* m_pCB_Trail = nullptr;

    // 입력/출력 UAV, SRV
    ID3D11ShaderResourceView* m_pLatestPosSRV = nullptr;
    ID3D11UnorderedAccessView* m_pHeaderUAV = nullptr;
    ID3D11UnorderedAccessView* m_pPointUAV = nullptr;

    // 파라미터 값
    _float m_fMaxLife = 0.8f;
    _float m_fSpawnDist = 0.05f;
    _float m_fSmooth = 0.15f;
    _uint m_iNumTrails = 0;

public:
    static CTrail_Compute* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END
