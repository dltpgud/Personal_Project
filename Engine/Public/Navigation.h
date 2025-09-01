#pragma once
#include "Component.h"

BEGIN(Engine)

struct AStarNode
{
    _uint index;
    _float gCost; 
    _float hCost; 
    _float fCost() const
    {
        return gCost + hCost;
    }
    _uint parent;
    _bool operator>(const AStarNode& other) const
    {
        return fCost() > other.fCost();
    }
};

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct NAVIGATION_DESC
	{
		_int	iCurrentCellIndex = { -1 };
	}NAVIGATION_DESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

	void            Update(const _float4x4* pWorldMatrix) {m_WorldMatrix = pWorldMatrix;}
#ifdef _DEBUG
	virtual HRESULT Render();
#endif

public:
    void           SetUp_Neighbor();
    _bool          Snap(_fvector vP1, _fvector vP2, _vector distance);
    void           Create_Poly(_float3 p1, _float3 p2, _float3 p3, _uint Type = 0);

    _bool          isMove(_fvector vAfterMoveWorldPos, _fvector vBeforeMoveWorldPos, _vector* Slide =nullptr);
    _float         Compute_HeightOnCell(_float3* fPos);
	void           Set_Taget(_vector Taget);
	vector<_uint>  FindPath(_uint startIndex, _uint goalIndex);
    _vector        Get_TagetPos(_int index);
    vector<_uint>  Get_PathPoints(){return m_PathPoints;};

    _bool          Get_bFalling();
    _bool          Get_bDemage(_int& HP);
    void           Set_Type(_uint Type);
    void           Find_CurrentCell(_vector vWorldPos);
    _vector        Get_SafePos();
                  
	HRESULT        Save(const _tchar* tFPath);
	HRESULT        Load(const _tchar* tFPath);
                   
	HRESULT        Delete_ALLCell();
	void           Delete_Cell(_vector LocalRayPos, _vector LocalRayDir);
     
 private:
	 int           Find_Cell_ByPosition(_vector vTargetPos);

 private:

     USE_LOCK;
  
     vector<_uint>              m_PathPoints;
	 _int					    m_iCurrentCellIndex = { -1 };
	 vector<class CCell*>	    m_Cells;
     vector<_uint>              m_vecNomoveType;
	 static	const _float4x4*	m_WorldMatrix ;  
	 _vector			        m_vSafePos{};
   

#ifdef _DEBUG
private:
	class CShader*				m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END