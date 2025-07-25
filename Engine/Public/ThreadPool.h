#pragma once
#include "Base.h"
#include "TLS.h"
BEGIN(Engine)
class CThreadPool : public CBase
{
public:
private:
    CThreadPool();
    virtual ~CThreadPool() = default;

public:
    HRESULT Initialize(_uint iNumThread);

    template <class T, class... Args>
    auto Add_Job(T&& f, Args&&... args) -> future<typename result_of<T(Args...)>::type>;

    void Work_thread(_int iIndex);
    _bool Finish_Job();
    void InitTLS();


private:

    _uint m_iNumThread{};

    vector<thread> m_vecThread;

      vector<std::shared_ptr<std::atomic<_bool>>> m_vecThread_Working;


    mutex m_Job_Mutex;
    condition_variable m_Job_Condition;

    atomic<_bool> m_bStop{};

    queue<function<void()>> m_Job_queue;

public:
    static CThreadPool* Create(_uint iNumThread);
    virtual void Free();
};

END

template <class T, class... Args>
    auto CThreadPool::Add_Job(T&& f, Args&&... args) -> std::future<typename std::result_of<T(Args...)>::type>
{
    using Result = typename std::result_of<T(Args...)>::type;

    auto task =
        std::make_shared<std::packaged_task<Result()>>(std::bind(std::forward<T>(f), std::forward<Args>(args)...));

    std::future<Result> ret_future = task->get_future();

    {
        std::unique_lock<std::mutex> lock(m_Job_Mutex);

        if (m_bStop.load())
            throw std::runtime_error("enqueue on stopped ThreadPool");

        m_Job_queue.emplace([task]() { (*task)(); });
    }

    m_Job_Condition.notify_one();

    return ret_future;
}

