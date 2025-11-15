// AStar_Manager.cpp
#include "AStarManager.h"
#include "Navigation.h"
#include "GameInstance.h"

CAStar_Manager::CAStar_Manager() : m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

void CAStar_Manager::Set_Mode(Mode mode)
{
    m_Mode = mode;
}

void CAStar_Manager::Request_Path(CNavigation* pOwner, _vector GolPos)
{
    if (nullptr == m_pCells || nullptr == pOwner)
        return;

    _uint start = pOwner->Get_CurrentCellIndex();
    _uint goal = pOwner->Find_Cell_ByPosition(GolPos);

    switch (m_Mode)
    {
    case Mode::SingleThread:
    {
        auto begin = std::chrono::high_resolution_clock::now();

        vector<_uint> path = this->FindPath_Internal(start, goal);
        pOwner->OnPathReady(path);

        auto end = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - begin).count();

        if (m_pGameInstance->Get_DIKeyDown(DIK_9))
        std::cout << "[A* SingleThread] Time: " << ms << " ms\n";

        break;
    }

    case Mode::ThreadPool:
    {
        auto begin = std::chrono::high_resolution_clock::now();

        // ThreadPool job 하나만 넣기
        m_pGameInstance->Add_Job(
            [this, pOwner, start, goal, begin]()
            {
                vector<_uint> path = this->FindPath_Internal(start, goal);
                pOwner->OnPathReady(path);

                auto end = std::chrono::high_resolution_clock::now();
                double ms = std::chrono::duration<double, std::milli>(end - begin).count();
                if (m_pGameInstance->Get_DIKeyDown(DIK_9))
                std::cout << "[A* ThreadPool] Time: " << ms << " ms\n";
            });

        break;
    }
    }
}

void CAStar_Manager::Start_AIUpdateLoop()
{
    if (m_bRunning)
        return;
    m_bRunning = true;

    m_AIThread = std::thread(
        [this]()
        {
            while (m_bRunning)
            {
                std::vector<PathJob> jobs;
                {
                    std::unique_lock<std::mutex> lock(m_jobMutex);
                    m_cv.wait(lock, [this] { return !m_pendingJobs.empty() || !m_bRunning; });
                    if (!m_bRunning)
                        break;

                    jobs.swap(m_pendingJobs);
                }

                if (m_Mode == Mode::SingleThread)
                {
                    auto begin = std::chrono::high_resolution_clock::now();

                    for (auto& job : jobs)
                    {
                        vector<_uint> path = FindPath_Internal(job.start, job.goal);
                        job.pOwner->OnPathReady(path);
                    }

                    auto end = std::chrono::high_resolution_clock::now();
                    double ms = std::chrono::duration<double, std::milli>(end - begin).count();
                    if (m_pGameInstance->Get_DIKeyDown(DIK_9))
                    std::cout << "[A* AIThread Single] Jobs: " << jobs.size() << "  Time: " << ms << " ms\n";
                }

                else if (m_Mode == Mode::ThreadPool)
                {
                    std::vector<std::function<void()>> tasks;
                    tasks.reserve(jobs.size());

                    for (auto& job : jobs)
                    {
                        tasks.emplace_back(
                            [this, job]()
                            {
                                vector<_uint> path = FindPath_Internal(job.start, job.goal);
                                job.pOwner->OnPathReady(path);
                            });
                    }

                    auto begin = std::chrono::high_resolution_clock::now();

                    m_pGameInstance->Add_Jobs(std::move(tasks));

                    while (!m_pGameInstance->AllJobCompleted()) { std::this_thread::yield(); }

                    auto end = std::chrono::high_resolution_clock::now();
                    double ms = std::chrono::duration<double, std::milli>(end - begin).count();
                    if (m_pGameInstance->Get_DIKeyDown(DIK_9))
                    std::cout << "[A* AIThread ThreadPool] Jobs: " << jobs.size() << "  Time: " << ms << " ms\n";
                }
            }
        });
}

void CAStar_Manager::Stop_AIUpdateLoop()
{
    m_bRunning = false;
    m_cv.notify_all();
    if (m_AIThread.joinable())
        m_AIThread.join();
}

HRESULT CAStar_Manager::Initialize_Prototype()
{
    Start_AIUpdateLoop();
    return S_OK;
}

vector<_uint> CAStar_Manager::FindPath_Internal(_uint startIndex, _uint goalIndex)
{
    vector<_uint> empty;
    if (nullptr == m_pCells)
        return empty;
    if (goalIndex >= m_pCells->size())
        return empty;

    priority_queue<AStarNode, std::vector<AStarNode>, std::greater<>> openList;
    unordered_map<_uint, float> costSoFar;
    unordered_map<_uint, _uint> cameFrom;

    openList.push({startIndex, 0.f, 0.f, startIndex});
    costSoFar[startIndex] = 0.f;
    cameFrom[startIndex] = startIndex;

    const CCell* goalCell = (*m_pCells)[goalIndex];
    _vector goalCenter = goalCell->GetCenter();

    while (!openList.empty())
    {
        AStarNode current = openList.top();
        openList.pop();

        if (current.index == goalIndex)
        {
            vector<_uint> path;
            _uint cur = goalIndex;
            while (cur != cameFrom[cur])
            {
                path.push_back(cur);
                cur = cameFrom[cur];
            }
            path.push_back(startIndex);
            std::reverse(path.begin(), path.end());
            return path;
        }

        const CCell* currCell = (*m_pCells)[current.index];
        _vector currCenter = currCell->GetCenter();

        for (_int i = 0; i < CCell::LINE_END; ++i)
        {
            _int neighborIndex = currCell->Get_Neighbors(i);
            if (neighborIndex < 0)
                continue;

            const CCell* neighborCell = (*m_pCells)[neighborIndex];
            _vector neighborCenter = neighborCell->GetCenter();

            _float stepCost = XMVectorGetX(DirectX::XMVector3Length(neighborCenter - currCenter));
            _float newG = costSoFar[current.index] + stepCost;

            auto it = costSoFar.find(neighborIndex);
            if (it == costSoFar.end() || newG < it->second)
            {
                costSoFar[neighborIndex] = newG;
                _float h = XMVectorGetX(DirectX::XMVector3Length(goalCenter - neighborCenter));

                openList.push({static_cast<_uint>(neighborIndex), newG, h, current.index});
                cameFrom[neighborIndex] = current.index;
            }
        }
    }

    return empty;
}

CAStar_Manager* CAStar_Manager::Create()
{
    CAStar_Manager* pInstance = new CAStar_Manager();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CAStar_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CAStar_Manager::Free()
{
    Stop_AIUpdateLoop();
    Safe_Release(m_pGameInstance);
}
