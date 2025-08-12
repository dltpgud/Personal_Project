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

    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(0.f, 2.f, 0.f, 1.f));
	m_fHP_Pluse = m_fHP * 1.3f / m_fMaxHP;
	m_fRatio = m_fHP * 0.01f / m_fMaxHP;
	return S_OK;
}

void CMonsterHP::Priority_Update(_float fTimeDelta)
{
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CMonsterHP::Update(_float fTimeDelta)
{
   if (false == m_bStart)
        return;

   if (m_fHP_Pluse != m_fHP / m_fMaxHP)
   {
   	 m_fHP_Pluse -= m_fRatio;
   
   	if (m_fHP_Pluse <= m_fHP / m_fMaxHP)
   	{
   		m_fHP_Pluse = (m_fHP / m_fMaxHP);
   	}
 
   	m_fCurRatio = m_fHP / m_fMaxHP;
   }
}

void CMonsterHP::Late_Update(_float fTimeDelta)
{
    if (false == m_bStart)
        return;
    
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CMonsterHP::Render()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof _float4)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	    return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_PSize", &m_fPsize, sizeof _float2)))
        return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

#pragma region HP_Bar_BackGround
        if (FAILED(m_pShaderCom->Bind_RawValue("g_Hit", &m_bStart, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(3);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

#pragma endregion
     if (FAILED(m_pShaderCom->Bind_RawValue("g_hp", &m_fCurRatio, sizeof(_float))))
	 	return E_FAIL;

     if (FAILED(m_pShaderCom->Bind_RawValue("g_hp_pluse", &m_fHP_Pluse, sizeof(_float))))
	 	return E_FAIL;
	 
	 if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
	 	return E_FAIL;
	 if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
	 	return E_FAIL;
	 
	 m_pShaderCom->Begin(4);
	 
	 m_pVIBufferCom->Bind_Buffers();
	 
	 m_pVIBufferCom->Render();
	
	return S_OK;
}

HRESULT CMonsterHP::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Point"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

 	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBufferPoint"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
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
