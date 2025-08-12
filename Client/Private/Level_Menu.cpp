#include "stdafx.h"
#include "LEVEL_MENU.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Menu.h"
#include "SpriteTexture.h"
#include "Aim.h"
#include "Player.h"
#include "Fade.h"

CLEVEL_MENU::CLEVEL_MENU(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLEVEL_MENU::Initialize()
{
	if (m_pGameInstance->Get_Player() != nullptr)
	{
		static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_bUpdate(false);
	}

	if (FAILED(Ready_UI()))
		return E_FAIL;

	m_pGameInstance->PlayBGM(CSound::SOUND_MENU, L"Menu.mp3",0.7f);

	return S_OK;
}

void CLEVEL_MENU::Update(_float fTimeDelta)
{	
	__super::Update(fTimeDelta);
}

HRESULT CLEVEL_MENU::Render()
{
	__super::Render();

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고(메뉴)레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLEVEL_MENU::Ready_UI()
{
   CUI::CUI_DESC DESC;
   DESC.iDeleteLevel = LEVEL_MENU;
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"menu",L"Prototype_GameObject_Menu",&DESC)))
		return E_FAIL;

   CUI::CUI_DESC CURSORUIDESC;
   CURSORUIDESC.iDeleteLevel = LEVEL_MENU;
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"CURSOR", L"Prototype_GameObject_Cursor", &CURSORUIDESC)))
        return E_FAIL;

   CUI::CUI_DESC PlayerHPDESC;
   PlayerHPDESC.iDeleteLevel = LEVEL_STATIC;
   PlayerHPDESC.Update = false;
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"playerHP", L"Prototype_GameObject_PlayerHP_UI", &PlayerHPDESC)))
      	return E_FAIL;
   
   CUI::CUI_DESC WeaponUIDESC;
   WeaponUIDESC.iDeleteLevel = LEVEL_STATIC;
   WeaponUIDESC.Update = false;
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"WeaponUI", L"Prototype_GameObject_WeaponUI_UI", &WeaponUIDESC)))
   	    return E_FAIL;
   
   CUI::CUI_DESC InteractiveUIDESC;
   InteractiveUIDESC.iDeleteLevel = LEVEL_STATIC;
   InteractiveUIDESC.Update = false;
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"Interactive", L"Prototype_GameObject_InteractiveUI", &InteractiveUIDESC)))
   	    return E_FAIL;
   
   CAim::CAim_DESC Desc1{};
   Desc1.UID = UIID_PlayerWeaPon_Aim;
   Desc1.Update = false;
   Desc1.iDeleteLevel = LEVEL_STATIC;
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"WeaPon_Aim1", L"Prototype_GameObject_Player_Aim",&Desc1)))
   	    return E_FAIL;

   CUI::CUI_DESC FadeDESC{};
   FadeDESC.iDeleteLevel = LEVEL_STATIC;
   FadeDESC.Update = false;
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"Fade",L"Prototype_Gameobject_Pade",&FadeDESC)))
		return E_FAIL;

	return S_OK;
}

CLEVEL_MENU* CLEVEL_MENU::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLEVEL_MENU* pInstance = new CLEVEL_MENU(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLEVEL_MENU");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLEVEL_MENU::Free()
{		
	 m_pGameInstance->StopSound(CSound::SOUND_MENU);
	__super::Free();
}
