#pragma once
#include "Base.h"

BEGIN(Engine)
class CBone :  public CBase
{
private:
	CBone();
	 ~CBone() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}
	_matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	_matrix Get_TransformationMatrix()
	{
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationFloat4x4Ptr() const {
		return &m_CombinedTransformationMatrix;
	}

	_int Get_ParentBoneIndex() {
		return m_iParentBoneIndex;
	}

public:
	void Update_CombinedTransformationMatrix(const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	void New_CombinedTransformationMatrix(_fmatrix NewTransformMatrix);

private:
	_char				m_szName[MAX_PATH] = {};

	_float4x4			m_TransformationMatrix = {};
	_float4x4			m_CombinedTransformationMatrix = {};

	_int				m_iParentBoneIndex = { -1 };

public:
	static CBone* Create(HANDLE& hFile);
	CBone* Clone();
	virtual void Free() override;
};
END;
