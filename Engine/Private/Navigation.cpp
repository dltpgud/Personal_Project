#include "Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"

const _float4x4* CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
    : CComponent{Prototype}, m_Cells{Prototype.m_Cells}, m_vecNomoveType{Prototype.m_vecNomoveType}

#ifdef _DEBUG
      ,
      m_pShader{Prototype.m_pShader}
#endif
{
    for (auto& pCell : m_Cells) Safe_AddRef(pCell);

#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype()
{

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements,
                                VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return S_OK;

    NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

    if (nullptr != pDesc)
        m_iCurrentCellIndex = pDesc->iCurrentCellIndex;
    else
        m_iCurrentCellIndex = 0;
    return S_OK;
}

void CNavigation::SetUp_Neighbor()
{
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDestCell : m_Cells)
        {
            if (pSourCell == pDestCell) // 같은 셀(삼각형)이면 아래 조건문 넘기고..
                continue;

            if (true ==
                pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
                pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);

            if (true ==
                pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
                pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);

            if (true ==
                pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
                pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
        }
    }
}

_bool CNavigation::isMove(_fvector vAfterWorldPos, _fvector vBeforeMoveWorldPos, _vector* Slide)
{
   if (m_iCurrentCellIndex <= -1 || m_iCurrentCellIndex > m_Cells.size())
       return false;
   
   _vector vAfterLocalPos = XMVector3TransformCoord(vAfterWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_WorldMatrix)));
   _vector vBeforeLocalPos =XMVector3TransformCoord(vBeforeMoveWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_WorldMatrix)));
   
   _int iNeighborIndex = {-1};
   _bool m_breturn = {true};
   
   _vector Slid{};
   if (false == m_Cells[m_iCurrentCellIndex]->isIn(vAfterLocalPos, vBeforeLocalPos, &iNeighborIndex, &Slid))
   { 
       if (-1 != iNeighborIndex)
       {
           while (true) 
           {
               if (true == m_Cells[iNeighborIndex]->isIn(vAfterLocalPos, vBeforeLocalPos, &iNeighborIndex,&Slid))
               {
                   if (CCell::NOMOVE == m_Cells[iNeighborIndex]->Get_Type())
                   {
                       *Slide = XMVectorZero();
                       return false;
                   }
                   
                   if (CCell::SAFE == m_Cells[iNeighborIndex]->Get_Type())
                   {
                       m_vSafePos = m_Cells[iNeighborIndex]->GetCenter();
                   }
                   break;
               }
              
               if (-1 == iNeighborIndex) 
               {
                   return false;
               }
           }
           m_iCurrentCellIndex = iNeighborIndex; 
           m_breturn = true;
       }
       else
       {
           m_breturn = false;
       }
   }
   
   _vector vNewLocalPos = vBeforeLocalPos + Slid;

   if (false == m_Cells[m_iCurrentCellIndex]->isIn(vNewLocalPos, vAfterLocalPos, &iNeighborIndex, &Slid))
   {
         if (-1 != iNeighborIndex)
         {
           while (true)
           {
               if (true == m_Cells[iNeighborIndex]->isIn(vNewLocalPos, vAfterLocalPos, &iNeighborIndex, &Slid))
               {
                   if (CCell::NOMOVE == m_Cells[iNeighborIndex]->Get_Type())
                   {
                       *Slide = XMVectorZero();
                       return false;
                   }

                   if (CCell::SAFE == m_Cells[iNeighborIndex]->Get_Type())
                   {
                       m_vSafePos = m_Cells[iNeighborIndex]->GetCenter();
                   }
                   break;
               }

               if (-1 == iNeighborIndex)
               {
                   return false;
               }
           }
           m_iCurrentCellIndex = iNeighborIndex;
           m_breturn = true;
       }
       else
       {
           m_breturn = false;
       }
   
   }
   *Slide = Slid;
    return m_breturn;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
   // if (false == m_bRender)
   //     return S_OK;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (-1 == m_iCurrentCellIndex)
    {
        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
            return E_FAIL;
        _float4 TerrainCol = {0.f, 1.f, 0.f, 1.f};
        m_pShader->Bind_RawValue("g_vColor", &TerrainCol, sizeof(_float4));

        m_pShader->Begin(0);

        for (auto& pCell : m_Cells) pCell->Render();
    }
    else
    {
        _float4x4 WorldMatrix = *m_WorldMatrix;
        WorldMatrix.m[3][1] += 0.001f;
        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
            return E_FAIL;
        _float4 Obj_on_Terrain = {1.f, 0.f, 0.f, 1.f};
        m_pShader->Bind_RawValue("g_vColor", &Obj_on_Terrain, sizeof(_float4));

        m_pShader->Begin(0);

        m_Cells[m_iCurrentCellIndex]->Render();
    }

    return S_OK;
}
#endif
vector<_uint> CNavigation::FindPath(_uint startIndex, _uint goalIndex)
{
    if (goalIndex >= m_Cells.size())
        return {};

    priority_queue<AStarNode, std::vector<AStarNode>, std::greater<>> openList;
    unordered_map<_uint, _float> costSoFar; // gCost 저장용
    unordered_map<_uint, _uint> cameFrom;  // 부모 노드 저장용

    openList.push({startIndex, 0.f, 0.f, startIndex});
    costSoFar[startIndex] = 0.f;
    cameFrom[startIndex] = startIndex;

    const CCell* goalCell = m_Cells[goalIndex];
    _vector goalCenter = goalCell->GetCenter();

    while (!openList.empty())
    {
        AStarNode current = openList.top();
        openList.pop();

        if (current.index == goalIndex)
        {
            // 경로 복원
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

        const CCell* currCell = m_Cells[current.index];
        _vector currCenter = currCell->GetCenter();

        // 이웃 순회
        for (int i = 0; i < CCell::LINE_END; ++i)
        {
            _int neighborIndex = currCell->Get_Neighbors(i);
            if (neighborIndex < 0)
                continue;

            const CCell* neighborCell = m_Cells[neighborIndex];
            _vector neighborCenter = neighborCell->GetCenter();

            // 현재 노드에서 이웃 노드까지 거리
            _float stepCost = XMVectorGetX(XMVector3Length(neighborCenter - currCenter));
            _float newG = costSoFar[current.index] + stepCost;

            if (costSoFar.find(neighborIndex) == costSoFar.end() || newG < costSoFar[neighborIndex])
            {
                costSoFar[neighborIndex] = newG;

                // 휴리스틱: 목표 중점과 이웃 중점 간 거리 (유클리드 거리)
                _float h = XMVectorGetX(XMVector3Length(goalCenter - neighborCenter));

                openList.push({static_cast<_uint>(neighborIndex), newG, h, current.index});
                cameFrom[neighborIndex] = current.index;
            }
        }
    }
    return {};
}

_bool CNavigation::Snap(_fvector vP1, _fvector vP2, _vector distance)
{
    _vector difference = XMVectorAbs(XMVectorSubtract(vP1, vP2));

    return XMVector3LessOrEqual(difference, distance);
}

void CNavigation::Create_Poly(_float3 p1, _float3 p2, _float3 p3, _uint Type)
{
    _float3 pPoints[3];

    pPoints[0] = p1;
    pPoints[1] = p2;
    pPoints[2] = p3;

    _vector vP1 = XMLoadFloat3(&pPoints[0]);
    _vector vP2 = XMLoadFloat3(&pPoints[1]);
    _vector vP3 = XMLoadFloat3(&pPoints[2]);

    _vector distance = {0.5f, 0.5f, 0.5f, 1.f}; 
    if (m_Cells.size() != 0)
    {
        for (auto vec : m_Cells) 
        {
            if (true == Snap(vec->Get_Point(CCell::POINT_A), vP1, distance))
                pPoints[0] = {XMVectorGetX(vec->Get_Point(CCell::POINT_A)),
                              XMVectorGetY(vec->Get_Point(CCell::POINT_A)),
                              XMVectorGetZ(vec->Get_Point(CCell::POINT_A))};
            if (true == Snap(vec->Get_Point(CCell::POINT_B), vP1, distance))
                pPoints[0] = {XMVectorGetX(vec->Get_Point(CCell::POINT_B)),
                              XMVectorGetY(vec->Get_Point(CCell::POINT_B)),
                              XMVectorGetZ(vec->Get_Point(CCell::POINT_B))};
            if (true == Snap(vec->Get_Point(CCell::POINT_C), vP1, distance))
                pPoints[0] = {XMVectorGetX(vec->Get_Point(CCell::POINT_C)),
                              XMVectorGetY(vec->Get_Point(CCell::POINT_C)),
                              XMVectorGetZ(vec->Get_Point(CCell::POINT_C))};

            if (true == Snap(vec->Get_Point(CCell::POINT_A), vP2, distance))
                pPoints[1] = {XMVectorGetX(vec->Get_Point(CCell::POINT_A)),
                              XMVectorGetY(vec->Get_Point(CCell::POINT_A)),
                              XMVectorGetZ(vec->Get_Point(CCell::POINT_A))};
            if (true == Snap(vec->Get_Point(CCell::POINT_B), vP2, distance))
                pPoints[1] = {XMVectorGetX(vec->Get_Point(CCell::POINT_B)),
                              XMVectorGetY(vec->Get_Point(CCell::POINT_B)),
                              XMVectorGetZ(vec->Get_Point(CCell::POINT_B))};
            if (true == Snap(vec->Get_Point(CCell::POINT_C), vP2, distance))
                pPoints[1] = {XMVectorGetX(vec->Get_Point(CCell::POINT_C)),
                              XMVectorGetY(vec->Get_Point(CCell::POINT_C)),
                              XMVectorGetZ(vec->Get_Point(CCell::POINT_C))};

            if (true == Snap(vec->Get_Point(CCell::POINT_A), vP3, distance))
                pPoints[2] = {XMVectorGetX(vec->Get_Point(CCell::POINT_A)),
                              XMVectorGetY(vec->Get_Point(CCell::POINT_A)),
                              XMVectorGetZ(vec->Get_Point(CCell::POINT_A))};
            if (true == Snap(vec->Get_Point(CCell::POINT_B), vP3, distance))
                pPoints[2] = {XMVectorGetX(vec->Get_Point(CCell::POINT_B)),
                              XMVectorGetY(vec->Get_Point(CCell::POINT_B)),
                              XMVectorGetZ(vec->Get_Point(CCell::POINT_B))};
            if (true == Snap(vec->Get_Point(CCell::POINT_C), vP3, distance))
                pPoints[2] = {XMVectorGetX(vec->Get_Point(CCell::POINT_C)),
                              XMVectorGetY(vec->Get_Point(CCell::POINT_C)),
                              XMVectorGetZ(vec->Get_Point(CCell::POINT_C))};
        }
    }

    _vector vNomal = m_pGameInstance->PointNomal(pPoints[0], pPoints[1], pPoints[2]);

    _float vNomalY = XMVectorGetY(vNomal); 

    if (vNomalY < 0.f)
    {
        swap(pPoints[1], pPoints[2]);
    }

    CCell* pCell = CCell::Create(m_pDevice, m_pContext, pPoints, static_cast<_uint>(m_Cells.size()), Type);
    if (nullptr == pCell)
    {
        MSG_BOX("Cell == NULL");
        return;
    }
    m_Cells.push_back(pCell);

    SetUp_Neighbor();
}

_float CNavigation::Compute_HeightOnCell(_float3* fPos)
{
    if (m_iCurrentCellIndex == -1)
        return 0.f;

    _vector v1 = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_A);
    _vector v2 = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_B);
    _vector v3 = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_C);

    v1 = XMVector3TransformCoord(v1, XMLoadFloat4x4(m_WorldMatrix));
    v2 = XMVector3TransformCoord(v2, XMLoadFloat4x4(m_WorldMatrix));
    v3 = XMVector3TransformCoord(v3, XMLoadFloat4x4(m_WorldMatrix));

    _vector Plane = XMPlaneFromPoints(v1, v2, v3);

    _float4 fPlane{};
    XMStoreFloat4(&fPlane, Plane);

    return (-fPlane.x * fPos->x - fPlane.z * fPos->z - fPlane.w) / fPlane.y;
}

HRESULT CNavigation::Save(const _tchar* tFPath)
{
    _ulong dwByte = {0};
    HANDLE hFile = CreateFile(tFPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    _float3 vPoints[3];
    _uint Type;
    for (auto& Cell : m_Cells)
    {
        XMStoreFloat3(&vPoints[0], Cell->Get_Point(CCell::POINT_A));
        XMStoreFloat3(&vPoints[1], Cell->Get_Point(CCell::POINT_B));
        XMStoreFloat3(&vPoints[2], Cell->Get_Point(CCell::POINT_C));
        Type = Cell->Get_Type();
        WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
        WriteFile(hFile, &Type, sizeof(_uint), &dwByte, nullptr);
    }

    CloseHandle(hFile);
    return S_OK;
}

HRESULT CNavigation::Load(const _tchar* tFPath)
{
    _ulong dwByte = {};
    HANDLE hFile = CreateFile(tFPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    while (true)
    {
        _bool Read = false;
        _float3 vPoints[3];
        _uint Type;
        Read = ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
        Read = ReadFile(hFile, &Type, sizeof(_uint), &dwByte, nullptr);
        if (0 == dwByte)
            break;
        if (0 > Type || CCell::TYPE::DEMAGE < Type || Type == NULL)
        {
            Type = CCell::TYPE::NOMAL;
        }

        if (Type == CCell::TYPE::NOMOVE)
            m_vecNomoveType.push_back(static_cast<_uint>(m_Cells.size()));

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, static_cast<_uint>( m_Cells.size()), Type);
        if (nullptr == pCell)
            return E_FAIL;

        m_Cells.push_back(pCell);
    }

    CloseHandle(hFile);

    SetUp_Neighbor();
    
 

    return S_OK;
}

void CNavigation::Set_Type(_uint Type)
{
  for (auto& index : m_vecNomoveType)
  {
    m_Cells[index]->Set_Type(Type);
  }
}

HRESULT CNavigation::Delete_ALLCell()
{
    for (auto& pCell : m_Cells) Safe_Release(pCell);
    m_Cells.clear();
    m_Cells.shrink_to_fit();
    m_iCurrentCellIndex = -1;
  
    m_vecNomoveType.clear();
    m_vecNomoveType.shrink_to_fit();

    return S_OK;
}

void CNavigation::Delete_Cell(_vector LocalRayPos, _vector LocalRayDir)
{
    _float CellDist = {FLT_MAX};
    size_t Index = {};
    LocalRayDir = XMVector3Normalize(LocalRayDir);
    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        _float fDist{};
        if (DirectX::TriangleTests::Intersects(LocalRayPos, LocalRayDir, m_Cells[i]->Get_Point(CCell::POINT_A),
                                               m_Cells[i]->Get_Point(CCell::POINT_B),
                                               m_Cells[i]->Get_Point(CCell::POINT_C), fDist))
        {
            if (fDist < CellDist)
            {
                CellDist = fDist;
                Index = i;
            }
        }
    }
    Safe_Release(*(m_Cells.begin() + Index));
    m_Cells.erase(m_Cells.begin() + Index);
}

_bool CNavigation::Get_bFalling()
{
    if (0 > m_iCurrentCellIndex || m_Cells.size() < m_iCurrentCellIndex)
        return false;

    if (m_Cells[m_iCurrentCellIndex]->Get_Type() == CCell::TYPE::FALL)
    {
        return true;       
    }
    else
        return false;
}

_bool CNavigation::Get_bDemage(_int& HP)
{
    if (0 > m_iCurrentCellIndex || m_Cells.size() < m_iCurrentCellIndex)
        return false;

    if (m_Cells[m_iCurrentCellIndex]->Get_Type() == CCell::TYPE::DEMAGE)
    {
        HP -= 1;
        return true;
    }
    else
        return false;
}

void CNavigation::Find_CurrentCell(_vector vWorldPos)
{
    _vector vAfterLocalPos  = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_WorldMatrix)));
    _vector vBeforeLocalPos = XMVectorZero();
    _int iNeighborIndex     = {-1};
    _vector slide{};

    for (size_t i = 0; i < m_Cells.size(); i++)
    {
        if (true == m_Cells[i]->isIn(vAfterLocalPos, vBeforeLocalPos, &iNeighborIndex, &slide))
        {
            m_iCurrentCellIndex = static_cast<_int>(i);
            return;
        }
    }
}

_vector CNavigation::Get_SafePos()
{
    return m_vSafePos;
}

_vector CNavigation::Get_TagetPos(_int index)
{
    if (index >= m_Cells.size())
        return XMVectorZero();

   return  XMVector3TransformCoord(m_Cells[index]->GetCenter(), XMLoadFloat4x4(m_WorldMatrix));
}

_int CNavigation::Find_Cell_ByPosition(_vector vTargetPos)
{
    _vector vAfterLocalPos  = XMVector3TransformCoord(vTargetPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_WorldMatrix)));
    _vector vBeforeLocalPos = XMVectorZero();
    _int iNeighborIndex     = {-1};
    _vector slide{};

    for (size_t i = 0; i < m_Cells.size(); i++)
    {
        if (true == m_Cells[i]->isIn(vAfterLocalPos, vBeforeLocalPos, &iNeighborIndex, &slide))
        {
            return static_cast<_int>(i);
        }
    }
    return -1;
}

void CNavigation::Set_Taget(_vector Taget)
{
    WRITE_LOCK;

     _int Goal = Find_Cell_ByPosition(Taget);
    if (Goal < 0 || m_iCurrentCellIndex < 0)
        return;

    m_PathPoints = FindPath(m_iCurrentCellIndex, Goal);
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CNavigation");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
    CNavigation* pInstance = new CNavigation(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CTimer");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CNavigation::Free()
{
    __super::Free();

    m_vecNomoveType.clear();
    m_vecNomoveType.shrink_to_fit();

    for (auto& pCell : m_Cells) Safe_Release(pCell);
    m_Cells.clear();
    m_Cells.shrink_to_fit();

#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif
}
