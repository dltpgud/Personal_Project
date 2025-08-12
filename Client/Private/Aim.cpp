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
	return  S_OK;
}

HRESULT CAim::Initialize(void* pArg)
{
	CAim_DESC* Desc = static_cast <CAim_DESC*>(pArg) ;
    
	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

    if (FAILED(Set_Desc()))
        return E_FAIL;
       
	return S_OK;
}

void CAim::Priority_Update(_float fTimeDelta)
{
}

void CAim::Update(_float fTimeDelta)
{
}

void CAim::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
    if (FAILED(m_pGameInstance->ADD_UI_ShakingList(this)))
        return;
}

HRESULT CAim::Render()
{	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	for (_int i = 0; i < 4; i++)
    {
        Set_UI_Pos(&m_AimDesc[i]);
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
              return E_FAIL;

         m_pShaderCom->Begin(0);
         m_pVIBufferCom->Bind_Buffers();
         m_pVIBufferCom->Render();
    }
	return S_OK;
}

void CAim::ShakingEvent(_float fTimeDelta)
{
    for (_int i = 0; i < 4; i++)
    {
        if (m_fShakingTime > 0.f)
        {
            if (m_AimDesc[i].bshakingX)
                m_AimDesc[i].fX += m_fShaking_X * m_AimDesc[i].fpositive_OR_negative;
            else if (m_AimDesc[i].bshakingY)
                m_AimDesc[i].fY += m_fShaking_Y * m_AimDesc[i].fpositive_OR_negative;
        }
        if (m_fShakingTime <= 0.f)
        {
            m_AimDesc[i].fX = m_AimDesc[i].fPrXPos;
            m_AimDesc[i].fY = m_AimDesc[i].fPrYPos;
        }
    }
}

HRESULT CAim::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

 	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferRoop"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Player_Aim"),
		TEXT("Com_Aim"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAim::Set_Desc()
{
    m_AimDesc[0].bshakingX = true;
    m_AimDesc[0].bshakingY = false;
    m_AimDesc[0].fSizeX = 10.f;
    m_AimDesc[0].fSizeY = 4.f;
    m_AimDesc[0].fX = g_iWinSizeX * 0.5f - 10.f;
    m_AimDesc[0].fY = g_iWinSizeY * 0.5f;
    m_AimDesc[0].fZ = 0.5f;
    m_AimDesc[0].fpositive_OR_negative = -1.f;
    m_AimDesc[0].fPrXPos = m_AimDesc[0].fX;
    m_AimDesc[0].fPrYPos = m_AimDesc[0].fY;

	m_AimDesc[1].bshakingX = true;
    m_AimDesc[1].bshakingY = false;
    m_AimDesc[1].fSizeX = 10.f;
    m_AimDesc[1].fSizeY = 4.f;
    m_AimDesc[1].fX = g_iWinSizeX * 0.5f + 10.f;
    m_AimDesc[1].fY = g_iWinSizeY * 0.5f;
    m_AimDesc[1].fZ = 0.5f;
    m_AimDesc[1].fpositive_OR_negative = 1.f;
    m_AimDesc[1].fPrXPos = m_AimDesc[1].fX;
    m_AimDesc[1].fPrYPos = m_AimDesc[1].fY;

	m_AimDesc[2].bshakingY = true;
    m_AimDesc[2].bshakingX = false;
    m_AimDesc[2].fSizeX = 4.f;
    m_AimDesc[2].fSizeY = 10.f;
    m_AimDesc[2].fX = g_iWinSizeX * 0.5f;
    m_AimDesc[2].fY = g_iWinSizeY * 0.5f - 10.f;
    m_AimDesc[2].fZ = 0.5f;
    m_AimDesc[2].fpositive_OR_negative = +1.f;
    m_AimDesc[2].fPrXPos = m_AimDesc[2].fX;
    m_AimDesc[2].fPrYPos = m_AimDesc[2].fY;

	m_AimDesc[3].bshakingY = true;
    m_AimDesc[3].bshakingX = false;
    m_AimDesc[3].fSizeX = 4.f;
    m_AimDesc[3].fSizeY = 10.f;
    m_AimDesc[3].fX = g_iWinSizeX * 0.5f;
    m_AimDesc[3].fY = g_iWinSizeY * 0.5f + 10.f;
    m_AimDesc[3].fZ = 0.5f;
    m_AimDesc[3].fpositive_OR_negative = -1.f;
    m_AimDesc[3].fPrXPos = m_AimDesc[3].fX;
    m_AimDesc[3].fPrYPos = m_AimDesc[3].fY;
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
