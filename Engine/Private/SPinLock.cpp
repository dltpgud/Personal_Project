#include "SPinLock.h"
#include "TLS.h"
void CSpinLock::WriteLock(const char* name)
{
    const unsigned __int32 lockThreadId = (m_lockFlag.load() & WRITE_THREAD_MASK) >> 16;

    if (g_ThreadlD == lockThreadId)
    {
        m_WirteCount++;
        return;
    }

    const int64 beginTick = GetTickCount64();

    // 아무도 소유 및 공유하고 있지 않을 때,경합해서 소유권을 얻는다.
    const unsigned __int32 desired = ((g_ThreadlD << 16) & WRITE_THREAD_MASK);
    while (true)
    {
        for (unsigned __int32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) // 이안에서 락을 획득해봐라..
        {
            unsigned __int32 expected = EMPTY_FLAG;
            if (m_lockFlag.compare_exchange_strong(OUT expected, desired)) // 락 획득
            {
                m_WirteCount++; // 카운트를 늘려 재귀 호출에도 한 번 더 잡게 허용
                return;
            }
        }

        if (GetTickCount64() - beginTick >= AQULRE_TIMEOUT_TICK)
            CRASH("LOCK_TIMEOUT");

        this_thread::yield(); // 실패 다른 쓰레드에게 양보
    }
}

void CSpinLock::WriteUnLock(const char* name)
{
    // Readlock를 다 풀기 전에는 WriteUnlock 불가능

    if ((m_lockFlag.load() & READ_COUNT_MASK) != 0) // 리드 카운트가 0이 아니라는 건 누군가 쓰고 있는거니까 문제
        CRASH("INVALID_UNLOCK_ORDER");

    const __int32 lockCount = --m_WirteCount;

    if (0 == m_WirteCount)
        m_lockFlag.store(EMPTY_FLAG);
}

void CSpinLock::ReadLock(const char* name)
{
    // 동일한 쓰레드가 소유하고 있다면 무조건 성공
    const unsigned __int32 lockThreadId = (m_lockFlag.load() & WRITE_THREAD_MASK) >> 16;

    if (g_ThreadlD == lockThreadId)
    {
        m_lockFlag.fetch_add(1); // 읽기 락을 1나 추가..
        return;
    }

    // 아무도 Write를 잡고 있지 않을 때 경합해서 공유 카운트(READ 비트)를 올린다.
    const __int64 beginTick = GetTickCount64();
    while (true)
    {
        for (unsigned __int32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
        {
            unsigned __int32 expected = (m_lockFlag.load() & READ_COUNT_MASK); // Write falg를 의도적으로 0으로 민다.
            if (m_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
                return;
        }

        if (GetTickCount64() - beginTick >= AQULRE_TIMEOUT_TICK)
            CRASH("LOCK_TIMEOUT");

        this_thread::yield();
    }
}

void CSpinLock::ReadUnLock(const char* name)
{
    if ((m_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0) // read lock을 해제 하면서 현재 쓰레드가 마지막 리더였는지
        CRASH("MULTPLE_UNLOCK");
}
