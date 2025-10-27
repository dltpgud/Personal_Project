#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)
class CGameObject;

class CSpatialGrid final : public CBase
{
public:
    struct Cell
    {
      vector<CGameObject*> monsters;
      vector<CGameObject*> decals;
      vector<CGameObject*> skills;
    };

public:
    CSpatialGrid() = default;
    virtual ~CSpatialGrid() = default;

    void SetWorld(const _float2& vMin, const _float2& vMax, _float cellSize);
    void Clear();
    void Rebuild(const list<CGameObject*>& decals, const list<CGameObject*>& monsters,
                 const list<CGameObject*>& skills);

    _bool WorldToCell(const _vector& pos, _int& ix, _int& iz) const;
    _int CellIndex(_int ix, _int iz) const;
    void GatherNeighborCells(_int ix, _int iz, _int outIdx[9], _int& outCount) const;

    vector<Cell>& Grid()
    {
        return m_Grid;
    }
    const vector<Cell>& Grid() const
    {
        return m_Grid;
    }


private:
    _float2 m_WorldMin{}, m_WorldMax{};
    _float m_CellSize = 10.f;
    _int m_GridW = 0, m_GridH = 0; // X, Z
    vector<Cell> m_Grid;
};

END
