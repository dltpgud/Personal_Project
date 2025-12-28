#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	void Set_HightMap(const _tchar* pHeightMapFilePath);
	void Set_Buffer(_int x, _int z);
        void DYNAMIC_Set_Buffer(_int x, _int z);
    void Set_QuadTree();
	_float3* Get_VtxPos() { return m_pPos; }
	_uint	 Get_NumVerticesX(){ return m_iNumVerticesX; }
	_uint    Get_NumVerticesZ() { return m_iNumVerticesZ; }

    AABB LocalAABB();
 


public:
     void Culling(_fmatrix WorldMatrixInverse);
    _bool Picking_OnTerrain_QuadTree(_vector RayPos, _vector RayDir, class CTransform* pTransform, OUT _float* fDist,
                                        OUT _float3* vNormal, OUT _float3* vWorldPos);


 private:
	_uint					m_iNumVerticesX = {};
	_uint					m_iNumVerticesZ = {};
	_float3*		        m_pPos          = {};
    _uint*                  m_pIndices      = {};
    class CQuadTree* m_pQuadTree = {nullptr};

   _float3 m_vMin {};
    _float3 m_vMax{};

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*    Clone(void* pArg) override;
	virtual void		   Free() override;
};

END
