#include "stdafx.h"
#include "Menu.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CMenu::CMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CUI{pDevice, pContext}
{
}

CMenu::CMenu(const CMenu& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CMenu::Initialize_Prototype()
{
	/* 패킷, 파일 입출력을 통한 초기화. */

	return  S_OK;
}

HRESULT CMenu::Initialize(void* pArg)
{	/* _float3	vTmp = _float3(1.f, 1.f, 1.f);
	XMStoreFloat3(&vTmp, XMVector3Normalize(XMLoadFloat3(&vTmp)));*/

	/*상위 클래스에 Arg를 던져 주어 최상위 클래스에 정보를 채워준다.*/
	/*최상위 부모에게 값을 넘겨 주면서 트렌스 폼을 생성해준다.*/
	
	CUI_DESC			Desc{};
	  Desc.PrUpdate = true;
      Desc.Update = true;
      Desc.LateUpdate = true;
	  Desc.UID = UIID_Menu;
	if (FAILED(__super::Initialize(&Desc)))
		 return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Set_MenuPos()))
		return E_FAIL;

	return S_OK;
}

_int CMenu::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CMenu::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < 3; i++) {	
	
		if (Desc[i].Hoverst == HS_Hover )
		{
			RECT		rcUI = {static_cast<LONG> (Desc[i].fX - Desc[i].fSizeX * 0.5f),static_cast<LONG> ( Desc[i].fY - Desc[i].fSizeY * 0.5f),
							  static_cast<LONG> (Desc[i].fX + Desc[i].fSizeX * 0.5f),static_cast<LONG> ( Desc[i].fY + Desc[i].fSizeY * 0.5f) };

			POINT		ptMouse{};

			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);

			
			if (true == (_bool)PtInRect(&rcUI, ptMouse))
			{
				Desc[i].LateUpdate = true;

				if (m_pGameInstance->Get_DIMouseDown(DIM_LB))
				{
					m_pGameInstance->Play_Sound(L"ST_Button_Click.ogg", CSound::SOUND_BGM, 1.f); 
						if (i == 0) {
							m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1, GORGE));
						}
						else if (i == 1)
						{
							PostQuitMessage(0);
						}
				}
			}
			else
			{
				Desc[i].LateUpdate = false;
			}
		}

	}
		
}

void CMenu::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
				return;
}

HRESULT CMenu::Render()
{

	Set_UI_Pos(&Desc[2]);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	for (_uint i = 0; i < 3; i++)
	{
		Set_UI_Pos(&Desc[i]);
		
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (Desc[i].LateUpdate == false)
		
			continue;
	


		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Bind_Buffers();

		m_pVIBufferCom->Render();
	}




	m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("게임 시작"), _float2(160.f, 435.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.8f);
	m_pGameInstance->Render_Text(TEXT("Robo"), TEXT("게임 종료"), _float2(160.f, 525.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.8f);
	return S_OK;
}

HRESULT CMenu::Set_MenuPos()
{  /*메뉴바 1*/
	Desc[0].UID = UIID_Menu;
	Desc[0].fX = 255.f;
	Desc[0].fY = 475.f;
	Desc[0].fZ = 0.1f;
	Desc[0].fSizeX = 255.f;
	Desc[0].fSizeY = 45.f;
	Desc[0].PrUpdate = true;
	Desc[0].Update = true;
	Desc[0].LateUpdate = false;
	Desc[0].fSpeedPerSec = 0.f;
	Desc[0].fRotationPerSec = 0.f;
	Desc[0].Hoverst = HS_Hover;

   /*메뉴바 2*/
	Desc[1].UID = UIID_Menu;
	Desc[1].fX = 255.f;
	Desc[1].fY = 565.f;
	Desc[1].fZ = 0.2f;
	Desc[1].fSizeX = 255.f;
	Desc[1].fSizeY = 45.f;
	Desc[1].PrUpdate = true;
	Desc[1].Update = true;
	Desc[1].LateUpdate = false;
	Desc[1].fSpeedPerSec = 0.f;
	Desc[1].fRotationPerSec = 0.f;
	Desc[1].Hoverst = HS_Hover;

   /*메뉴 배경*/
	Desc[2].UID = UIID_Menu;
	Desc[2].fX = g_iWinSizeX * 0.5f;
	Desc[2].fY = g_iWinSizeY * 0.5f;
	Desc[2].fZ = 0.3f;
	Desc[2].fSizeX = g_iWinSizeX;
	Desc[2].fSizeY = g_iWinSizeY;
	Desc[2].PrUpdate = true;
	Desc[2].Update = true;
	Desc[2].LateUpdate = true;
	Desc[2].fSpeedPerSec = 0.f;
	Desc[2].fRotationPerSec = 0.f;		
	Desc[2].Hoverst = HS_NonHover;

	return S_OK;
}

HRESULT CMenu::Add_Components()
{
	/* 멤버변수로 직접 참조를 하게되면 */
	/* 1. 내가 내 컴포넌트를 이용하고자할 때 굳이 검색이 필요없이 특정 멤버변수로 바로 기능을 이용하면 된다. */
	/* 2. 다른 객체가 내 컴포넌트를 검색하고자 할때 스위치케이스가 겁나 늘어나는 상황. */

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu"),
		TEXT("Com_Texture_Menu"), reinterpret_cast<CComponent**>(&m_pTextureCom))))  // 다이나믹 캐스팅도 스타틱 캐스팅도 할 수  없는 상황.
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

CMenu* CMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMenu* pInstance = new CMenu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMenu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMenu::Clone(void* pArg)
{
	CMenu* pInstance = new CMenu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMenu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenu::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
