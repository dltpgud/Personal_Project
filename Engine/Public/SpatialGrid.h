#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)
class CGameObject;
class Collider_Manager;
class CSpatialGrid final : public CBase
{
public:
    struct Cell
    {

        vector<CGameObject*> Obj;
    };

public:
    CSpatialGrid() = default;
    virtual ~CSpatialGrid() = default;
    void ClearDynamic();
    void Clear();
    void SetWorld(const _float2& vMin, const _float2& vMax, _float cellSize);
    _bool WorldToCell(const _vector& pos, _int& ix, _int& iz) const;
    _int CellIndex(_int ix, _int iz) const;
    void GatherNeighborCells(_int ix, _int iz, _int outIdx[9], _int& outCount) const;
    void QueryNearby(const _vector& pos, _float range, OUT vector<CGameObject*>& out, _uint groupMask) const;
    void BuildStaticGrid(const list<CGameObject*>& staticObjs);
    void UpdateDynamicGrid(const list<CGameObject*>& dynamicObjs);

    vector<Cell>& DynamicGrid() 
    {
        return m_DynamicGrid;
    }

#ifdef _DEBUG
    _float2  GetWorldMin();
    _float2  GetWorldMax(); 
    _float   GetCellSize();
#endif // _DEBUG

private:
    _float2 m_WorldMin{}, m_WorldMax{};
    _float m_CellSize = 10.f;
    _int m_GridW = 0, m_GridH = 0; // X, Z
    vector<Cell> m_StaticGrid;
    vector<Cell> m_DynamicGrid;

};

END
