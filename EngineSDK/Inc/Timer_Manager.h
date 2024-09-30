#pragma once

#include "Timer.h"
#include "Base.h"

BEGIN(Engine)

class CTimer_Manager : public CBase
{
private:
	CTimer_Manager(void);
	virtual ~CTimer_Manager(void) = default;

public:
	_float					Get_TimeDelta(const _wstring& pTimerTag);
#ifndef _DEBUG
	void					Get_FPS(const _wstring& pTimerTag, HWND g_hWnd);
#endif
public:
	HRESULT					Add_Timer(const _wstring& strTimerTag);
	void					Update_TimeDelta(const _wstring& strTimerTag);
	
private:
	CTimer*					Find_Timer(const _wstring& strTimerTag);

private:
	map<const _wstring, CTimer*>			m_mapTimer;

public:
	static CTimer_Manager*	Create();
	virtual void			Free(void) override;
};

END
