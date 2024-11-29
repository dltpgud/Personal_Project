#include "stdafx.h"
#include "ShockWave.h"
#include "GameInstance.h"

CShockWave::CShockWave(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CSkill{ pDevice, pContext }
{
}

CShockWave::CShockWave(const CShockWave& Prototype)
	: CSkill{ Prototype }
{
}

HRESULT CShockWave::Initialize_Prototype()
{
	/* 패킷, 파일입ㅇ출력을 통한 초기화. */

	return S_OK;
}

HRESULT CShockWave::Initialize(void * pArg)
{
	 CShockWave_DESC* pDesc = static_cast<CShockWave_DESC*>(pArg);
	 pDesc->fLifeTime = 10.f;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSetY (m_vPos, 1.7f));
	m_fScaleSpeed = 20.f;
	m_fCurrentScale = 1.f;
	return S_OK;
}

_int CShockWave::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	__super::Priority_Update(fTimeDelta);
	return OBJ_NOEVENT;
}

void CShockWave::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta*1.2f;
	m_fCurrentScale += m_fScaleSpeed * fTimeDelta;
	m_pTransformCom->Set_Scaling(m_fCurrentScale, 10.f, m_fCurrentScale);
	__super::Update(fTimeDelta);
}

void CShockWave::Late_Update(_float fTimeDelta)
{


	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONLIGHT, this)))
		return;


	if (FAILED(m_pGameInstance->Add_MonsterBullet(this)))
		return;
		__super::Late_Update(fTimeDelta);
}

void CShockWave::Dead_Rutine(_float fTimeDelta)
{
	m_bDead = true;
}

HRESULT CShockWave::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


		_uint iNumMesh = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMesh; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
				"g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(4)))
				return E_FAIL;

			m_pModelCom->Render(i);
		}
	
	return S_OK;
}

HRESULT CShockWave::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Proto_Component_ShockWave"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

 	CBounding_Sphere::BOUND_SPHERE_DESC		CBounding_Sphere{};
    _float3 Center{}, Extents{};
    CBounding_Sphere.fRadius = 0.39f;
 	CBounding_Sphere.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), 
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CBounding_Sphere))) 
		return E_FAIL;


	return S_OK;
}

HRESULT CShockWave::Bind_ShaderResources()
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


	if (FAILED(m_pShaderCom->Bind_RawValue("g_TimeSum", &m_fTime, sizeof(_float))))
		return E_FAIL;

	
	return S_OK;
}

CShockWave* CShockWave::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShockWave*		pInstance = new CShockWave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShockWave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShockWave::Clone(void * pArg)
{
	CShockWave*		pInstance = new CShockWave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CShockWave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShockWave::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);

}
