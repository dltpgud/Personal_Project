#include "stdafx.h"
#include "ShootEffect.h"
#include "GameInstance.h"
#include "Weapon.h"
CShootEffect::CShootEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBlendObject { pDevice, pContext }
{
}

CShootEffect::CShootEffect(const CShootEffect& Prototype)
	: CBlendObject { Prototype }
{
}

HRESULT CShootEffect::Initialize_Prototype()
{
	/* 패킷, 파일입ㅇ출력을 통한 초기화. */

	return S_OK;
}

HRESULT CShootEffect::Initialize(void * pArg)
{
	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	CShootEffect_DESC* pDesc = static_cast<CShootEffect_DESC*>(pArg);

	pDesc->fSpeedPerSec = 5.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);
	m_vTget = pDesc->vTgetPos;
	m_vPos = pDesc->vPos;
	m_WorldPtr = pDesc->WorldPtr;
	m_Local = pDesc->Local;
	m_iWeaponType = pDesc->iWeaponType;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if(m_iWeaponType == CWeapon::HendGun)
	  m_pTransformCom->Set_Scaling(0.3f, 0.3f, 0.3f);
	else
	if (m_iWeaponType == CWeapon::AssaultRifle)
	  m_pTransformCom->Set_Scaling(0.3f, 0.3f, 0.3f);
	else
	 if (m_iWeaponType == CWeapon::MissileGatling)
	  m_pTransformCom->Set_Scaling(0.5f, 0.5f, 0.5f);
	else
    if (m_iWeaponType == CWeapon::HeavyCrossbow)
		m_pTransformCom->Set_Scaling(0.4f, 0.4f, 0.4f);

	m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, m_vPos);

	return S_OK;
}

_int CShootEffect::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;




	return OBJ_NOEVENT;
}

void CShootEffect::Update(_float fTimeDelta)
{
	__super::Compute_Depth();


	if (m_iWeaponType == CWeapon::HendGun)
	{
		m_RGB[0] = {0.f, 0.5f, 1.f, 0.5f};
		m_RGB[1] = { 0.f, 1.f, 1.f,1.f };


		m_fFrame += 50.f * fTimeDelta;

		if (m_fFrame >= 5.f)
			m_bDead = true;

	}

	if (m_iWeaponType == CWeapon::AssaultRifle)
	{
		m_RGB[0] = { 0.f, 0.5f, 1.f, 0.5f };
		m_RGB[1] = { 0.f, 1.f, 0.f,1.f };


		m_fFrame += 50.f * fTimeDelta;

		if (m_fFrame >= 5.f)
			m_bDead = true;

	}

	if (m_iWeaponType == CWeapon::MissileGatling)
	{
		m_RGB[0] = { 0.5f, 0.5f, 0.5f, 0.5f };
		m_RGB[1] = { 1.f, 1.f, 0.f,1.f };


		m_fFrame += 50.f * fTimeDelta;

		if (m_fFrame >= 5.f)
			m_bDead = true;

	}
	
	if (m_iWeaponType == CWeapon::HeavyCrossbow)
	{
		m_RGB[0] = { 0.f, 0.5f, 1.f, 0.5f };
		m_RGB[1] = { 0.f, 1.f, 1.f,1.f };


		m_fFrame += 50.f * fTimeDelta;

		if (m_fFrame >= 5.f)
			m_bDead = true;
	}
}

void CShootEffect::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->LookAt(m_vTget);

	_vector vHPos = XMVector3TransformCoord(m_Local, XMLoadFloat4x4(m_WorldPtr));

	m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, vHPos);
	
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_BLEND, this)))
		return;
}

HRESULT CShootEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(m_fFrame))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShootEffect::Add_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_spackEffect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CShootEffect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_RGB", &m_RGB[0], sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_RGBEnd", &m_RGB[1], sizeof(_float4))))
		return E_FAIL;
	return S_OK;
}

CShootEffect* CShootEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShootEffect*		pInstance = new CShootEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShootEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShootEffect::Clone(void * pArg)
{
	CShootEffect*		pInstance = new CShootEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CShootEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShootEffect::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
