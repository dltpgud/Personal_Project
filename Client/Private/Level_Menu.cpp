#include "stdafx.h"
#include "LEVEL_MENU.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Menu.h"

/*로보 퀘스트의 로고는 생략하고 이걸 메뉴 클래스로 사용하자.. 로고에 쓸만한 이미지가 없잔아..*/

CLEVEL_MENU::CLEVEL_MENU(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLEVEL_MENU::Initialize()
{

	if (FAILED(Ready_Layer(CGameObject::GAMEOBJ_TYPE::UI)))
		return E_FAIL;


	/*플레이어꺼 놓고*/
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_Player, false)))
		return S_OK;


//m_pGameInstance->PlayBGM(L"ST.ogg",1.f);


	return S_OK;
}

void CLEVEL_MENU::Update(_float fTimeDelta)
{	
	if (FAILED(m_pGameInstance->Set_OpenUI(CUI::UIID_Player, false)))
		return ;


	__super::Update(fTimeDelta);

#ifdef _DEBUG

	if (GetAsyncKeyState(VK_F1) & 0x8000)
	{
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1, GORGE));
		return;
	}
#endif // _DEBUG
}

HRESULT CLEVEL_MENU::Render()
{
	__super::Render();

#ifdef _DEBUG

	SetWindowText(g_hWnd, TEXT("로고(메뉴)레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLEVEL_MENU::Ready_Layer(const _uint& pLayerTag)
{
	/* BackGround 레이어에 필요한 객체들을 생성(복제)하여 추가해준다. */
    /* 백그라운드 객체를 복제하여 생성할 때 Desc구조체의 주소를 void* pArg의 인자로 던진다. */
    // CMenu::BACKGROUND_DESC			Desc{};

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MENU, pLayerTag, L"Prototype_GameObject_Menu")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, pLayerTag, L"Prototype_GameObject_PlayerHP_UI")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, pLayerTag, L"Prototype_GameObject_Cursor")))
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
	__super::Free();

}
