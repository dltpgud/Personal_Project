#include "stdafx.h"
#include "Aim.h"
#include "GameInstance.h"
#include "Transform.h"
CAim::CAim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{ pDevice, pContext }
{
}

CAim::CAim(const CAim& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CAim::Initialize_Prototype()
{
	/* 패킷, 파일 입출력을 통한 초기화. */

	return  S_OK;
}

HRESULT CAim::Initialize(void* pArg)
{
	CAim_DESC* Desc = static_cast <CAim_DESC*>(pArg) ;

	
	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_TRANSFORM(
		CTransform::T_POSITION,
		XMVectorSet(m_fX - ViewportDesc.Width * 0.5f, -m_fY + ViewportDesc.Height * 0.5f, m_fZ, 1.f));

	
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bshakingX = Desc->bshakingX;
	m_bshakingY = Desc->bshakingY;
	m_positive_OR_negative = Desc->fpositive_OR_negative;
	m_fPrXPos = m_fX;
	m_fPrYPos = m_fY;
	return S_OK;
}

void CAim::Priority_Update(_float fTimeDelta)
{

}

void CAim::Update(_float fTimeDelta)
{
	if (m_IsShaking) {
		if (m_fShakingTime > 0.f)
		{
			if(m_bshakingX)
			 m_fX += m_fShaking_X * m_positive_OR_negative;
			else
			if(m_bshakingY)
			 m_fY += m_fShaking_Y * m_positive_OR_negative;

		}
		if (m_fShakingTime <= 0.f)
		{
			m_fX = m_fPrXPos;
			m_fY = m_fPrYPos;
		}
	}
}

void CAim::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CAim::Render()
{
	__super::Render();



		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;


		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	
		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Bind_Buffers();

		m_pVIBufferCom->Render();
	

	return S_OK;
}

HRESULT CAim::Add_Components()
{
	HRESULT hr = {};

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
 	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferRoop"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Player_Aim"),
		TEXT("Com_Aim"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

		return S_OK;
}

CAim* CAim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAim* pInstance = new CAim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAim::Clone(void* pArg)
{
	CAim* pInstance = new CAim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CAim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAim::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
