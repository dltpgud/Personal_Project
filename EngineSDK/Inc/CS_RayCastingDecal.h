#pragma once
#include "ComputeShader.h"

BEGIN(Engine)
class CCS_RayCastingDecal final : public CComputeShader
{
public:
    struct RayCB
    {
        _float3 RayOrigin;
        _float _pad0;
        _float3 RayDir;
        _float _pad1; // 정규화해서 넣기
        _uint NumTris;
        _float3 _pad2;
        _float3 DecalSize;
        _float _pad3; // (width,height,thickness)
    };

protected:
    CCS_RayCastingDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCS_RayCastingDecal(const CCS_RayCastingDecal& Proto);
    virtual ~CCS_RayCastingDecal() = default;

public:
    virtual HRESULT Initialize_Proto();
    virtual HRESULT Initialize(void* pArg) override;

public:
    // 출력(SRV로 픽셀/버텍스 셰이더에서 읽을 것)
    ID3D11ShaderResourceView* GetWorldSRV() 
    {
        return m_pSRV_DecalWorld;
    }
    ID3D11ShaderResourceView* GetWorldInvSRV() 
    {
        return m_pSRV_DecalWorldInv;
    }
    virtual HRESULT Compute(UINT X, UINT Y = 1, UINT Z = 1) override;
    void SetMeshSRVs(ID3D11ShaderResourceView* posSRV, ID3D11ShaderResourceView* idxSRV)
    {
        m_pSRV_Pos = posSRV;
        m_pSRV_Idx = idxSRV;
    }

    // 레이/개수/크기 업데이트 & 실행
    void UpdateRayCB(const _float3& O, const _float3& D_norm, _uint numTris, const _float3& decalSize);

private:
    HRESULT CreateOutputs(); // World/WorldInv/MinT 생성
    HRESULT CreateStructuredUAVSRV(ID3D11Device* dev, UINT elemSize, UINT elemCount, ID3D11Buffer** ppBuf,
                                   ID3D11UnorderedAccessView** ppUAV, ID3D11ShaderResourceView** ppSRV);


private:
    // 상수버퍼
    ID3D11Buffer* m_pCB = nullptr;

    // 입력
    ID3D11ShaderResourceView* m_pSRV_Pos = nullptr; // t0
    ID3D11ShaderResourceView* m_pSRV_Idx = nullptr; // t1

    // 출력 버퍼(+UAV/SRV)
    ID3D11Buffer* m_pBuf_DecalWorld = nullptr;
    ID3D11UnorderedAccessView* m_pUAV_DecalWorld = nullptr;
    ID3D11ShaderResourceView* m_pSRV_DecalWorld = nullptr;

    ID3D11Buffer* m_pBuf_DecalWorldInv = nullptr;
    ID3D11UnorderedAccessView* m_pUAV_DecalWorldInv = nullptr;
    ID3D11ShaderResourceView* m_pSRV_DecalWorldInv = nullptr;

    ID3D11Buffer* m_pBuf_MinT = nullptr;
    ID3D11UnorderedAccessView* m_pUAV_MinT = nullptr;


public:
    static CCS_RayCastingDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};
END
