#include "ThreadPool.h"

CThreadPool::CThreadPool()
{
}

HRESULT CThreadPool::Initialize(_uint iNumThread)
{
    m_iNumThread = iNumThread;

    m_vecThread_Working.resize(m_iNumThread);
    for (auto& working : m_vecThread_Working) working = make_shared<std::atomic<_bool>>(false);


    for (_uint i = 0; i < m_iNumThread; ++i)
    {
        m_vecThread.emplace_back([this, i] {
                this->InitTLS();
           this-> Work_thread(i); });
    }

    return S_OK;
}

void CThreadPool::Work_thread(_int iIndex)
{
    while (true)
    {
        function<void()> Job;
        {
            unique_lock<mutex> lock(m_Job_Mutex);
            m_Job_Condition.wait(lock, [this] { return !this->m_Job_queue.empty() || m_bStop.load(); });

            if (m_bStop.load() && this->m_Job_queue.empty())
                return;

            Job = move(m_Job_queue.front());
            m_Job_queue.pop();
            lock.unlock();
        }

        m_vecThread_Working[iIndex]->store(true);

        Job();
        
        m_vecThread_Working[iIndex]->store(false);
    }
}

_bool CThreadPool::Finish_Job()
{
    unique_lock<mutex> jobLock(m_Job_Mutex);
    if (!this->m_Job_queue.empty())
        return false;
   
    for (auto& working : m_vecThread_Working)
    {
        if (working->load())
            return false;
    }
    return true;
}

void CThreadPool::InitTLS()
{
    static atomic<unsigned __int32> SThreadID = 1;
    g_ThreadlD = SThreadID.fetch_add(1);
}

CThreadPool* CThreadPool::Create(_uint iNumThread)
{
    CThreadPool* pInstance = new CThreadPool();

    if (FAILED(pInstance->Initialize(iNumThread)))
    {
        MSG_BOX("Failed To Created : CThreadPool");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CThreadPool::Free()
{
    m_bStop.store(true);
            
       {
           unique_lock<mutex> lock(m_Job_Mutex);
           m_Job_Condition.notify_all();
       }
        for (thread& t : m_vecThread)
        {
            if (t.joinable())
                t.join();
        }

        m_vecThread.clear();
        m_vecThread_Working.clear();
    
}

