#include "Input_Device.h"

USING(Engine)

CInput_Device::CInput_Device()
{
}

_bool CInput_Device::Mouse_Fix(_bool Fix)
{
    if (Fix)
        m_bMouseFixed = true;

    if (Get_DIKeyDown(DIK_TAB))
        m_bMouseFixed = !m_bMouseFixed; // 토글

    if (m_bMouseFixed)
    {
        POINT ptMouse{static_cast<LONG>(m_iWinSizeX) >> 1, static_cast<LONG>(m_iWinSizeY) >> 1};
        ClientToScreen(m_hWnd, &ptMouse);
        SetCursorPos(ptMouse.x, ptMouse.y);
    }

    return m_bMouseFixed;
}

HRESULT CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    // DirectInput 객체 생성
    if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
        return E_FAIL;

    // 키보드 장치 생성
    if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
        return E_FAIL;

    m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
    m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    m_pKeyBoard->Acquire();

    // 마우스 장치 생성
    if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
        return E_FAIL;

    m_pMouse->SetDataFormat(&c_dfDIMouse);
    m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    m_pMouse->Acquire();

    m_hWnd = hWnd;
    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;

    return S_OK;
}

void CInput_Device::Update_InputDev()
{
    // 이전 상태 저장
    memcpy(m_PreKeyState, m_byKeyState, sizeof(m_PreKeyState));
    memcpy(m_PreMouseState, m_tMouseState.rgbButtons, sizeof(m_PreMouseState));

    // 현재 상태 갱신
    if (FAILED(m_pKeyBoard->GetDeviceState(sizeof(m_byKeyState), m_byKeyState)))
        m_pKeyBoard->Acquire();

    if (FAILED(m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState)))
        m_pMouse->Acquire();
}

CInput_Device* CInput_Device::Create(HINSTANCE hInst, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    CInput_Device* pInstance = new CInput_Device();

    if (FAILED(pInstance->Initialize(hInst, hWnd, iWinSizeX, iWinSizeY)))
    {
        MSG_BOX("Failed to Create : CInput_Device");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInput_Device::Free(void)
{
    if (m_pKeyBoard)
    {
        m_pKeyBoard->Unacquire();
        Safe_Release(m_pKeyBoard);
    }
    if (m_pMouse)
    {
        m_pMouse->Unacquire();
        Safe_Release(m_pMouse);
    }
    Safe_Release(m_pInputSDK);
}
