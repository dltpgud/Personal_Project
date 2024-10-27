#include "Collider.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

#include "GameInstance.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
	, m_eColliderType{ Prototype.m_eColliderType }
#ifdef _DEBUG
	, m_pBatch{ Prototype.m_pBatch }
	, m_pEffect{ Prototype.m_pEffect }
	, m_pInputLayout{ Prototype.m_pInputLayout }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eColliderType)
{
	m_eColliderType = eColliderType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t			iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout);

#endif




	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	const CBounding::BOUND_DESC* pDesc = static_cast<const CBounding::BOUND_DESC*>(pArg);

	switch (m_eColliderType)
	{
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(pDesc);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(pDesc);
		break;
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(pDesc);
		break;
	}

	if (nullptr == m_pBounding)
		return E_FAIL;

	return S_OK;
}



_bool CCollider::Intersect(CCollider* pTargetCollider)
{

	return m_isColl = m_pBounding->Intersect(pTargetCollider->m_eColliderType, pTargetCollider->m_pBounding);
}

_bool CCollider::RayIntersects(_vector RayPos, _vector RayDir, _float& fDis)
{


	m_isColl = m_pBounding->RayIntersect(RayPos, RayDir, fDis);

	return m_isColl;
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	if (nullptr == m_pBounding ||
		nullptr == m_pEffect)
		return E_FAIL;

	// 쉐이더에 행렬 값 던져 주고
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	// 정점들을 장치에 바인딩해준다.
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch, false == m_isColl ? XMVectorSet(0.f, 0.f, 1.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));
	
	m_pBatch->End();



	return S_OK;
}

#endif

void CCollider::Update(_fmatrix WorldMatrix)
{
	m_pBounding->Update(WorldMatrix);
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

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

	Safe_Release(m_pBounding);

#ifdef _DEBUG
	if (false == m_bClone)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);

#endif
}
