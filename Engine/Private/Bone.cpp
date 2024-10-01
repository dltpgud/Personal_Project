#include "Bone.h"

CBone::CBone()
{
}

void CBone::Update_CombinedTransformationMatrix(const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (m_iParentBoneIndex == -1)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

		return;
	}

	XMStoreFloat4x4(&m_CombinedTransformationMatrix,
		XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
}

CBone* CBone::Create(HANDLE& hFile)
{
	CBone* pInstance = new CBone();
	CBone* pNew = nullptr;
	DWORD dwByte = { 0 };
	_bool bReadFile;

	bReadFile = ReadFile(hFile, pInstance, sizeof(CBone), &dwByte, nullptr);

	pNew = pInstance->Clone();

	delete static_cast<void*>(pInstance);
	
	if (!bReadFile)
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}



 	return pNew;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}
