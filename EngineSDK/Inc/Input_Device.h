#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;
	
public:
	_byte	Get_DIKeyState(_ubyte byKeyID)		
	{ 
		return m_byKeyState[byKeyID]; 
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) 
	{ 	
		return m_tMouseState.rgbButtons[eMouse]; 	
	}

	_byte	Get_DIMouseDown(MOUSEKEYSTATE eMouse) {
	
		return m_tMouseState.rgbButtons[eMouse] && !m_PreMouseState[eMouse];
	}

	_byte  Get_DIKeyDown(_ubyte byKeyID)
	{
		return !m_PreKeyState[byKeyID] && m_byKeyState[byKeyID];
	}

	_byte	Get_DIAnyKey() {
		for (_uint i = 0; i < 256; ++i) {
			if (!m_PreKeyState[i] && m_byKeyState[i]) { return !m_PreKeyState[i] && m_byKeyState[i]; }
		}
		return false;
	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)	
	{	
		return *(((_long*)&m_tMouseState) + eMouseState);	
	}
	
public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);
private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

private:
	_byte					m_byKeyState[256] = {};		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte					m_PreMouseState[3]{};
	_byte					m_PreKeyState[256]{};
	DIMOUSESTATE			m_tMouseState = {};

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
END
#endif // InputDev_h__
