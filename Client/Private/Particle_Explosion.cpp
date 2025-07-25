#include "stdafx.h"
#include "Particle_Explosion.h"

#include "GameInstance.h"

CParticle_Explosion::CParticle_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject  { pDevice, pContext }
{
}

CParticle_Explosion::CParticle_Explosion(const CParticle_Explosion & Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticle_Explosion::Initialize_Prototype()
{
	/* 패킷, 파일입ㅇ출력을 통한 초기화. */

	return S_OK;
}

HRESULT CParticle_Explosion::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	CParticle_Explosion_Desc* pDesc = static_cast<CParticle_Explosion_Desc*>(pArg);

	m_pParentMatrix = pDesc->pParentMatrix;

	if (FAILED(Add_Components()))
		return E_FAIL;


	m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(0.f, 3.f, 0.f, 1.f));
	return S_OK;
}

void CParticle_Explosion::Priority_Update(_float fTimeDelta)
{

	m_fTimeSum += fTimeDelta;  
	
	if (m_fTimeSum >= 0.1f) 
	{
		m_fTimeSum -= 0.1f; 
		m_currentFrame = (m_currentFrame + 1) % 8; 
	}

}

void CParticle_Explosion::Update(_float fTimeDelta)
{

	m_pVIBufferCom->Spread(fTimeDelta);
}

void CParticle_Explosion::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLOOM, this)))
		return;


	XMStoreFloat4x4(&m_NewWordMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentMatrix);
}

HRESULT CParticle_Explosion::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Explosion::Add_Components()
{
	/* 멤버변수로 직접 참조를 하게되면 */
	/* 1. 내가 내 컴포넌트를 이용하고자할 때 굳이 검색이 필요없이 특정 멤버변수로 바로 기능을 이용하면 된다. */
	/* 2. 다른 객체가 내 컴포넌트를 검색하고자 할때 스위치케이스가 겁나 늘어나는 상황. */

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Proto Component Fire_Explostion"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Shader_Particle_Point"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;



	return S_OK;
}

HRESULT CParticle_Explosion::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_NewWordMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_textureSize",&m_textureSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_frameSize", &m_frameSize, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_framesPerRow", &m_framesPerRow, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_currentFrame", &m_currentFrame, sizeof(_int))))
		return E_FAIL;
	





	return S_OK;
}

CParticle_Explosion * CParticle_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle_Explosion*		pInstance = new CParticle_Explosion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle_Explosion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticle_Explosion::Clone(void * pArg)
{
	CParticle_Explosion*		pInstance = new CParticle_Explosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CParticle_Explosion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Explosion::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
