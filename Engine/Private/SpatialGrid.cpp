#include "SpatialGrid.h"
#include "GameObject.h"
#include "VIBuffer_Terrain.h"
#include "Collider_Manager.h"
#include "GameInstance.h"

CSpatialGrid::CSpatialGrid() 
{
}
void CSpatialGrid::Clear()
{
    for (auto& obj : m_DynamicGrid) { obj.Obj.clear(); }
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
   
    m_DynamicGrid.resize(m_GridW * m_GridH);
    
}

void CSpatialGrid::UpdateDynamicGrid(const list<CGameObject*>& dynamicObjs)
{
    for (auto* obj : dynamicObjs)
    {
        if (!obj)
            continue;
        _int ix, iz;
        if (WorldToCell(obj->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION), ix, iz))
        {
            m_DynamicGrid[iz * m_GridW + ix].Obj.push_back(obj);
        }
    }
}

_float CSpatialGrid::GetCellSize()
{
    return m_CellSize;
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
            _int nx = ix + dx, nz = iz + dz;
            if (nx < 0 || nz < 0 || nx >= m_GridW || nz >= m_GridH)
                continue;
            outIdx[outCount++] = nz * m_GridW + nx;
        }
    }
}

void CSpatialGrid::QueryNearby(const _vector& pos, _float range, OUT vector<CGameObject*>& out) const
{
    out.clear();
    out.reserve(16);

    _int ix, iz;
    if (!WorldToCell(pos, ix, iz)) // 월드 위치 기반 셀 인덱스 반환
        return;

    _int r = static_cast<_int>(ceil(range / m_CellSize)); // 반경 몇 칸 검사 할지 범위 설정
    for (_int dz = -r; dz <= r; ++dz) // 중심 셀을 기준 월드 안 주변 r칸에 ​포함되는 모든 셀 검사​
    {
        for (_int dx = -r; dx <= r; ++dx)
        {
            _int nx = ix + dx, nz = iz + dz;
            if (nx < 0 || nz < 0 || nx >= m_GridW || nz >= m_GridH)
                continue;

            const Cell& cell = m_DynamicGrid[nz * m_GridW + nx];

            // 각 셀의 정적 오브젝트와​ 동적 오브젝트에 접근해 ​ 그룹 별 결과 반환
            for (auto* obj : cell.Obj) {
                if (!obj)
                    return;

                out.push_back(obj);
            }
        }
    }
}

void CSpatialGrid::QueryAABB(const _vector& Prepos, const _vector& Curpos, const _float& RayLen,
                             OUT vector<CGameObject*>& out) const
{
    out.clear();
    out.reserve(32);

    // 1️ Swept AABB 생성 (Prepos ↔ Curpos)
    const _float px0 = XMVectorGetX(Prepos);
    const _float pz0 = XMVectorGetZ(Prepos);
    const _float px1 = XMVectorGetX(Curpos);
    const _float pz1 = XMVectorGetZ(Curpos);

    _float minX = min(px0, px1);
    _float maxX = max(px0, px1);
    _float minZ = min(pz0, pz1);
    _float maxZ = max(pz0, pz1);

    // RayLen 보정 (투사체 / 레이용)
    minX -= RayLen;
    maxX += RayLen;
    minZ -= RayLen;
    maxZ += RayLen;

    // 2️⃣ AABB → Cell 범위 변환
    _int cellMinX = static_cast<_int>((minX - m_WorldMin.x) / m_CellSize);
    _int cellMaxX = static_cast<_int>((maxX - m_WorldMin.x) / m_CellSize);
    _int cellMinZ = static_cast<_int>((minZ - m_WorldMin.y) / m_CellSize);
    _int cellMaxZ = static_cast<_int>((maxZ - m_WorldMin.y) / m_CellSize);

    // Clamp
    cellMinX = max(0, cellMinX);
    cellMinZ = max(0, cellMinZ);
    cellMaxX = min(m_GridW - 1, cellMaxX);
    cellMaxZ = min(m_GridH - 1, cellMaxZ);

    // 3️⃣ 해당 셀들의 오브젝트 수집
    for (_int z = cellMinZ; z <= cellMaxZ; ++z)
    {
        for (_int x = cellMinX; x <= cellMaxX; ++x)
        {
            const Cell& cell = m_DynamicGrid[z * m_GridW + x];
            for (auto* obj : cell.Obj)
            {
                if (!obj)
                    continue;

                out.push_back(obj);
            }
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
#endif // _DEBUG
