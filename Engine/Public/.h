#pragma once
//  [\\\\\\\\] [\\\\\\\\] [RRRRRRRR] [RRRRRRRR]
// W->W (O), W->R(O), R->W(X)
#include "Engine_Defines.h"

class CSpinLock 
{   
    enum : unsigned __int32
    {
        AQULRE_TIMEOUT_TICK = 10000,    // 최대로 기다려줄 틱
        MAX_SPIN_COUNT = 5000,          //  최대 스핀카운트 값
        WRITE_THREAD_MASK = 0xFFFF0000, // 비트 플래그 방식으로 사용할 것임으로 상위 16비트만 뽑아오기 위한 마스크
        READ_COUNT_MASK = 0x0000FFFF,   // 하위 16비트만을 뽑아오기 위한 마스크
        EMPTY_FLAG = 0x00000000
    };

public:
    void WriteLock(const char* name);
    void WriteUnLock(const char* name);
    void ReadLock(const char* name);
    void ReadUnLock(const char* name);

private:
    std::atomic<unsigned __int32> m_lockFlag = EMPTY_FLAG;
    unsigned __int16 m_WirteCount = 0;
};


    /*-------------------------LOCK_GARDS-----------------*/
// RAII 패턴

class CReadLockGuard 
{
public:
    CReadLockGuard(CSpinLock& lock, const char* name) : m_lock(lock), m_name(name)
    {
        m_lock.ReadLock(name);
    }
    ~CReadLockGuard()
    {
        m_lock.ReadUnLock(m_name);
    }

private:
    CSpinLock& m_lock;
    const char* m_name;
};


class CWriteLockGuard 
{
public:
    CWriteLockGuard(CSpinLock& lock, const char* name) : m_lock(lock), m_name(name)
    {
        m_lock.WriteLock(name);
    }
    ~CWriteLockGuard()
    {
        m_lock.WriteUnLock(m_name);
    }

private:
    CSpinLock& m_lock;
    const char* m_name;
};