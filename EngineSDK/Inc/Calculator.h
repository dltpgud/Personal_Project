#pragma once
#include "Base.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CTransform;
class CNavigation;
class CVIBuffer;
class CModel;
class ENGINE_DLL CCalculator final : public CBase
{
public:
private:
    CCalculator(){};
    CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CCalculator() = default;

public:
    void    Make_Ray( _matrix Proj, _matrix view, _vector* RayPos, _vector* RayDir, _bool forPlayer = false);

    _float3 Picking_OnTerrain(CVIBuffer_Terrain* pTerrainBufferCom, _vector RayPos, _vector RayDir,
                              CTransform* Transform, OUT _float* fDis, OUT _float3* vNormal);

    HRESULT Initialize(HWND hWnd, _uint iViewportWidth, _uint iViewportHeight);

    _vector PointNomal(_float3 fP1, _float3 fP2, _float3 fP3);
    _float  Compute_Random_Normal();
    _float  Compute_Random(_float fMin, _float fMax);
    _bool RayIntersectsAABB_Local(_vector rayO_L, _vector rayD_L, const _float3& mn, const _float3& mx);


private:
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};
    _uint							m_iViewportWidth{}, m_iViewportHeight{};
    class CGameInstance* m_pGameInstance = { nullptr };
    HWND g_hWnd;
public:
    static CCalculator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iViewportWidth, _uint iViewportHeight);
    virtual void Free() override;
};
END
