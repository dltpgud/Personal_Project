#include "stdafx.h"
#include "LEVEL_MENU.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Menu.h"
#include "SpriteTexture.h"
#include "Aim.h"
#include "Player.h"
#include "Pade.h"
/*로보 퀘스트의 로고는 생략하고 이걸 메뉴 클래스로 사용하자.. 로고에 쓸만한 이미지가 없잔아..*/

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


	m_pGameInstance->PlayBGM(CSound::SOUND_MENU, L"Menu.mp3",0.5f);
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
    CUI::CUI_DESC MENUUIDESC;
    MENUUIDESC.iDeleteLevel = LEVEL_MENU;
    MENUUIDESC.iMaxLevel = LEVEL_END;
    MENUUIDESC.pEnable_Level = new _bool[LEVEL_END]{false, false, true, false, false, false};
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"menu",L"Prototype_GameObject_Menu",&MENUUIDESC)))
		return E_FAIL;
    Safe_Delete_Array(MENUUIDESC.pEnable_Level);

    CUI::CUI_DESC CURSORUIDESC;
    CURSORUIDESC.iDeleteLevel = LEVEL_MENU;
    CURSORUIDESC.iMaxLevel = LEVEL_END;
    CURSORUIDESC.pEnable_Level = new _bool[LEVEL_END]{false, false, true, false, false, false};
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"CURSOR", L"Prototype_GameObject_Cursor", &CURSORUIDESC)))
        return E_FAIL;
    Safe_Delete_Array(CURSORUIDESC.pEnable_Level);

   CUI::CUI_DESC PlayerHPDESC;
   PlayerHPDESC.iDeleteLevel = LEVEL_STATIC;
   PlayerHPDESC.iMaxLevel = LEVEL_END;
   PlayerHPDESC.pEnable_Level = new _bool[LEVEL_END]{false, false, false, true, true, true};
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"playerHP", L"Prototype_GameObject_PlayerHP_UI", &PlayerHPDESC)))
   	return E_FAIL;
   Safe_Delete_Array(PlayerHPDESC.pEnable_Level);
   
   CUI::CUI_DESC WeaponUIDESC;
   WeaponUIDESC.iDeleteLevel = LEVEL_STATIC;
   WeaponUIDESC.iMaxLevel = LEVEL_END;
   WeaponUIDESC.pEnable_Level = new _bool[LEVEL_END]{false, false, false, true, true, true};
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"WeaponUI", L"Prototype_GameObject_WeaponUI_UI", &WeaponUIDESC)))
   	return E_FAIL;
   Safe_Delete_Array(WeaponUIDESC.pEnable_Level);
   
   CUI::CUI_DESC InteractiveUIDESC;
   InteractiveUIDESC.iDeleteLevel = LEVEL_STATIC;
   InteractiveUIDESC.iMaxLevel = LEVEL_END;
   InteractiveUIDESC.pEnable_Level = new _bool[LEVEL_END]{false, false, false, true, true, true};
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"Interactive", L"Prototype_GameObject_InteractiveUI", &InteractiveUIDESC)))
   	return E_FAIL;
   Safe_Delete_Array(InteractiveUIDESC.pEnable_Level);
   
   CAim::CAim_DESC Desc1{};
   Desc1.bshakingX = true;
   Desc1.bshakingY = false;
   Desc1.fSizeX = 10.f;
   Desc1.fSizeY = 4.f;
   Desc1.fX = g_iWinSizeX * 0.5f -10.f;
   Desc1.fY = g_iWinSizeY * 0.5f;
   Desc1.UID = CUI::UIID_PlayerWeaPon_Aim;
   Desc1.Update = true;
   Desc1.fpositive_OR_negative = -1.f;
   Desc1.iDeleteLevel = LEVEL_STATIC;
   Desc1.iMaxLevel = LEVEL_END;
   Desc1.pEnable_Level = new _bool[LEVEL_END]{false, false, false, true, true, true};
   if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"WeaPon_Aim1", L"Prototype_GameObject_Player_Aim",&Desc1)))
   	return E_FAIL;
         Safe_Delete_Array(Desc1.pEnable_Level);
   
   CAim::CAim_DESC Desc2{};
   Desc2.bshakingX = true;
   Desc2.bshakingY = false;
   Desc2.fSizeX = 10.f;
   Desc2.fSizeY = 4.f;
   Desc2.fX = g_iWinSizeX * 0.5f +10.f;
   Desc2.fY = g_iWinSizeY * 0.5f;
   Desc2.UID = CUI::UIID_PlayerWeaPon_Aim;
   Desc2.Update = true;
     Desc2.iDeleteLevel = LEVEL_STATIC;
     Desc2.iMaxLevel = LEVEL_END;
     Desc2.pEnable_Level = new _bool[LEVEL_END]{false, false, false, true, true, true};
   Desc2.fpositive_OR_negative = 1.f;
     if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"WeaPon_Aim2", L"Prototype_GameObject_Player_Aim", &Desc2)))
   	return E_FAIL;
         Safe_Delete_Array(Desc2.pEnable_Level);
   
     CAim::CAim_DESC Desc3{};
   Desc3.bshakingY = true;
   Desc3.bshakingX = false;
   Desc3.fSizeX = 4.f;
   Desc3.fSizeY = 10.f;
   Desc3.fX = g_iWinSizeX * 0.5f;
   Desc3.fY = g_iWinSizeY * 0.5f -10.f;
   Desc3.UID = CUI::UIID_PlayerWeaPon_Aim;
   Desc3.Update = true;
   Desc3.fpositive_OR_negative = +1.f;
     Desc3.iDeleteLevel = LEVEL_STATIC;
     Desc3.iMaxLevel = LEVEL_END;
     Desc3.pEnable_Level = new _bool[LEVEL_END]{false, false, false, true, true, true};
     if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"WeaPon_Aim3", L"Prototype_GameObject_Player_Aim", &Desc3)))
   	return E_FAIL;
         Safe_Delete_Array(Desc3.pEnable_Level);
   
     CAim::CAim_DESC Desc4{};
   Desc4.bshakingY = true;
   Desc4.bshakingX = false;
   Desc4.fSizeX = 4.f;
   Desc4.fSizeY = 10.f;
	Desc4.fX = g_iWinSizeX * 0.5f;
	Desc4.fY = g_iWinSizeY * 0.5f + 10.f;
	Desc4.UID = CUI::UIID_PlayerWeaPon_Aim;
	Desc4.Update = true;
	Desc4.fpositive_OR_negative = -1.f;
    Desc4.iDeleteLevel = LEVEL_STATIC;
    Desc4.iMaxLevel = LEVEL_END;
    Desc4.pEnable_Level = new _bool[LEVEL_END]{false, false, false, true, true, true};
    if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"WeaPon_Aim4", L"Prototype_GameObject_Player_Aim", &Desc4)))
		return E_FAIL;
        Safe_Delete_Array(Desc4.pEnable_Level);

	CUI::CUI_DESC FadeDESC{};
        FadeDESC.iDeleteLevel = LEVEL_STATIC;
        FadeDESC.iMaxLevel = LEVEL_END;
        FadeDESC.pEnable_Level = new _bool[LEVEL_END]{true, true, true, true, true, true};
        FadeDESC.Update = true;
	if (FAILED(m_pGameInstance->Add_UI_To_CLone(L"Fade",L"Prototype_Gameobject_Pade",&FadeDESC)))
		return E_FAIL;
        Safe_Delete_Array(FadeDESC.pEnable_Level);
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
	__super::Free();

}
