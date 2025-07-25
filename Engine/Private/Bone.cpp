#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(HANDLE& hFile)
{
     DWORD dwByte = { 0 };
    _bool bReadFile;

	void* pInstance = malloc(sizeof (CBone));
    bReadFile = ReadFile(hFile, pInstance, sizeof(CBone), &dwByte, nullptr);

    CBone* pData = reinterpret_cast<CBone*>(pInstance);

    if (!bReadFile || dwByte != sizeof(CBone) || pData == nullptr)
    {
        free(pInstance);
        return E_FAIL;
    }
 
    memcpy(m_szName, pData->m_szName, sizeof(m_szName));
    memcpy(&m_TransformationMatrix, &pData->m_TransformationMatrix, sizeof(_float4x4));
    memcpy(&m_CombinedTransformationMatrix, &pData->m_CombinedTransformationMatrix, sizeof(_float4x4));

    m_iParentBoneIndex = pData->m_iParentBoneIndex;
	free(pInstance);
    return S_OK;
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

void CBone::New_CombinedTransformationMatrix(_fmatrix NewTransformMatrix)
{
	XMStoreFloat4x4(&m_CombinedTransformationMatrix,
		NewTransformMatrix * XMLoadFloat4x4(&m_CombinedTransformationMatrix));
}

CBone* CBone::Create(HANDLE& hFile)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize( hFile)))
    {
        MSG_BOX("Failed to Created : CBone");
        Safe_Release(pInstance);
    }
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
