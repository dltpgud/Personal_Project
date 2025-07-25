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

    // �ƹ��� ���� �� �����ϰ� ���� ���� ��,�����ؼ� �������� ��´�.
    const unsigned __int32 desired = ((g_ThreadlD << 16) & WRITE_THREAD_MASK);
    while (true)
    {
        for (unsigned __int32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) // �̾ȿ��� ���� ȹ���غ���..
        {
            unsigned __int32 expected = EMPTY_FLAG;
            if (m_lockFlag.compare_exchange_strong(OUT expected, desired)) // �� ȹ��
            {
                m_WirteCount++; // ī��Ʈ�� �÷� ��� ȣ�⿡�� �� �� �� ��� ���
                return;
            }
        }

        if (GetTickCount64() - beginTick >= AQULRE_TIMEOUT_TICK)
            CRASH("LOCK_TIMEOUT");

        this_thread::yield(); // ���� �ٸ� �����忡�� �纸
    }
}

void CSpinLock::WriteUnLock(const char* name)
{
    // Readlock�� �� Ǯ�� ������ WriteUnlock �Ұ���

    if ((m_lockFlag.load() & READ_COUNT_MASK) != 0) // ���� ī��Ʈ�� 0�� �ƴ϶�� �� ������ ���� �ִ°Ŵϱ� ����
        CRASH("INVALID_UNLOCK_ORDER");

    const __int32 lockCount = --m_WirteCount;

    if (0 == m_WirteCount)
        m_lockFlag.store(EMPTY_FLAG);
}

void CSpinLock::ReadLock(const char* name)
{
    // ������ �����尡 �����ϰ� �ִٸ� ������ ����
    const unsigned __int32 lockThreadId = (m_lockFlag.load() & WRITE_THREAD_MASK) >> 16;

    if (g_ThreadlD == lockThreadId)
    {
        m_lockFlag.fetch_add(1); // �б� ���� 1�� �߰�..
        return;
    }

    // �ƹ��� Write�� ��� ���� ���� �� �����ؼ� ���� ī��Ʈ(READ ��Ʈ)�� �ø���.
    const __int64 beginTick = GetTickCount64();
    while (true)
    {
        for (unsigned __int32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
        {
            unsigned __int32 expected = (m_lockFlag.load() & READ_COUNT_MASK); // Write falg�� �ǵ������� 0���� �δ�.
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
    if ((m_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0) // read lock�� ���� �ϸ鼭 ���� �����尡 ������ ����������
        CRASH("MULTPLE_UNLOCK");
}
