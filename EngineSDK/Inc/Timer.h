#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer : public CBase
{
private:
	 CTimer(void);
	virtual ~CTimer(void) = default;

public:
	_float					Get_TimeDelta(void) const { return m_fTimeDelta; }
#ifndef _DEBUG
	void					Get_FPS(HWND g_hWnd)
	{
		if (m_ftimeAcc > 1.f)
		{
			m_ftimeAcc = 0.f;
			swprintf_s(m_szFPS, L"FPS : %d", m_iFrameCount);
			SetWindowText(g_hWnd, m_szFPS);
			m_iFrameCount = 0;
		}
	}
#endif
public:
	HRESULT					Ready_Timer(void);
	void					Update_Timer(void);
	
private:
	LARGE_INTEGER			m_FrameTime{};
	LARGE_INTEGER			m_FixTime{};
	LARGE_INTEGER			m_LastTime{};
	LARGE_INTEGER			m_CpuTick{};

	_int					m_iFrameCount{ 0 };
	_float					m_fFrame{ 0.0 };
	_float					m_ftimeAcc{ 0.0 };
	TCHAR					m_szFPS[32]{};
private:
	_float					m_fTimeDelta{};

public:
	static CTimer*			Create(void);
	virtual void			Free(void) override;

};

END

