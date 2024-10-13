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
    _bool isMove(_fvector vWorldPos);
#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif

	_bool Snap(_fvector vP1, _fvector vP2, _vector distance);
	_vector PointNomal(_float3 fP1, _float3 fP2, _float3 fP3);
	void Create_Poly(_float3 p1, _float3 p2, _float3 p3);


	HRESULT Save(const _tchar* tFPath);
	HRESULT Load(const _tchar* tFPath);
	HRESULT Delete_ALLCell();
	void Delete_Cell(_vector LocalRayPos, _vector LocalRayDir);
private:
	_int					m_iCurrentCellIndex = { -1 };
	vector<class CCell*>	m_Cells;
	static	const _float4x4*				m_WorldMatrix ;  // ���� ������ ������ ���� �� ���� ��ǥ�� �������� ��� ���� ��ü�� (����, �÷��̾�, ���)�� ����ؾ��ϱ� ����

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