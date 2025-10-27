#include "stdafx.h"
#include "Particle_Fog.h"

#include "GameInstance.h"

CParticle_Fog::CParticle_Fog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject  { pDevice, pContext }
{
}

CParticle_Fog::CParticle_Fog(const CParticle_Fog& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticle_Fog::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Fog::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CCParticle_FogDESC* pDesc = static_cast<CCParticle_FogDESC*>(pArg);

	m_pParentMatrix = pDesc->pParentMatrix;

	m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(0.f, 1.f, 0.f, 1.f));
	return S_OK;
}

void CParticle_Fog::Priority_Update(_float fTimeDelta)
{


}

void CParticle_Fog::Update(_float fTimeDelta)
{
	m_pVIBufferCom->Spread(fTimeDelta);
}

void CParticle_Fog::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONLIGHT, this)))
		return;
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
		return;


	XMStoreFloat4x4(&m_NewWordMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentMatrix);
}

HRESULT CParticle_Fog::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Fog::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Shader_Particle_Point"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Fog"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Fog::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_NewWordMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_FogMaskTex", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("Time", m_pGameInstance->Get_TimeDeltaSum(TEXT("Timer_60")), sizeof(_float))))
		return E_FAIL;
	
	return S_OK;
}

CParticle_Fog* CParticle_Fog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Fog* pInstance = new CParticle_Fog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle_Fog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_Fog::Clone(void* pArg)
{
    CParticle_Fog* pInstance = new CParticle_Fog(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CParticle_Explosion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Fog::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
