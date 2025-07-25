#include "..\Public\QuadTree.h"

#include "GameInstance.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
    m_iCorners[CORNER_LT] = iLT; // 좌상단
    m_iCorners[CORNER_RT] = iRT; // 우상단
    m_iCorners[CORNER_RB] = iRB; // 우하단
    m_iCorners[CORNER_LB] = iLB; // 좌하단

    if (1 == m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT]) // 칸의 간격이 1이면 함수 종료
        return S_OK;

    m_iCenter = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RB]) >> 1; // 중앙 정점

    _uint iLC, iTC, iRC, iBC;
    // 사각형을 분할   
    iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
    iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
    iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
    iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

    // 분할한 사각형을 자식으로 생성
    m_Children[CORNER_LT] = CQuadTree::Create(m_iCorners[CORNER_LT], iTC, m_iCenter, iLC);
    m_Children[CORNER_RT] = CQuadTree::Create(iTC, m_iCorners[CORNER_RT], iRC, m_iCenter);
    m_Children[CORNER_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[CORNER_RB], iBC);
    m_Children[CORNER_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[CORNER_LB]);

	return S_OK;
}

void CQuadTree::Culling(CGameInstance* pGameInstance, const _float3* pVerticesPos, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrixInv)
{

    //  isDraw() 함수를 통해 카메라와의 거리를 비교해 분할 여부를 결정
    if (nullptr == m_Children[CORNER_LT] || true == isDraw(pGameInstance, pVerticesPos, WorldMatrixInv))
    {
        _uint iIndices[4] = {
            m_iCorners[CORNER_LT],
            m_iCorners[CORNER_RT],
            m_iCorners[CORNER_RB],
            m_iCorners[CORNER_LB],
        };

        _bool isIn[4] = {
            // 각 꼭지점이 로컬 상의 절두체 안에 있는지 확인
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[0]]), 1.f)),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[1]]), 1.f)),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[2]]), 1.f)),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[3]]), 1.f)),
        };

        // 각 삼각형 안에 있으면 인덱스를 올려준다.
        if (true == isIn[0] || true == isIn[1] || true == isIn[2])
        {
            pIndices[(*pNumIndices)++] = iIndices[0];
            pIndices[(*pNumIndices)++] = iIndices[1];
            pIndices[(*pNumIndices)++] = iIndices[2];
        }

        if (true == isIn[0] || true == isIn[2] || true == isIn[3])
        {
            pIndices[(*pNumIndices)++] = iIndices[0];
            pIndices[(*pNumIndices)++] = iIndices[2];
            pIndices[(*pNumIndices)++] = iIndices[3];
        }

        return;  // 재귀 종료
    }

    // 중앙 정점과 사각형의 좌상단 정점과의 거리, 다시 말해 사각형의 반지름을 구한다.
    _float fRadius = 
        XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenter]))
            .m128_f32[0];

    // fRadius 보다 로컬 상의 Frustum 안에 있는 terrain 사각형이 더 작다면 재귀로 들어간다.
    if (true == pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
    {
        for (auto& pChild : m_Children)
        {
            if (nullptr != pChild)
                pChild->Culling(pGameInstance, pVerticesPos, pIndices, pNumIndices, WorldMatrixInv);
        }
    }
}

_bool CQuadTree::isDraw(CGameInstance * pGameInstance, const _float3 * pVerticesPos, _fmatrix WorldMatrixInv)
{
	_vector		vCamPosition = XMLoadFloat4(pGameInstance->Get_CamPosition());

	vCamPosition = XMVector3TransformCoord(vCamPosition, WorldMatrixInv);

	_float		fCamDistance = XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCenter]) - vCamPosition).m128_f32[0];

	_float		fWidth = static_cast <_uint>(m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT]);

	if (fCamDistance * 0.1f > fWidth)
		return true;

	return false;
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Created : CQuadTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	__super::Free();

	for (auto& pQuadTree : m_Children)
		Safe_Release(pQuadTree);
}
