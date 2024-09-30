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

	Desc->Update = true;
	Desc->PrUpdate = true;
	Desc->LateUpdate = true;
	Desc->UID = UIID_Loading;
	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Set_LoadPos()))
		return E_FAIL;

	if (FAILED(Set_LoadPos2()))
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

}

void CLoading::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CLoading::Render()
{
	for (_int i = 0; i < 6; i++) 
	{
		Set_UI_Pos(&Desc[i]);
			Desc[i].fX -= Desc[i].fSpeedPerSec;
		
	
		if (i == 0 || i == 3) {
		
			if (Desc[i].fX < Desc[i].x * -0.5f) {
				Set_UI_Pos(&pDesc[i]);
				pDesc[i].fX -= pDesc[i].fSpeedPerSec;

				if (pDesc[i].fX < pDesc[i].x * -0.5f)
					Roop(i, Desc, pDesc);
			}
		}
		else
		{
		
	

			if (Desc[i].fX < Desc[i].x *-2) {
				Set_UI_Pos(&pDesc[i]);
				pDesc[i].fX -= pDesc[i].fSpeedPerSec;

				if (pDesc[i].fX < pDesc[i].x * -2)
					Roop(i, Desc, pDesc);

			}
			
		}

		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Bind_Buffers();

		m_pVIBufferCom->Render();
	}
	return S_OK;
}

HRESULT CLoading::Initialize_GORGE()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Canyon"),
	TEXT("Com_Texture_Load_Canyon"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


HRESULT CLoading::Set_LoadPos()
{
	/*배경 1 선인장*/
	Desc[0].UID = UIID_Loading;
	Desc[0].fX = 4080.f;
	Desc[0].fY = 360.f;
	Desc[0].fSizeX = 4080.f;
	Desc[0].fSizeY = 720.f;
	Desc[0].PrUpdate = true;
	Desc[0].Update = true;
	Desc[0].LateUpdate = true;
	Desc[0].fSpeedPerSec = 20.f;
	Desc[0].x = 4080.f;


	/*bus*/
	Desc[1].UID = UIID_Loading;
	Desc[1].fX = 231.f;
	Desc[1].fY = 509.f;
	Desc[1].fSizeX = 550.f;
	Desc[1].fSizeY = 280.f;
	Desc[1].PrUpdate = true;
	Desc[1].Update = true;
	Desc[1].LateUpdate = true;
	Desc[1].fSpeedPerSec = 0.f;
	Desc[1].x = 231.f;

	/*바닥*/
	Desc[2].UID = UIID_Loading;
	Desc[2].fX = 10040.f;
	Desc[2].fY = 720.f;
	Desc[2].fSizeX = 20040.f;
	Desc[2].fSizeY = 200.f;
	Desc[2].PrUpdate = true;
	Desc[2].Update = true;
	Desc[2].LateUpdate = true;
	Desc[2].fSpeedPerSec = 7.f;
	Desc[2].x = 10040.f;

	/*배경 2 중간에 나오는 배경*/
	Desc[3].UID = UIID_Loading;
	Desc[3].fX = 2150.f;
	Desc[3].fY = 360.f;
	Desc[3].fSizeX = 2280.f;;
	Desc[3].fSizeY = 720.f;
	Desc[3].PrUpdate = true;
	Desc[3].Update = true;
	Desc[3].LateUpdate = true;
	Desc[3].fSpeedPerSec = 6.f;
	Desc[3].x = 2150.f;

	/*배경 3 중간 배경*/
	Desc[4].UID = UIID_Loading;
	Desc[4].fX = 8200.f;
	Desc[4].fY = 360.f;
	Desc[4].fSizeX = 10480.f;;
	Desc[4].fSizeY = 720.f;
	Desc[4].PrUpdate = true;
	Desc[4].Update = true;
	Desc[4].LateUpdate = true;
	Desc[4].fSpeedPerSec = 5.f;
	Desc[4].x = 5200.f;

	/*가장 뒤 배경*/
	Desc[5].UID = UIID_Loading;
	Desc[5].fX = 3500.f;
	Desc[5].fY = 120.f;
	Desc[5].fSizeX = 8000.f;
	Desc[5].fSizeY = 1340.f;
	Desc[5].PrUpdate = true;
	Desc[5].Update = true;
	Desc[5].LateUpdate = true;
	Desc[5].fSpeedPerSec = 2.f;
	Desc[5].x = 3500.f;
	return S_OK;
}

HRESULT CLoading::Set_LoadPos2()
{	/*배경 1 선인장*/
	pDesc[0].UID = UIID_Loading;
	pDesc[0].fX = Desc[0].fX * 0.5f;
	pDesc[0].fY = 360.f;
	pDesc[0].fSizeX = 4080.f;
	pDesc[0].fSizeY = 720.f;
	pDesc[0].PrUpdate = true;
	pDesc[0].Update = true;
	pDesc[0].LateUpdate = true;
	pDesc[0].fSpeedPerSec = 20.f;
	pDesc[0].x = 4080.f;


	/*bus*/
	pDesc[1].UID = UIID_Loading;
	pDesc[1].fX = Desc[0].fX * 0.5f;
	pDesc[1].fY = Desc[1].fY;
	pDesc[1].fSizeX = 550.f;
	pDesc[1].fSizeY = 280.f;
	pDesc[1].PrUpdate = true;
	pDesc[1].Update = true;
	pDesc[1].LateUpdate = true;
	pDesc[1].fSpeedPerSec = 0.f;
	pDesc[1].x = 231.f;
	
	/*바닥*/
	pDesc[2].UID = UIID_Loading;
	pDesc[2].fX = Desc[0].fX * 0.5f;
	pDesc[2].fY = 720.f;
	pDesc[2].fSizeX = 20040.f;
	pDesc[2].fSizeY = 200.f;
	pDesc[2].PrUpdate = true;
	pDesc[2].Update = true;
	pDesc[2].LateUpdate = true;
	pDesc[2].fSpeedPerSec = 7.f;
	pDesc[2].x = 10040.f;
	
	/*배경 2 중간에 나오는 배경*/
	pDesc[3].UID = UIID_Loading;
	pDesc[3].fX = Desc[0].fX * 0.5f;
	pDesc[3].fY = 360.f;
	pDesc[3].fSizeX = 2280.f;;
	pDesc[3].fSizeY = 720.f;
	pDesc[3].PrUpdate = true;
	pDesc[3].Update = true;
	pDesc[3].LateUpdate = true;
	pDesc[3].fSpeedPerSec = 6.f;
	pDesc[3].x = 2150.f;
	
	/*배경 3 중간 배경*/
	pDesc[4].UID = UIID_Loading;
	pDesc[4].fX = Desc[0].fX * 0.5f;
	pDesc[4].fY = 360.f;
	pDesc[4].fSizeX = 10480.f;;
	pDesc[4].fSizeY = 720.f;
	pDesc[4].PrUpdate = true;
	pDesc[4].Update = true;
	pDesc[4].LateUpdate = true;
	pDesc[4].fSpeedPerSec = 5.f;
	pDesc[4].x = 5200.f;
	
	/*가장 뒤 배경*/
	pDesc[5].UID = UIID_Loading;
	pDesc[5].fX = Desc[0].fX * 0.5f;
	pDesc[5].fY = 120.f;
	pDesc[5].fSizeX = 8000.f;
	pDesc[5].fSizeY = 1340.f;
	pDesc[5].PrUpdate = true;
	pDesc[5].Update = true;
	pDesc[5].LateUpdate = true;
	pDesc[5].fSpeedPerSec = 2.f;
	pDesc[5].x = 3500.f;

	return S_OK;
}

void CLoading::Roop(const _uint& i, CLoading_DESC desc[], CLoading_DESC pdesc[])
{
	desc[i].fX = pdesc[i].fSizeX;
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
