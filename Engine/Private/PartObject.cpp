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

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;	
	
	return S_OK;
}

void CPartObject::Priority_Update(_float fTimeDelta)
{
	return ;
}

void CPartObject::Update(_float fTimeDelta)
{
	if (true == m_bDeadState)
	{
		m_fDeadTimeSum += fTimeDelta;

		if (m_fDeadTimeSum >= m_fDeadTime) {
			m_fthreshold += fTimeDelta;
			if (m_fthreshold > 1.f) {
				m_fthreshold = 1.0f;  
			}
		}
	}

	__super::Update(fTimeDelta);
}

void CPartObject::Late_Update(_float fTimeDelta)
{

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix)); 
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

void CPartObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
