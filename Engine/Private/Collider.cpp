#include "Collider.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{

}

CCollider::CCollider(const CCollider & Prototype)
	: CComponent { Prototype }
{

}

HRESULT CCollider::Initialize_Prototype(TYPE eColliderType)
{
	m_eColliderType = eColliderType;




	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	//m_pBounding = CBounding_AABB::Create();


	return S_OK;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eColliderType)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();


}
