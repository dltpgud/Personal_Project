#include "stdafx.h"
#include "MonsterHP.h"
#include "GameInstance.h"
#include "Transform.h"
CMonsterHP::CMonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{ pDevice, pContext }
{
}

CMonsterHP::CMonsterHP(const CMonsterHP& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CMonsterHP::Initialize_Prototype()
{
	/* 패킷, 파일 입출력을 통한 초기화. */

	return  S_OK;
}

HRESULT CMonsterHP::Initialize(void* pArg)
{
	CMonsterHP_DESC* Desc = static_cast <CMonsterHP_DESC*>(pArg) ;
	{
		m_fHP = Desc->fHP;
		m_fMaxHP = Desc->fMaxHP;
	}
	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	m_pTransformCom->Set_Scaling(2.f, 0.2f, 0.f);

	m_fHP_Pluse = m_fHP * 1.3f / m_fMaxHP;
	m_fRatio = m_fHP * 0.01f / m_fMaxHP;
	return S_OK;
}

_int CMonsterHP::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_pTransformCom->LookAt(m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION));

	return OBJ_NOEVENT;
}

void CMonsterHP::Update(_float fTimeDelta)
{

	if (true == m_bStart) {
		if (m_fHP_Pluse != m_fHP / m_fMaxHP)
		{
			m_fHP_Pluse -= m_fRatio;
	
			if (m_fHP_Pluse <= m_fHP / m_fMaxHP)
			{
				m_fHP_Pluse = (m_fHP / m_fMaxHP);
			}
		}
	}

	
}

void CMonsterHP::Late_Update(_float fTimeDelta)
{
	if (false == m_bLateUpdaet)
		return;
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CMonsterHP::Render()
{

#pragma region HP_Bar_BackGround
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Bool("g_Hit", m_bHit)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

#pragma endregion
		
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float("g_hp", m_fHP / m_fMaxHP)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Float("g_hp_pluse", m_fHP_Pluse)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Bool("g_Hit", m_bHit)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 1)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;
	
	
	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();
	

	return S_OK;
}



HRESULT CMonsterHP::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_Inverse"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
 	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferRoop"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterHP"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	


	return S_OK;

}

CMonsterHP* CMonsterHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterHP* pInstance = new CMonsterHP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterHP::Clone(void* pArg)
{
	CMonsterHP* pInstance = new CMonsterHP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMonsterHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterHP::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
