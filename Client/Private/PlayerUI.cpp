#include "stdafx.h"
#include "PlayerUI.h"
#include "GameInstance.h"
CPlayerUI::CPlayerUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext}
{
}

CPlayerUI::CPlayerUI(const CPlayerUI& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CPlayerUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerUI::Initialize(void* pArg)
{
	CUI_DESC			Desc{};

	Desc.fX = g_iWinSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;
	Desc.UID = CUI::UIID_Player;
	Desc.PrUpdate = true;
	Desc.Update = true;
	Desc.LateUpdate = true;
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(500.f, 100.f, 1.f);
	m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(175.f - g_iWinSizeX * 0.5f, -640.f + g_iWinSizeY * 0.5f, 0.f, 1.f));

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

_int CPlayerUI::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CPlayerUI::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_F8) & 0x8000)
	{
		m_HP -= 10.f;
		m_HP_Pluse = m_HP / m_MaxHP + 0.1f;
	}
	if (GetAsyncKeyState(VK_F7) & 0x8000)
	{
			m_HP = m_HP + m_HP / m_MaxHP + 0.01f;
	}

}

void CPlayerUI::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CPlayerUI::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float("g_hp", m_HP/ m_MaxHP)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Float("g_hp_pluse", m_HP_Pluse)))
		return E_FAIL;
;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom_HpBar->Bind_ShaderResource(m_pShaderCom, "g_Texture0", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom_HpBar->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 1)))
		return E_FAIL;
	if (FAILED(m_pTextureCom_HpBar->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 2)))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();



	return S_OK;
}

HRESULT CPlayerUI::Add_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_HpBar"),
		TEXT("Com_Texture_PlayerHpBar"), reinterpret_cast<CComponent**>(&m_pTextureCom_HpBar)))) 
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

CPlayerUI* CPlayerUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerUI* pInstance = new CPlayerUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerUI::Clone(void* pArg)
{
	CPlayerUI* pInstance = new CPlayerUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayerUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerUI::Free()
{

	__super::Free();


	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pTextureCom_HpBar);

	Safe_Release(m_pShaderCom);


}
