#include "SpatialGrid.h"
#include "GameObject.h"

void CSpatialGrid::SetWorld(const _float2& vMin, const _float2& vMax, _float cellSize)
{
    m_WorldMin = vMin;
    m_WorldMax = vMax;
    m_CellSize = max(0.1f, cellSize);

    const _float width = vMax.x - vMin.x;
    const _float depth = vMax.y - vMin.y; // vMax.y는 Z축을 의미

    m_GridW = static_cast<int>(ceilf(width / m_CellSize));
    m_GridH = static_cast<int>(ceilf(depth / m_CellSize));

    m_Grid.clear();
    m_Grid.resize(max(0, m_GridW * m_GridH));
}

void CSpatialGrid::Clear()
{
    for (auto& c : m_Grid)
    {
        c.decals.clear();
        c.monsters.clear();
        c.skills.clear();
    }
}

_bool CSpatialGrid::WorldToCell(const _vector& pos, _int& ix, _int& iz) const
{
    const _float x = XMVectorGetX(pos);
    const _float z = XMVectorGetZ(pos);

    ix = static_cast<_int>((x - m_WorldMin.x) / m_CellSize);
    iz = static_cast<_int>((z - m_WorldMin.y) / m_CellSize);

    if (ix < 0 || iz < 0 || ix >= m_GridW || iz >= m_GridH)
        return false;
    return true;
}

_int CSpatialGrid::CellIndex(_int ix, _int iz) const
{
    return iz * m_GridW + ix;
}

void CSpatialGrid::GatherNeighborCells(_int ix, _int iz, _int outIdx[9], _int& outCount) const
{
    outCount = 0;
    for (_int dz = -1; dz <= 1; ++dz)
    {
        for (_int dx = -1; dx <= 1; ++dx)
        {
            _int nx = ix + dx;
            _int nz = iz + dz;
            if (nx < 0 || nz < 0 || nx >= m_GridW || nz >= m_GridH)
                continue;
            outIdx[outCount++] = CellIndex(nx, nz);
        }
    }
}

void CSpatialGrid::Rebuild(const list<CGameObject*>& decals, const list<CGameObject*>& monsters,
                           const list<CGameObject*>& skills)
{
    if (m_Grid.empty())
        SetWorld({-500.f, -500.f}, {500.f, 500.f}, m_CellSize);

    Clear();

    auto pushToCell = [&](CGameObject* obj, auto picker)
    {
        if (!obj)
            return;
        const _vector p = obj->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
        _int ix, iz;
        if (!WorldToCell(p, ix, iz))
            return;
        picker(m_Grid[CellIndex(ix, iz)]).push_back(obj);
    };

for (auto* o : decals)
    {
        if (!o)
            continue;

        pushToCell(o, [](Cell& c) -> auto& { return c.decals; });
    }
    for (auto* o : monsters)
        pushToCell(o, [](Cell& c) -> auto& { return c.monsters; });
    for (auto* o : skills)
        pushToCell(o, [](Cell& c) -> auto& { return c.skills; });

}
