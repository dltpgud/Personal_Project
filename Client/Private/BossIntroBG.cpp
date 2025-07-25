#include "stdafx.h"
#include "BossIntroBG.h"
#include "GameInstance.h"
#include "Transform.h"
#include "SceneCamera.h"
#include "Camera_Free.h"
#include "BillyBoom.h"
#include"Player.h"
#include "BossHP.h"
CBossIntroBG::CBossIntroBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{ pDevice, pContext }
{
}

CBossIntroBG::CBossIntroBG(const CBossIntroBG& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CBossIntroBG::Initialize_Prototype()
{
	/* 패킷, 파일 입출력을 통한 초기화. */

	return  S_OK;
}

HRESULT CBossIntroBG::Initialize(void* pArg)
{
	CBossIntroBG_DESC* Desc = static_cast <CBossIntroBG_DESC*>(pArg) ;

	if (FAILED(__super::Initialize(Desc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if(FAILED(Set_Pos()))
	 return E_FAIL;
		return S_OK;
}

void CBossIntroBG::Priority_Update(_float fTimeDelta)
{

	m_pTransformCom->LookAt(XMLoadFloat4(m_pGameInstance->Get_CamPosition()));

}

void CBossIntroBG::Update(_float fTimeDelta)
{
	m_interver += fTimeDelta;
	if (m_interver > 1.0f) {
		m_interver = 1.0f;  // 이미지가 완전히 나타나면 멈춤

		m_TimeSum += fTimeDelta;
	}

	if (m_TimeSum >= 2.5f)
	{
		m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerHP, true);

		m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerWeaPon, true);

		m_pGameInstance->Set_OpenUI(CUI::UIID_PlayerWeaPon_Aim, true);

				dynamic_cast<CBossHPUI*>(m_pGameInstance->Find_Clone_UIObj(L"BossHP"))->Set_Open(true);

		dynamic_cast<CBillyBoom*>(
                    m_pGameInstance->Find_CloneGameObject(LEVEL_BOSS, TEXT("Layer_Monster"), CGameObject::ACTOR))
                    ->Set_State(CBillyBoom::ST_Idle);
	
		dynamic_cast<CCamera_Free*>(
                    m_pGameInstance->Find_CloneGameObject(LEVEL_STATIC, TEXT("Layer_Camera"), CGameObject::CAMERA))
                    ->Set_Update(true);
	
		dynamic_cast<CSceneCamera*>(
                    m_pGameInstance->Find_CloneGameObject(LEVEL_BOSS, TEXT("Layer_Camera"), CGameObject::CAMERA))
                    ->Set_Dead(true);
                static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_Key(true);

		m_bDead = true;
	}
}

void CBossIntroBG::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
		return;
}

HRESULT CBossIntroBG::Render()
{
	for (_uint i = 0; i < 3; i++) {

		Set_IntroPos(&IntroDesc[i]);
		
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;

		if (FAILED(m_pTextureCom[TYPE_Mask]->Bind_ShaderResource(m_pShaderCom, "maskTexture", 0)))
			return E_FAIL;
		if (FAILED(m_pTextureCom[TYPE_Tex]->Bind_ShaderResource(m_pShaderCom, "g_Texture", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("threshold",&m_interver, sizeof(_float))))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pVIBufferCom->Bind_Buffers();

		m_pVIBufferCom->Render();
	}

	return S_OK;
}

HRESULT CBossIntroBG::Set_Pos()
{

	IntroDesc[0].fSizeX = 35.f;
	IntroDesc[0].fSizeY = 18.f;
	IntroDesc[0].fX = m_fX;
	IntroDesc[0].fY = m_fY ;
	IntroDesc[0].fZ = m_fZ+4.f;


	IntroDesc[2].fSizeX = 7.f;
	IntroDesc[2].fSizeY = 3.f;
	IntroDesc[2].fX = m_fX;
	IntroDesc[2].fY = m_fY  + 0.3f;
	IntroDesc[2].fZ = m_fZ	- 9.9f;

	IntroDesc[1].fSizeX = 2.f;
	IntroDesc[1].fSizeY = 2.f;
	IntroDesc[1].fX =  m_fX	+ 1.7f;
	IntroDesc[1].fY =  m_fY + 1.5f;
	IntroDesc[1].fZ =  m_fZ - 8.8f;

	return S_OK;
}

void CBossIntroBG::Set_IntroPos(void* pArg)
{
	CUI_DESC* pDesc = static_cast<CUI_DESC*>(pArg);

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fZ = pDesc->fZ;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_TRANSFORM(
		CTransform::T_POSITION,
		XMVectorSet(m_fX, m_fY + m_fSizeY * 0.4f, m_fZ, 1.f));
}



HRESULT CBossIntroBG::Add_Components()
{
	HRESULT hr = {};

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_Inverse"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
 	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferRoop"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Texture_Mask"),
		TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_Mask]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Intro"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_Tex]))))
		return E_FAIL;

	return hr;

}

CBossIntroBG* CBossIntroBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossIntroBG* pInstance = new CBossIntroBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBossIntroBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossIntroBG::Clone(void* pArg)
{
	CBossIntroBG* pInstance = new CBossIntroBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBossIntroBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossIntroBG::Free()
{
	__super::Free();
	for (size_t i = 0; i < TYPE_END; i++) {
		Safe_Release(m_pTextureCom[i]);
	}
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
