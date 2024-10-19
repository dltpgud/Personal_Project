#pragma once
#include "Base.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CTransform;
class CVIBuffer;
class CModel;
class ENGINE_DLL CCalculator final : public CBase
{
public:
private:
    CCalculator(){};
    CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
    virtual ~CCalculator() = default;

public:
    void Make_Ray( _matrix Proj, _matrix view, _vector* RayPos, _vector* RayDir);

    _float3 Picking_OnTerrain(HWND hWnd, CVIBuffer_Terrain* pTerrainBufferCom, _vector RayPos, _vector RaDir,
                              CTransform* Transfor, _float* fDis);

    _float Compute_Random_Normal();
    _float Compute_Random(_float fMin, _float fMax);
private:
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};
    HWND g_hWnd;
public:
    static CCalculator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
    virtual void Free() override;
};
END
