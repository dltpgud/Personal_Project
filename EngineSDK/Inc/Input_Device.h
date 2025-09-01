#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
private:
    CInput_Device();
    virtual ~CInput_Device() = default;

public:
    _byte Get_DIKeyState(_ubyte byKeyID) const
    {
        return m_byKeyState[byKeyID];
    }

    _byte Get_DIMouseState(MOUSEKEYSTATE eMouse) const
    {
        return m_tMouseState.rgbButtons[eMouse];
    }

    _bool Get_DIMouseDown(MOUSEKEYSTATE eMouse) const
    {
        return (m_tMouseState.rgbButtons[eMouse] & 0x80) && !(m_PreMouseState[eMouse] & 0x80);
    }

    _bool Get_DIKeyDown(_ubyte byKeyID) const
    {
        return !(m_PreKeyState[byKeyID] & 0x80) && (m_byKeyState[byKeyID] & 0x80);
    }

    _byte Get_DIMouseUp(MOUSEKEYSTATE eMouse)
    {
        return !m_tMouseState.rgbButtons[eMouse] && m_PreMouseState[eMouse];
    }

    _bool Get_DIAnyKey() const
    {
        for (_uint i = 0; i < 256; ++i)
        {
            if (!(m_PreKeyState[i] & 0x80) && (m_byKeyState[i] & 0x80))
                return true;
        }
        return false;
    }

    _long Get_DIMouseMove(MOUSEMOVESTATE eMouseState) const
    {
        switch (eMouseState)
        {
        case MOUSEMOVESTATE::DIMS_X: return m_tMouseState.lX;
        case MOUSEMOVESTATE::DIMS_Y: return m_tMouseState.lY;
        case MOUSEMOVESTATE::DIMS_Z: return m_tMouseState.lZ;
        default: return 0;
        }
    }

    _bool Mouse_Fix(_bool Fix);

public:
    HRESULT Initialize(HINSTANCE hInst, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
    void Update_InputDev();

public:
    static CInput_Device* Create(HINSTANCE hInst, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
    virtual void Free(void);

private:
    // DirectInput
    LPDIRECTINPUT8 m_pInputSDK = nullptr;
    LPDIRECTINPUTDEVICE8 m_pKeyBoard = nullptr;
    LPDIRECTINPUTDEVICE8 m_pMouse = nullptr;

    // 상태 저장
    _byte m_byKeyState[256] = {};
    _byte m_PreKeyState[256] = {};
    _byte m_PreMouseState[3] = {};
    DIMOUSESTATE m_tMouseState = {};

    // 윈도우 정보
    HWND m_hWnd = {};
    _uint m_iWinSizeX = 0;
    _uint m_iWinSizeY = 0;

    // 마우스 고정 여부
    _bool m_bMouseFixed = false;
};

END
#endif // InputDev_h__
