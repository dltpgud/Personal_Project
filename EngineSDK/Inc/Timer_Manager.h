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
    _float*                 Get_TimeDeltaSum(const _wstring& pTimerTag);

public:
	HRESULT					Add_Timer(const _wstring& strTimerTag);
	void					Update_TimeDelta(const _wstring& strTimerTag);
    void                    BeginFrameStamp(const _wstring& strTimerTag);
    _int                    GetFrameStamp(const _wstring& strTimerTag) ;
    private:
	CTimer*					Find_Timer(const _wstring& strTimerTag);

private:
	map<const _wstring, CTimer*>			m_mapTimer;

public:
	static CTimer_Manager*	Create();
	virtual void			Free(void) override;
};

END
