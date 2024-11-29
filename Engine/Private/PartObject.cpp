#include "PartObject.h"

CPartObject::CPartObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CPartObject::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CPartObject::Initialize(void * pArg)
{
	PARTOBJECT_DESC*		pDesc = static_cast<PARTOBJECT_DESC*>(pArg);

	m_pParentMatrix = pDesc->pParentMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	
	
	return S_OK;
}

_int CPartObject::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CPartObject::Update(_float fTimeDelta)
{
	if (true == m_DyingTime)
	{
		m_DyTimeSum += fTimeDelta;

		if (m_DyTimeSum >= m_DyTime) {
			m_interver += fTimeDelta;
			if (m_interver > 1.f) {
				m_interver = 1.0f;  // 이미지가 완전히 나타나면 멈춤
			}
		}
	}

	__super::Update(fTimeDelta);
}

void CPartObject::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pParentMatrix) *
		m_pTransformCom->Get_WorldMatrix()); // 부모행렬과 내 월드랑 곱해서 그린다

	__super::Late_Update(fTimeDelta);
}

HRESULT CPartObject::Render()
{
	__super::Render();
	return S_OK;
}

const _float4x4* CPartObject::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrix(pBoneName);
}

void CPartObject::Set_BoneUpdateMatrix(const _char* pBoneName, _fmatrix NewMatrix)
{
	m_pModelCom->Set_BoneUpdateMatrix(pBoneName, NewMatrix);
}





void CPartObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
