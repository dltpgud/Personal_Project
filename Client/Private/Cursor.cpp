#include "stdafx.h"
#include "Cursor.h"
#include "GameInstance.h"

CCursor::CCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{ pDevice, pContext }
{
}

CCursor::CCursor(const CCursor& Prototype) : CUI{ Prototype }
{
}

HRESULT CCursor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCursor::Initialize(void* pArg)
{
	CUI_DESC			Desc{};
	Desc.fX = g_iWinSizeX * 0.5f;
	Desc.fY = g_iWinSizeY * 0.5f;
	Desc.fSizeX = 60.f;
	Desc.fSizeY = 60.f;
	Desc.Update = true;
	Desc.PrUpdate = true;
	Desc.LateUpdate = true;
	Desc.UID = UIID_Cursor;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

_int CCursor::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CCursor::Update(_float fTimeDelta)
{
	POINT ptCursor{};
	GetCursorPos(&ptCursor);
	ScreenToClient(g_hWnd, &ptCursor);

	m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet((_float)ptCursor.x - g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f - (_float)ptCursor.y, 0.f,1.f));




}

void CCursor::Late_Update(_float fTimeDelta)
{

	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CCursor::Render()
{
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

HRESULT CCursor::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cusor"),
		TEXT("Com_Texture_Menu"), reinterpret_cast<CComponent**>(&m_pTextureCom))))  
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



CCursor* CCursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCursor* pInstance = new CCursor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Cursour Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;;
}

CGameObject* CCursor::Clone(void* pArg)
{
	CCursor* pInstance = new CCursor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCursor::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}

