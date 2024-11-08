#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct NAVIGATION_DESC
	{
		_int			iCurrentCellIndex = { -1 };
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

	void Update(const _float4x4* pWorldMatrix) {
		m_WorldMatrix = pWorldMatrix;
	}
public:
        void SetUp_Neighbor();
    _bool isMove(_fvector vAfterMoveWorldPos, _fvector vBeforeMoveWorldPos, _vector* Slide =nullptr);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif


	_uint Get_CurrentCell_Type();
	_bool Snap(_fvector vP1, _fvector vP2, _vector distance);

	void Create_Poly(_float3 p1, _float3 p2, _float3 p3, _uint Type = 0);
	_float Compute_HeightOnCell(_float3* fPos);
	HRESULT Save(const _tchar* tFPath);
	HRESULT Load(const _tchar* tFPath);
	void Set_Type(_uint Type);
	void Set_Type_From_Ray(_vector LocalRayPos, _vector LocalRayDir,_uint Type = 0);
	HRESULT Delete_ALLCell();
	void Delete_Cell(_vector LocalRayPos, _vector LocalRayDir);

	_bool ISFall();
	void Find_CurrentCell(_vector vWorldPos);
	_vector Get_SafePos();
private:
	_int					m_iCurrentCellIndex = { -1 };
	vector<class CCell*>	m_Cells;
	static	const _float4x4*				m_WorldMatrix ;  // 전역 변수로 설정한 것은 이 월드 좌표를 지형위에 띄운 여러 객체들 (몬스터, 플레이어, 등등)이 사용해야하기 때문
	_int					m_iNonMoveCellIndex = { -1 };
	_int					m_iSafeCellIndex = { -1 };
	_vector					m_vSafePos{};
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