#include "stdafx.h"
#include "Loading.h"
#include "GameInstance.h"
#include "Transform.h"
CLoading::CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{ pDevice, pContext }
{
}

CLoading::CLoading(const CLoading& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CLoading::Initialize_Prototype()
{
	/* 패킷, 파일 입출력을 통한 초기화. */

	return  S_OK;
}

HRESULT CLoading::Initialize(void* pArg)
{
	CLoading_DESC* Desc = static_cast <CLoading_DESC*>(pArg) ;

	m_iTexIndex = Desc->TexIndex;
	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_TRANSFORM(
		CTransform::TRANSFORM_POSITION,
		XMVectorSet(m_fX - ViewportDesc.Width * 0.5f, -m_fY + ViewportDesc.Height * 0.5f, m_fZ, 1.f));

	m_fFirstX = m_fX;
	if (FAILED(Add_Components()))
		return E_FAIL;



	if (FAILED(Set_LoadPos(Desc)))
		return E_FAIL;


	return S_OK;
}

_int CLoading::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CLoading::Update(_float fTimeDelta)
{
	m_pTransformCom->Go_Left(fTimeDelta);
}

void CLoading::Late_Update(_float fTimeDelta)
{
	_float3 vPos{};
	XMStoreFloat3(&vPos, m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION));


	if (vPos.x < m_fFirstX *-2) {

		Set_UI_Pos(&Desc);
		Desc.fX -= Desc.fSpeedPerSec;
	}

	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CLoading::Render()
{


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIndex)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CLoading::Initialize_GORGE()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Canyon"),
	TEXT("Com_Texture_Load_Canyon"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading::Set_LoadPos(CLoading_DESC* pArg)
{
	Desc.UID = pArg->UID;
	Desc.fX = m_fX * 2.f;
	Desc.fY = pArg->fY;
	Desc.fZ = pArg->fZ;
	Desc.fSizeX = pArg->fSizeX;
	Desc.fSizeY = pArg->fSizeY;
	Desc.PrUpdate = true;
	Desc.Update = true;
	Desc.LateUpdate = true;
	Desc.fSpeedPerSec = pArg->fSpeedPerSec;
	Desc.TexIndex = pArg->TexIndex;

	return S_OK;
}



HRESULT CLoading::Add_Components()
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

CLoading* CLoading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoading* pInstance = new CLoading(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading::Clone(void* pArg)
{
	CLoading* pInstance = new CLoading(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLoading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
