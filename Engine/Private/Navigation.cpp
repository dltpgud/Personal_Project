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
 //   XMStoreFloat4x4(m_WorldMatrix, XMMatrixIdentity());

    _ulong dwByte = {};
    HANDLE hFile = CreateFile(pNavigationFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    while (true)
    {
        _float3 vPoints[3];

        ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

        if (0 == dwByte)
            break;

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
        if (nullptr == pCell)
            return E_FAIL;

        m_Cells.push_back(pCell);
    }

    CloseHandle(hFile);

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements,
                                VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

    SetUp_Neighbor();

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
            if (pSourCell == pDestCell)  // 같은 셀(삼각형)이면 아래 조건문 넘기고..
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
_bool CNavigation::isMove(_fvector vWorldPos)
{
    _vector vLocalPos = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_WorldMatrix)));

    _int iNeighborIndex = {-1};

    /* 현재 이동하고 난 결과위치가 원래 존재하고 있던 쎌 바깥으로 나갔다. */
    if (false == m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex))
    {	/* 나간 방향에 이웃이 있었다면. */
        if (-1 != iNeighborIndex)
        {
            while (true)  // 반복하자 계속
            {
                if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex))  // 이웃이 있을때 까지
                    break;

                if (-1 == iNeighborIndex)  // 여기까지 오면 없는거고
                    return false;
            }
                m_iCurrentCellIndex = iNeighborIndex;  //반복문 탈출에 성공했으면 그 좌표를 현재 셀 인덱스로 잡자  
                return true;
        }
        else /* 나간 방향에 이웃이 없었다면. */
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

    m_pShader->Begin(0);

    for (auto& pCell : m_Cells) pCell->Render();

    return S_OK;
}
_bool CNavigation::Snap(_fvector vP1, _fvector vP2, _vector distance)
{
    // 각 요소별 차이를 계산
    XMVECTOR difference = XMVectorAbs(XMVectorSubtract(vP1, vP2));

    // 각 요소가 distance 범위 내에 있는지 확인
      return XMVector3LessOrEqual(difference, distance);
                                        
}
void CNavigation::Create_Poly(_float3 p1, _float3 p2, _float3 p3)
{
    _float3 pPoints[3];
      
    pPoints[0] = p1;
    pPoints[1] = p2;
    pPoints[2] = p3;
 
    _vector vP1 =XMLoadFloat3(&pPoints[0]);
    _vector vP2 = XMLoadFloat3(&pPoints[1]);
    _vector vP3 = XMLoadFloat3(&pPoints[2]);

    _vector distance = { 0.2f,0.2f,0.2f,1.f };
    if (m_Cells.size() != 0)
    {
        for (auto vec : m_Cells)
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

    CCell* pCell = CCell::Create(m_pDevice, m_pContext, pPoints, m_Cells.size());
    if (nullptr == pCell) {

        MSG_BOX("Cell == NULL");
        return;
    }
    m_Cells.push_back(pCell);
}

#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
                                 const _tchar* pNavigationFilePath)
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
