#include "SpatialGrid.h"
#include "GameObject.h"
#include "VIBuffer_Terrain.h"
#include "Collider_Manager.h"
void CSpatialGrid::ClearDynamic()
{
    for (auto& obj : m_DynamicGrid)
    {
        obj.Obj.clear(); // 동적 객체 리스트 비우기

    }
}
void CSpatialGrid::Clear()
{
     for (auto& obj : m_DynamicGrid)
     { 
         obj.Obj.clear();

     }

    for (auto& obj : m_StaticGrid)
    {
         obj.Obj.clear();
    }
}
void CSpatialGrid::SetWorld(const _float2& vMin, const _float2& vMax, _float cellSize)
{
    m_WorldMin = vMin;
    m_WorldMax = vMax;
    m_CellSize = max(0.1f, cellSize);

    const _float width = vMax.x - vMin.x;
    const _float depth = vMax.y - vMin.y; // vMax.y는 Z축을 의미

    m_GridW = static_cast<int>(ceilf(width / m_CellSize));
    m_GridH = static_cast<int>(ceilf(depth / m_CellSize));

    m_StaticGrid.resize(m_GridW * m_GridH);
    m_DynamicGrid.resize(m_GridW * m_GridH);
}

void CSpatialGrid::BuildStaticGrid(const list<CGameObject*>& staticObjs)
{
    for (auto* obj : staticObjs)
    {
        if (!obj)
            continue;

        auto* pTerrain = dynamic_cast<CVIBuffer_Terrain*>(obj->Find_Component(TEXT("Com_Buffer")));
        if (pTerrain)
        {
            // Terrain의 World AABB 계산
            AABB aabb = pTerrain->Get_WorldAABB(obj->Get_Transform());

            // 커버하는 셀 영역 계산
            _int minX = static_cast<_int>((aabb.min.x - m_WorldMin.x) / m_CellSize);
            _int maxX = static_cast<_int>((aabb.max.x - m_WorldMin.x) / m_CellSize);
            _int minZ = static_cast<_int>((aabb.min.z - m_WorldMin.y) / m_CellSize);
            _int maxZ = static_cast<_int>((aabb.max.z - m_WorldMin.y) / m_CellSize);

            minX = max(0, minX);
            minZ = max(0, minZ);
            maxX = min(m_GridW - 1, maxX);
            maxZ = min(m_GridH - 1, maxZ);

            for (_int z = minZ; z <= maxZ; ++z)
            {
                for (_int x = minX; x <= maxX; ++x) { m_StaticGrid[CellIndex(x, z)].Obj.push_back(obj); }
            }
        }
        else
        {
            _int ix, iz;
            if (WorldToCell(obj->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION), ix, iz))
                m_StaticGrid[iz * m_GridW + ix].Obj.push_back(obj);
        }
    }
}

void CSpatialGrid::UpdateDynamicGrid(const list<CGameObject*>& dynamicObjs)
{
    for (auto* obj : dynamicObjs)
    {
        if (!obj)
            continue;
        int ix, iz;
        if (WorldToCell(obj->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION), ix, iz))
        {
            m_DynamicGrid[iz * m_GridW + ix].Obj.push_back(obj);
        }
    }
}

#ifdef _DEBUG
_float2 CSpatialGrid::GetWorldMin()
{
    return m_WorldMin;
}

_float2 CSpatialGrid::GetWorldMax()
{
    return m_WorldMax;
}

_float CSpatialGrid::GetCellSize()
{
    return m_CellSize;
}
#endif // _DEBUG

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
    for (int dz = -1; dz <= 1; ++dz)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            int nx = ix + dx, nz = iz + dz;
            if (nx < 0 || nz < 0 || nx >= m_GridW || nz >= m_GridH)
                continue;
            outIdx[outCount++] = nz * m_GridW + nx;
        }
    }
}


void CSpatialGrid::QueryNearby(const _vector& pos, _float range, OUT vector<CGameObject*>& out, _uint groupType) const
{
    out.clear();
    out.reserve(64);  
    _int ix, iz;
    if (!WorldToCell(pos, ix, iz))
        return;

    _int r = static_cast<int>(ceil(range / m_CellSize));
    for (int dz = -r; dz <= r; ++dz)
    {
        for (int dx = -r; dx <= r; ++dx)
        {
            int nx = ix + dx, nz = iz + dz;
            if (nx < 0 || nz < 0 || nx >= m_GridW || nz >= m_GridH)
                continue;

            const Cell& sc = m_StaticGrid[nz * m_GridW + nx];
            const Cell& dc = m_DynamicGrid[nz * m_GridW + nx];

            if (groupType == Collider_Manager::COL_MONSTER)
                out.insert(out.end(), dc.Obj.begin(), dc.Obj.end());
            else if (groupType == Collider_Manager::COL_STATIC)
                out.insert(out.end(), sc.Obj.begin(), sc.Obj.end());
            else if (groupType == Collider_Manager::COL_MONSTER_SKILL)
                out.insert(out.end(), dc.Obj.begin(), dc.Obj.end());
        }
    }
}



