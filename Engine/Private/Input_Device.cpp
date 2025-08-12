#include "Input_Device.h"

Engine::CInput_Device::CInput_Device(void)
{

}

_bool CInput_Device::Mouse_Fix(_bool Fix)
{
    if (true == Fix)
        m_bturn = true;

    if (Get_DIKeyDown(DIK_TAB))
    {
        if (!m_bturn)
        {
            m_bturn = true;
        }
        else
            m_bturn = false;
    }

	if (m_bturn == true) 
	{
		POINT ptMouse{ static_cast<LONG>(g_iWinSizeX) >> 1, static_cast<LONG>(g_iWinSizeY) >> 1 };

		ClientToScreen(g_Hwnd, &ptMouse);
		SetCursorPos(ptMouse.x, ptMouse.y);
	}


	return m_bturn;
}

HRESULT Engine::CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK, nullptr)))
		return E_FAIL;
											

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;


	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	g_Hwnd = hWnd;
	g_iWinSizeX = iWinSizeX;
	g_iWinSizeY = iWinSizeY;
	return S_OK;
}

void CInput_Device::Update_InputDev(void)
{	
	memcpy(m_PreKeyState, m_byKeyState, sizeof(m_PreKeyState));
	memcpy(m_PreMouseState, m_tMouseState.rgbButtons, sizeof(_byte) * 3);

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}


CInput_Device * CInput_Device::Create(HINSTANCE hInst, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	CInput_Device*		pInstance = new CInput_Device();

	if (FAILED(pInstance->Initialize(hInst, hWnd, iWinSizeX, iWinSizeY)))
	{
		MSG_BOX("Failed to Created : CInput_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

