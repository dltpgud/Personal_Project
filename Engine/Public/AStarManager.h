
#pragma once
#include "Base.h"
#include "Cell.h"

BEGIN(Engine)
class CNavigation; // 앞으로 콜백해줄 거라 미리 선언

class CAStar_Manager final : public CBase
{
private:
    struct AStarNode
    {
        _uint index;
        _float g;
        _float h;
        _uint parent;
        _bool operator>(const AStarNode& other) const
        {
            return (g + h) > (other.g + other.h);
        }
    };

    struct PathJob
    {
        CNavigation* pOwner;
        _uint start;
        _uint goal;
    };
    CAStar_Manager();
    virtual ~CAStar_Manager() = default;
   
public:

    // 네비게이션 원본 셀을 등록해두는 함수 (게임 시작 시 1번만)
    void Set_NavMesh(const vector<CCell*>& cells)
    {
        // 불변 데이터로 쓸 거라 그냥 포인터만 들고 있음
        m_pCells = &cells;
    }

    // 비동기 요청
    void Request_Path(CNavigation* pOwner, _vector GolPos);
    void Start_AIUpdateLoop();
    void Stop_AIUpdateLoop();

private:
    HRESULT Initialize_Prototype();

    // 실제 A* 로직 (동기)
    vector<_uint> FindPath_Internal(_uint startIndex, _uint goalIndex);

private:
    const vector<CCell*>* m_pCells = nullptr; // 공유 불변 navmesh
    class CGameInstance* m_pGameInstance{};
    std::mutex m_jobMutex;
    std::vector<PathJob> m_pendingJobs;

    std::atomic<bool> m_bRunning = false;
    std::thread m_AIThread;
    std::condition_variable m_cv;
    
public:
    static CAStar_Manager* Create();
    virtual void Free() override;
};
END