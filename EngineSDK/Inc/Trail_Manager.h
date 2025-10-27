#pragma once
#include "Base.h"
#include "Shader.h"
#include "Trail_Compute.h"
#include "VIBuffer_Point.h"

BEGIN(Engine)

class CTrail_Manager final : public CBase
{
private:
    explicit CTrail_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CTrail_Manager() = default;

public:
    HRESULT Initialize(_uint iMaxTrailCount);
    void Update(const vector<_float3>& vPositions, _float fDeltaTime);
    void Render();

private:
    HRESULT Create_TrailBuffers();
    HRESULT Create_RenderShader();
    HRESULT Create_ComputeSystem();

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    class CGameInstance* m_pGameInstance = nullptr;

    _uint m_iMaxTrailCount = 0;

    // ====== Render ======
    CShader* m_pShaderCom = nullptr;
    CVIBuffer_Point* m_pPointBuffer = nullptr;

    // ====== Compute ======
    CTrail_Compute* m_pTrailCompute = nullptr;

    // ====== GPU 버퍼 ======
    ID3D11Buffer* m_pTrailHeaders = nullptr;
    ID3D11Buffer* m_pTrailPoints = nullptr;
    ID3D11Buffer* m_pLatestPos = nullptr;

    ID3D11ShaderResourceView* m_pSRV_TrailPoints = nullptr;
    ID3D11ShaderResourceView* m_pSRV_TrailHeaders = nullptr;

private:
    _float m_fSpawnDist = 0.05f;
    _float m_fMaxLife = 1.0f;
    _float m_fSmooth = 0.3f;

public:
    static CTrail_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iMaxTrailCount);
    virtual void Free() override;
};

END
