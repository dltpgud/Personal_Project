#include "stdafx.h"
#include "Fade.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CFade::CFade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{pDevice, pContext}
{
}

CFade::CFade(const CFade& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CFade::Initialize_Prototype()
{
	return  S_OK;
}

HRESULT CFade::Initialize(void* pArg)
{	
    CUI_DESC* pDesc = static_cast<CUI_DESC*>(pArg);
	  pDesc->UID = UIID_Fade;
      pDesc->fX = g_iWinSizeX * 0.5f;
      pDesc->fY = g_iWinSizeY * 0.5f;
      pDesc->fZ =0.01f;
      pDesc->fSizeX = g_iWinSizeX;
      pDesc->fSizeY = g_iWinSizeY;

    if (FAILED(__super::Initialize(pDesc)))
		 return E_FAIL;
       
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
        m_pTransformCom->Set_TRANSFORM(
            CTransform::T_POSITION,
            XMVectorSet(m_fX - ViewportDesc.Width * 0.5f, -m_fY + ViewportDesc.Height * 0.5f, m_fZ, 1.f));

	return S_OK;
}

void CFade::Priority_Update(_float fTimeDelta)
{

}

void CFade::Update(_float fTimeDelta)
{
    if(true ==	m_bSetPade) 
    	m_fTimeSum += fTimeDelta * 1.5f;
    else if (false == m_bSetPade)
       m_fTimeSum -= fTimeDelta*1.5f;
    
    if(m_fTimeSum >= 1.f)
       m_fTimeSum = 1.f;
}

void CFade::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
				return;
}

HRESULT CFade::Render()
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

_bool CFade::FinishPade()
{
	if (m_fTimeSum == 0.f)
		return true;

	if (m_fTimeSum == 1.f)
		return true;

	return false;
}



HRESULT CFade::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pade"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))  
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CFade* CFade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFade* pInstance = new CFade(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFade::Clone(void* pArg)
{
	CFade* pInstance = new CFade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFade::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
