#include "stdafx.h"
#include "ShootingUI.h"
#include "GameInstance.h"
#include "Weapon.h"
CShootingUI::CShootingUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{ pDevice, pContext }
{
}

CShootingUI::CShootingUI(const CShootingUI& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CShootingUI::Initialize_Prototype()
{
	return  S_OK;
}

HRESULT CShootingUI::Initialize(void* pArg)
{
	CShootingUI_DESC* Desc = static_cast <CShootingUI_DESC*>(pArg) ;
	m_iWeaPonState = Desc->iWeaPonState;
	m_iWeaPonTYPE = Desc->iWeaPonTYPE;

	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_TRANSFORM(
		CTransform::T_POSITION,
		XMVectorSet(m_fX - ViewportDesc.Width * 0.5f, -m_fY + ViewportDesc.Height * 0.5f, m_fZ, 1.f));

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CShootingUI::Priority_Update(_float fTimeDelta)
{
}

void CShootingUI::Update(_float fTimeDelta)
{
	if (m_iWeaPonState == CWeapon::WS_SHOOT && *m_iWeaPonTYPE == CWeapon::HendGun)
	{
		m_iTex = Prrr;
		m_RGB.x  = 0.0f;
		m_RGB.y = 0.9f;
		m_RGB.z = 0.9f;
		m_RGB.w = 0.9f;
	 	
		if (m_bWeaponUP)	
		{
			m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
			m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,XMVectorSet((m_fX + rand() % 40) - ViewportDesc.Width * 0.5f, -(m_fY + rand() % 4) + ViewportDesc.Height * 0.5f, m_fZ, 1.f));
			m_bWeaponUP = false;
		}
	}

	if (m_iWeaPonState == CWeapon::WS_RELOAD && *m_iWeaPonTYPE == CWeapon::HendGun)
	{
		m_iTex = CLack;
		m_RGB.x= 0.5f;
		m_RGB.y= 0.5f;
		m_RGB.z= 0.5f;
		m_RGB.w= 0.9f;

		if (m_bWeaponDown)
		{
			m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
			m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,XMVectorSet((m_fX -50.f) - ViewportDesc.Width * 0.5f, -(m_fY+250.f) + ViewportDesc.Height * 0.5f, m_fZ, 1.f));
			m_bWeaponDown = false;
		}
	}

	if (m_iWeaPonState == CWeapon::WS_SHOOT && *m_iWeaPonTYPE == CWeapon::AssaultRifle)
	{
		m_iTex = Tra;
		m_RGB.x= 0.0f;
		m_RGB.y= 0.9f;
		m_RGB.z= 0.0f;
		m_RGB.w= 0.9f;
		if (m_bWeaponUP)
		{
			m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
			m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,XMVectorSet((m_fX+220.f + rand() % 40) - ViewportDesc.Width * 0.5f, -(m_fY+30.f +rand() % 4) + ViewportDesc.Height * 0.5f, m_fZ, 1.f));
			m_bWeaponUP = false;
		}
	}

	if (m_iWeaPonState == CWeapon::WS_RELOAD && *m_iWeaPonTYPE == CWeapon::AssaultRifle)
	{
		m_iTex = CLack;
		m_RGB.x = 0.f;
		m_RGB.y = 15.f / 255.f;
		m_RGB.z = 1.f;
		m_RGB.w = 0.9f;
		m_iTex2 = ShootEF_END;
		if (m_bWeaponDown)
		{
			m_pTransformCom->Set_Scaling(m_fSizeX , m_fSizeY , 1.f);
			m_pTransformCom->Set_TRANSFORM(
				CTransform::T_POSITION,
				XMVectorSet((m_fX - 75.f) - ViewportDesc.Width * 0.5f, -(m_fY + 260.f) + ViewportDesc.Height * 0.5f, m_fZ, 1.f));

			m_bWeaponDown = false;
		}
	}

	if (m_iWeaPonState == CWeapon::WS_SHOOT && *m_iWeaPonTYPE == CWeapon::MissileGatling)
	{
		m_iTex = TaKa;
		m_RGB.x = 1.f;
		m_RGB.y = 1.f;
		m_RGB.z = 0.f;
		m_RGB.w = 0.9f;

		if (m_bWeaponUP)
		{
			m_pTransformCom->Set_Scaling(m_fSizeX*2.f, m_fSizeY*2.f, 1.f);
			m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,XMVectorSet((m_fX+60.f + rand()%100)  - ViewportDesc.Width * 0.5f, -(m_fY - 70.f )+ ViewportDesc.Height * 0.5f, m_fZ, 1.f));
			m_bWeaponUP = false;
		}
	}

	if (m_iWeaPonState == CWeapon::WS_RELOAD && *m_iWeaPonTYPE == CWeapon::MissileGatling)
	{
		m_iTex = CLack2;
		m_RGB.x = 1.f;
		m_RGB.y = 0.f;
		m_RGB.z = 0.f;
		m_RGB.w = 0.9f;

		if (m_bWeaponDown)
		{
			m_pTransformCom->Set_Scaling(m_fSizeX * 2.f, m_fSizeY * 2.f, 1.f);
			m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,XMVectorSet((m_fX - 75.f) - ViewportDesc.Width * 0.5f, -(m_fY + 260.f) + ViewportDesc.Height * 0.5f, m_fZ, 1.f));
			m_bWeaponDown = false;
		}
	}

	if (m_iWeaPonState == CWeapon::WS_SHOOT && *m_iWeaPonTYPE == CWeapon::HeavyCrossbow)
	{
		m_iTex = Flink;
		m_RGB.x = 0.0f;
		m_RGB.y = 0.9f;
		m_RGB.z = 0.9f;
		m_RGB.w = 0.9f;

		if (m_bWeaponUP)
		{
			m_pTransformCom->Set_Scaling(m_fSizeX * 1.5f, m_fSizeY * 1.5f, 1.f);
			m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,XMVectorSet((m_fX + rand() % 100) - ViewportDesc.Width * 0.5f, -(m_fY +10) + ViewportDesc.Height * 0.5f, m_fZ, 1.f));
			m_bWeaponUP = false;
		}
	}

	if (m_iWeaPonState == CWeapon::WS_RELOAD && *m_iWeaPonTYPE == CWeapon::HeavyCrossbow)
	{
		m_iTex = CLack;
		m_RGB.x = 0.5f;
		m_RGB.y = 0.5f;
		m_RGB.z = 0.5f;
		m_RGB.w = 0.9f;
		if (m_bWeaponDown)
		{
			m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,XMVectorSet((m_fX +200.f) - ViewportDesc.Width * 0.5f, -(m_fY + 80.f) + ViewportDesc.Height * 0.5f, m_fZ, 1.f));
			m_bWeaponDown = false;
		}
	}
}

void CShootingUI::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CShootingUI::Render()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_RGB", &m_RGB,sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[m_iTex]->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	
	m_pShaderCom->Begin(5);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CShootingUI::Set_RandomPos(_bool WeaponUP, _bool WeaponDown, _bool WeaponSide)
{
	m_bWeaponUP = WeaponUP;
	m_bWeaponDown = WeaponDown;
	m_bWeaponSide = WeaponSide;
}

void CShootingUI::Set_PosClack(_float X, _float Y)
{
	m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION,XMVectorSet((m_fX - X) - ViewportDesc.Width * 0.5f, -(m_fY + Y) + ViewportDesc.Height * 0.5f, m_fZ, 1.f));
}

HRESULT CShootingUI::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

 	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferRoop"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PrrrEffect"),
		TEXT("Com_TexPrrr"), reinterpret_cast<CComponent**>(&m_pTextureCom[Prrr]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ClackEffect"),
		TEXT("Com_TexClack"), reinterpret_cast<CComponent**>(&m_pTextureCom[CLack]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TAAEffect"),
		TEXT("Com_TexTAA"), reinterpret_cast<CComponent**>(&m_pTextureCom[Taa]))))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_tAkAEffect"),
		TEXT("Com_TexTAKA"), reinterpret_cast<CComponent**>(&m_pTextureCom[TaKa]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Clack2Effect"),
		TEXT("Com_TexClack2"), reinterpret_cast<CComponent**>(&m_pTextureCom[CLack2]))))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FlinkEffect"),
		TEXT("Com_TexFlink"), reinterpret_cast<CComponent**>(&m_pTextureCom[Flink]))))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TRAEffect"),
		TEXT("Com_TexTRA"), reinterpret_cast<CComponent**>(&m_pTextureCom[Tra]))))
		return E_FAIL;

	return S_OK;
}

CShootingUI* CShootingUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShootingUI* pInstance = new CShootingUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShootingUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShootingUI::Clone(void* pArg)
{
	CShootingUI* pInstance = new CShootingUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CShootingUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShootingUI::Free()
{
	__super::Free();

	for (_int i = 0; i < ShootEF_END; i++) {
		Safe_Release(m_pTextureCom[i]);
	}
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
