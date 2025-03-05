#include "stdafx.h"
#include "Pade.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CPade::CPade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{pDevice, pContext}
{
}

CPade::CPade(const CPade& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CPade::Initialize_Prototype()
{
	/* 패킷, 파일 입출력을 통한 초기화. */

	return  S_OK;
}

HRESULT CPade::Initialize(void* pArg)
{	
	CUI_DESC			Desc{};

      Desc.Update = false;

	  Desc.UID = UIID_Pade;
	  Desc.fX = g_iWinSizeX * 0.5f;
	  Desc.fY = g_iWinSizeY * 0.5f;
	  Desc.fZ = 0.f;
	  Desc.fSizeX = g_iWinSizeX ;
	  Desc.fSizeY = g_iWinSizeY;
	if (FAILED(__super::Initialize(&Desc)))
		 return E_FAIL;

	Set_UI_Pos(&Desc);
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

_int CPade::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CPade::Update(_float fTimeDelta)
{
if(true ==	m_bSetPade) 
	m_fTimeSum += fTimeDelta*1.5f;
else if (false == m_bSetPade)
    m_fTimeSum -= fTimeDelta*1.5f;

   if (m_fTimeSum >= 1.f)
      m_fTimeSum = 1.f;
}

void CPade::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
				return;
}

HRESULT CPade::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_TimeSum", &m_fTimeSum, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(7);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();


	return S_OK;
}

_bool CPade::FinishPade()
{
	if (m_fTimeSum == 0.f)
		return true;

	if (m_fTimeSum == 1.f)
		return true;

	return false;
}



HRESULT CPade::Add_Components()
{
	/* 멤버변수로 직접 참조를 하게되면 */
	/* 1. 내가 내 컴포넌트를 이용하고자할 때 굳이 검색이 필요없이 특정 멤버변수로 바로 기능을 이용하면 된다. */
	/* 2. 다른 객체가 내 컴포넌트를 검색하고자 할때 스위치케이스가 겁나 늘어나는 상황. */

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pade"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))  // 다이나믹 캐스팅도 스타틱 캐스팅도 할 수  없는 상황.
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

CPade* CPade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPade* pInstance = new CPade(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPade::Clone(void* pArg)
{
	CPade* pInstance = new CPade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPade::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
