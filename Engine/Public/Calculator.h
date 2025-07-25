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

    _float3 Picking_OnTerrain(HWND hWnd, CVIBuffer_Terrain* pTerrainBufferCom, _vector RayPos, _vector RaDir,
                              CTransform* Transform, _float* fDis);

    HRESULT Initialize(HWND hWnd, _uint iViewportWidth, _uint iViewportHeight);

    HRESULT Compute_Y(CNavigation* pNavigation, CTransform* Transform, _float3* Pos);

    	_bool isComputeHeight(_fvector vTargetPos, _float3* pOut);

    _vector PointNomal(_float3 fP1, _float3 fP2, _float3 fP3);
    _float  Compute_Random_Normal();
    _float  Compute_Random(_float fMin, _float fMax);


    _bool isPicked(_float3* pOut, _bool IsPlayer);
private:
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};
    _uint							m_iViewportWidth{}, m_iViewportHeight{};
    /* 복사 받아오기위한 용도 */
    /* 락, 언락하면서 특정 픽셀의 정보를 얻어온다. */
    ID3D11Texture2D* m_pTexture2D = { nullptr };

    class CGameInstance* m_pGameInstance = { nullptr };


    HWND g_hWnd;
public:
    static CCalculator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iViewportWidth, _uint iViewportHeight);
    virtual void Free() override;
};
END
