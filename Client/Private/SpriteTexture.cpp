#include "stdafx.h"
#include "SpriteTexture.h"
#include "GameInstance.h"
#include "Transform.h"
CSpriteTexture::CSpriteTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{ pDevice, pContext }
{
}

CSpriteTexture::CSpriteTexture(const CSpriteTexture& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CSpriteTexture::Initialize_Prototype()
{
	/* 패킷, 파일 입출력을 통한 초기화. */

	return  S_OK;
}

HRESULT CSpriteTexture::Initialize(void* pArg)
{
	CSpriteTexture_DESC* Desc = static_cast <CSpriteTexture_DESC*>(pArg) ;

	m_TexIndex = Desc->TexIndex;
	m_wKey = Desc->FilePath;
	m_interver = Desc->interver;
	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_TRANSFORM(
		CTransform::TRANSFORM_POSITION,
		XMVectorSet(m_fX - ViewportDesc.Width * 0.5f, -m_fY + ViewportDesc.Height * 0.5f, m_fZ, 1.f));

	m_index = 0;
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

_int CSpriteTexture::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CSpriteTexture::Update(_float fTimeDelta)
{
	m_fTick += fTimeDelta;

	if (m_TexIndex != 1) {
		if (m_fTick >= m_interver) {
			if (m_index < m_TexIndex)
			{
				m_index++;
				m_fTick = 0.f;
				if (m_index == m_TexIndex)
					m_index = 0;
			}
		}
	}
	else if(m_TexIndex ==1 )
		m_index = 0;

}

void CSpriteTexture::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CSpriteTexture::Render()
{

		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;


		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_index)))
			return E_FAIL;
	
		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Bind_Buffers();

		m_pVIBufferCom->Render();
	

	return S_OK;
}

HRESULT CSpriteTexture::Initialize_GORGE()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_wKey,
	TEXT("Com_Texture_Load_Canyon"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}






HRESULT CSpriteTexture::Add_Components()
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
	switch (m_pLoadingID)
	{
	case GORGE :
		hr = Initialize_GORGE();
		break;
	}


	return hr;

}

CSpriteTexture* CSpriteTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpriteTexture* pInstance = new CSpriteTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpriteTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpriteTexture::Clone(void* pArg)
{
	CSpriteTexture* pInstance = new CSpriteTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CSpriteTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpriteTexture::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
