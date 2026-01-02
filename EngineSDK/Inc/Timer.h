#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer : public CBase
{
private:
	 CTimer();
	virtual ~CTimer() = default;

public:
	_float					Get_TimeDelta() const { return m_fTimeDelta; }
		_float*			    Get_TimeDeltaSum()  { return &m_fTimeDeltaSum; }
public:
	HRESULT					Ready_Timer();
	void					Update_Timer();
    void                    BeginFrameStamp() {++m_uTick;}
	_int                    GetFrameStamp() const { return m_uTick; };
private:
	LARGE_INTEGER			m_FrameTime{};
	LARGE_INTEGER			m_FixTime{};
	LARGE_INTEGER			m_LastTime{};
	LARGE_INTEGER			m_CpuTick{};

private:
	_float					m_fTimeDelta{};
    _float                  m_fTimeDeltaSum{};
    _int m_uTick = 0;

public:
	static CTimer*			Create();
	virtual void			Free() override;

};

END

