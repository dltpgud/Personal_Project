#include "Navigation.h"

#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"


const _float4x4* CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
    : CComponent{Prototype}, m_Cells{Prototype.m_Cells}

#ifdef _DEBUG
    , m_pShader{Prototype.m_pShader}
#endif
{
    for (auto& pCell : m_Cells) Safe_AddRef(pCell);

#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationFilePath)
{
 
    if(nullptr != pNavigationFilePath)
    {
         Load(pNavigationFilePath);
    }

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

   m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

    return S_OK;
}
void CNavigation::SetUp_Neighbor()
{

    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDestCell : m_Cells)
        {
            if (pSourCell == pDestCell)  // ���� ��(�ﰢ��)�̸� �Ʒ� ���ǹ� �ѱ��..
                continue;

            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
                pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);

            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
                pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);

            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
                pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
        }
    }
}
_bool CNavigation::isMove(_fvector vAfterWorldPos , _fvector vBeforeMoveWorldPos, _vector* Slide)
{
    _vector vAfterLocalPos = XMVector3TransformCoord(vAfterWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_WorldMatrix)));
    _vector vBeforeLocalPos = XMVector3TransformCoord(vBeforeMoveWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_WorldMatrix)));

    _int iNeighborIndex = {-1};

    /* ���� �̵��ϰ� �� �����ġ�� ���� �����ϰ� �ִ� �� �ٱ����� ������. */
    if (false == m_Cells[m_iCurrentCellIndex]->isIn(vAfterLocalPos, vBeforeLocalPos, &iNeighborIndex , Slide))
    {	/* ���� ���⿡ �̿��� �־��ٸ�. */
        if (-1 != iNeighborIndex)
        {
            while (true)  // �ݺ����� ���
            {
                if (true == m_Cells[iNeighborIndex]->isIn(vAfterLocalPos, vBeforeLocalPos, &iNeighborIndex, Slide))  // �̿��� ������ ����
                    break;
                
                if (-1 == iNeighborIndex)  // ������� ���� ���°Ű�
                { 
                   return false;
                }
            }
     
            m_iCurrentCellIndex = iNeighborIndex;  //�ݺ��� Ż�⿡ ���������� �� ��ǥ�� ���� �� �ε����� ����      
            return true;
        }
        else /* ���� ���⿡ �̿��� �����ٸ�. */
        {
            return false;
        }
    }

    return true;
}   

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
    
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
        _float4  TerrainCol = { 0.f, 1.f, 0.f, 1.f };
        m_pShader->Bind_RawValue("g_vColor", &TerrainCol, sizeof(_float4));
    
        m_pShader->Begin(0);
    
        for (auto& pCell : m_Cells)
            pCell->Render();
    }
    
    else
    {
        _float4x4		WorldMatrix = *m_WorldMatrix;
        WorldMatrix.m[3][1] += 0.001f;
        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
            return E_FAIL;
        _float4   Obj_on_Terrain = { 1.f, 0.f, 0.f, 1.f };
        m_pShader->Bind_RawValue("g_vColor", &Obj_on_Terrain, sizeof(_float4));
    
        m_pShader->Begin(0);
    
        m_Cells[m_iCurrentCellIndex]->Render();
    }
    

    return S_OK;
}

#endif
_uint CNavigation::Get_CurrentCell_Type()
{
    return  m_Cells[m_iCurrentCellIndex]->Get_Type();
}
_bool CNavigation::Snap(_fvector vP1, _fvector vP2, _vector distance)
{
    // �� ��Һ� ���̸� ���
    XMVECTOR difference = XMVectorAbs(XMVectorSubtract(vP1, vP2));

    // �� ��Ұ� distance ���� ���� �ִ��� Ȯ��
      return XMVector3LessOrEqual(difference, distance);
                                        
}
_vector CNavigation::PointNomal(_float3 fP1, _float3 fP2, _float3 fP3)
{
    _vector v1 = XMLoadFloat3(&fP1);
    _vector v2 = XMLoadFloat3(&fP2);
    _vector v3 = XMLoadFloat3(&fP3);

    // ���� v1 -> v2�� v1 -> v3 ���� ���
    _vector edge1 = v2 - v1;
    _vector edge2 = v3 - v1;

    // �� ������ ������ ����� ���� ����
    return XMVector3Cross(edge1, edge2);

}

void CNavigation::Create_Poly(_float3 p1, _float3 p2, _float3 p3, _uint Type)
{
    _float3 pPoints[3];
      
    pPoints[0] = p1;
    pPoints[1] = p2;
    pPoints[2] = p3;
 
    _vector vP1 =XMLoadFloat3(&pPoints[0]);
    _vector vP2 = XMLoadFloat3(&pPoints[1]);
    _vector vP3 = XMLoadFloat3(&pPoints[2]);


    // ��ó�� ���� ������  �� ���� ��ǥ�� ����Ѵ�.
    _vector distance = { 0.5f,0.5f,0.5f,1.f }; // �������� ���ϰ�
    if (m_Cells.size() != 0)
    {
        for (auto vec : m_Cells) // �ݺ��� ���鼭 �˻�.
        {
            if (true == Snap(vec->Get_Point(CCell::POINT_A), vP1, distance))
                pPoints[0] = { XMVectorGetX(vec->Get_Point(CCell::POINT_A)),XMVectorGetY(vec->Get_Point(CCell::POINT_A)),XMVectorGetZ(vec->Get_Point(CCell::POINT_A)) };
            if (true == Snap(vec->Get_Point(CCell::POINT_B), vP1, distance))
                pPoints[0] = { XMVectorGetX(vec->Get_Point(CCell::POINT_B)),XMVectorGetY(vec->Get_Point(CCell::POINT_B)),XMVectorGetZ(vec->Get_Point(CCell::POINT_B)) };
            if (true == Snap(vec->Get_Point(CCell::POINT_C), vP1, distance))
                pPoints[0] = { XMVectorGetX(vec->Get_Point(CCell::POINT_C)),XMVectorGetY(vec->Get_Point(CCell::POINT_C)),XMVectorGetZ(vec->Get_Point(CCell::POINT_C)) };

            if (true == Snap(vec->Get_Point(CCell::POINT_A), vP2, distance))
                pPoints[1] = { XMVectorGetX(vec->Get_Point(CCell::POINT_A)),XMVectorGetY(vec->Get_Point(CCell::POINT_A)),XMVectorGetZ(vec->Get_Point(CCell::POINT_A)) };
            if (true == Snap(vec->Get_Point(CCell::POINT_B), vP2, distance))
                pPoints[1] = { XMVectorGetX(vec->Get_Point(CCell::POINT_B)),XMVectorGetY(vec->Get_Point(CCell::POINT_B)),XMVectorGetZ(vec->Get_Point(CCell::POINT_B)) };
            if (true == Snap(vec->Get_Point(CCell::POINT_C), vP2, distance))
                pPoints[1] = { XMVectorGetX(vec->Get_Point(CCell::POINT_C)),XMVectorGetY(vec->Get_Point(CCell::POINT_C)),XMVectorGetZ(vec->Get_Point(CCell::POINT_C)) };

            if (true == Snap(vec->Get_Point(CCell::POINT_A), vP3, distance))
                pPoints[2] = { XMVectorGetX(vec->Get_Point(CCell::POINT_A)),XMVectorGetY(vec->Get_Point(CCell::POINT_A)),XMVectorGetZ(vec->Get_Point(CCell::POINT_A)) };
            if (true == Snap(vec->Get_Point(CCell::POINT_B), vP3, distance))
                pPoints[2] = { XMVectorGetX(vec->Get_Point(CCell::POINT_B)),XMVectorGetY(vec->Get_Point(CCell::POINT_B)),XMVectorGetZ(vec->Get_Point(CCell::POINT_B)) };
            if (true == Snap(vec->Get_Point(CCell::POINT_C), vP3, distance))
                pPoints[2] = { XMVectorGetX(vec->Get_Point(CCell::POINT_C)),XMVectorGetY(vec->Get_Point(CCell::POINT_C)),XMVectorGetZ(vec->Get_Point(CCell::POINT_C)) };

        }
    }


    // ������ ���� ������ ���ϰ� 
    _vector vNomal = PointNomal(pPoints[0], pPoints[1], pPoints[2]);

    _float vNomalY = XMVectorGetY(vNomal); // ������ Y���� ����� �������� �ð� �������� �� ���ĵǾ� ����.
  
    if (vNomalY < 0.f) {
        // �ݽð� ���� -> �ð� �������� �ٲٱ� ���� �� ��°�� �� ��° ������ ��ȯ
         swap(pPoints[1], pPoints[2]);

    }
#ifdef _DEBUG
    cout << pPoints[0].x << " " <<  pPoints[0].y << " " << pPoints[0].z << endl;
    cout << pPoints[1].x << " " << pPoints[1].y << " " << pPoints[1].z << endl;
    cout << pPoints[2].x << " " << pPoints[2].y << " " << pPoints[2].z << endl;
#endif

    CCell* pCell = CCell::Create(m_pDevice, m_pContext, pPoints, m_Cells.size(), Type);
    if (nullptr == pCell) {

        MSG_BOX("Cell == NULL");
        return;
    }
    m_Cells.push_back(pCell);



    SetUp_Neighbor();
}


_float CNavigation::Compute_HeightOnCell(_float3* fPos)
{
    _vector v1 = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_A);
    _vector v2 = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_B);
    _vector v3 = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_C);

    v1 = XMVector3TransformCoord(v1, XMLoadFloat4x4(m_WorldMatrix));
    v2 = XMVector3TransformCoord(v2, XMLoadFloat4x4(m_WorldMatrix));
    v3 = XMVector3TransformCoord(v3, XMLoadFloat4x4(m_WorldMatrix));

    _vector Plane =  XMPlaneFromPoints(v1,v2,v3);

    _float4 fPlane{};
    XMStoreFloat4(&fPlane, Plane);


    return  (-fPlane.x * fPos->x - fPlane.z * fPos->z - fPlane.w) / fPlane.y;
}

HRESULT CNavigation::Save(const _tchar* tFPath)
{
    _ulong		dwByte = { 0 };
    HANDLE		hFile = CreateFile(tFPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    _float3		vPoints[3];
    _uint       Type;
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

        if (0 > Type || 2 < Type || Type ==NULL)
        {
            Type = CCell::TYPE::NOMAL;
        }
        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size(),Type);
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
    m_Cells[m_iCurrentCellIndex]->Set_Type(Type);
}

void CNavigation::Set_Type_From_Ray(_vector LocalRayPos, _vector LocalRayDir, _uint Type)
{
    _float CellDist = { 0xffff };
    size_t Index = {};

    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        _float fDist{};
        if (DirectX::TriangleTests::Intersects(LocalRayPos, LocalRayDir, m_Cells[i]->Get_Point(CCell::POINT_A), m_Cells[i]->Get_Point(CCell::POINT_B), m_Cells[i]->Get_Point(CCell::POINT_C),
            fDist))
        {
            if (fDist < CellDist)
            {
                CellDist = fDist;
                Index = i;
            }

        }
    }
    m_Cells[Index]->Set_Type(Type);
}

HRESULT CNavigation::Delete_ALLCell()
{

    for (auto& pCell : m_Cells) Safe_Release(pCell);

    m_Cells.clear();
    return S_OK;
}

void CNavigation::Delete_Cell(_vector LocalRayPos, _vector LocalRayDir)
{
 // �� ..�̰� ���� ���ؼ� �Ϸ��ϱ� �ݺ��� erase �ϰ� ���� �ݺ��� ����ֱ� �� �����;;;
// ������� �ݺ��� ��ſ� �ε����� ã�Ƽ� ��������..
    _float CellDist = { 0xffff };
    size_t Index = {};

    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        _float fDist{};
        if (DirectX::TriangleTests::Intersects(LocalRayPos, LocalRayDir, m_Cells[i]->Get_Point(CCell::POINT_A), m_Cells[i]->Get_Point(CCell::POINT_B), m_Cells[i]->Get_Point(CCell::POINT_C),
            fDist))
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





CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath )
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pNavigationFilePath)))
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

    for (auto& pCell : m_Cells) Safe_Release(pCell);

#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif
}
