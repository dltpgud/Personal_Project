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
}

void CPartObject::Late_Update(_float fTimeDelta)
{
	
}

HRESULT CPartObject::Render()
{
	return S_OK;
}

void CPartObject::Free()
{
	__super::Free();


}
