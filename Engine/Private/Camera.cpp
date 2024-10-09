#include "Camera.h"
#include "Transform.h"
#include "GameInstance.h"


CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCamera::CCamera(const CCamera& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

	m_fFovy = pDesc->fFovy;
	m_fAspect = pDesc->fAspect;
	m_fNearZ = pDesc->fNearZ;
	m_fFarZ = pDesc->fFarZ;

	return S_OK;
}

_int CCamera::Priority_Update(_float fTimeDelta)
{
	m_pGameInstance->Set_TransformMatrix(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());

	m_pGameInstance->Set_TransformMatrix(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNearZ, m_fFarZ));

	return OBJ_NOEVENT;
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

CGameObject* CCamera::Clone(void* pArg)
{
	return nullptr;
}

void CCamera::Free()
{
	__super::Free();
}
