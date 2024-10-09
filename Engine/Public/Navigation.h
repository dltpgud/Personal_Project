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
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationFilePath);
	virtual HRESULT Initialize(void* pArg) override;

	void Update(const _float4x4* pWorldMatrix) {
		m_WorldMatrix = *pWorldMatrix;
	}
public:
        void SetUp_Neighbor();
    _bool isMove(_fvector vWorldPos);
#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif

private:
	_int					m_iCurrentCellIndex = { -1 };
	vector<class CCell*>	m_Cells;
	static	_float4x4				m_WorldMatrix ;  // ���� ������ ������ ���� �� ���� ��ǥ�� �������� ��� ���� ��ü�� (����, �÷��̾�, ���)�� ����ؾ��ϱ� ����

#ifdef _DEBUG
private:
	class CShader*				m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END