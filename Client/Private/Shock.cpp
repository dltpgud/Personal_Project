#include "stdafx.h"
#include "Shock.h"
#include "GameInstance.h"

CShock::CShock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CSkill{ pDevice, pContext }
{
}

CShock::CShock(const CShock& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CShock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShock::Initialize(void * pArg)
{

	if (nullptr != pArg) {
		CShock_DESC* pDesc = static_cast<CShock_DESC*>(pArg);

	
		if (FAILED(__super::Initialize(pDesc)))
			return E_FAIL;
	}
	else
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;


	m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSetY(m_vPos, 1.7f));
	m_fLifeTime = 10.f;
	m_fScaleSpeed = 20.f;
	m_fCurrentScale = 1.0f;
	return S_OK;
}

void CShock::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	return ;
}

void CShock::Update(_float fTimeDelta)
{

	m_fCurrentScale += m_fScaleSpeed * fTimeDelta;
	m_pTransformCom->Set_Scaling(m_fCurrentScale, 10.f, m_fCurrentScale);
}

void CShock::Late_Update(_float fTimeDelta)
{	
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
		return;

__super::Late_Update(fTimeDelta);
}

void CShock::Dead_Rutine(_float fTimeDelta)
{
	m_bDead = true;
}

HRESULT CShock::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
			if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
			"g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(5)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CShock::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Proto_Component_Shock"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	return S_OK;
}

HRESULT CShock::Bind_ShaderResources()
{
 	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
 		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

 	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
	 	return E_FAIL;

 	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_RGB", &m_RGB, sizeof(_float4))))
		return E_FAIL;
	return S_OK;
}

CShock* CShock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShock*		pInstance = new CShock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShock::Clone(void * pArg)
{
	CShock*		pInstance = new CShock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CShock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShock::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
