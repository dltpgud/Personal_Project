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

	DWORD dwByte = { 0 };
	_bool bReadFile;

//	bReadFile = ReadFile(hFile, pInstance, sizeof(CBone), &dwByte, nullptr);



bReadFile =	ReadFile(hFile, pInstance->m_szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
bReadFile =	ReadFile(hFile, &pInstance->m_iParentBoneIndex, sizeof(_int), &dwByte, nullptr);
bReadFile =	ReadFile(hFile, &pInstance->m_TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);
bReadFile =	ReadFile(hFile, &pInstance->m_CombinedTransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);
	
	if (!bReadFile)
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	//CBone temp{};
	//memcpy(pInstance->m_szName, temp.m_szName, MAX_PATH);
	//memcpy(&pInstance->m_TransformationMatrix, &temp.m_TransformationMatrix, sizeof m_TransformationMatrix);
	//memcpy(&pInstance->m_CombinedTransformationMatrix, &temp.m_CombinedTransformationMatrix, sizeof m_CombinedTransformationMatrix);
	//memcpy(&pInstance->m_iParentBoneIndex, &temp.m_iParentBoneIndex, sizeof m_iParentBoneIndex);

 	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}
