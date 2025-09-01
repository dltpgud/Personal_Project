#include "..\Public\QuadTree.h"

#include "GameInstance.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
    m_iCorners[CORNER_LT] = iLT; // 왼쪽 위
    m_iCorners[CORNER_RT] = iRT; // 오른쪽 위
    m_iCorners[CORNER_RB] = iRB; // 오른쪽 아래
    m_iCorners[CORNER_LB] = iLB; // 왼쪽 아래

    if (1 == m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT]) // 칸의 개수가 1이면 더 이상 분할 안함
        return S_OK;

    m_iCenter = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RB]) >> 1; // 중앙 인덱스

    _uint iLC, iTC, iRC, iBC;
    // 사각형의 중점   
    iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
    iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
    iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
    iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

    // 자식 사각형들을 생성
    m_Children[CORNER_LT] = CQuadTree::Create(m_iCorners[CORNER_LT], iTC, m_iCenter, iLC);
    m_Children[CORNER_RT] = CQuadTree::Create(iTC, m_iCorners[CORNER_RT], iRC, m_iCenter);
    m_Children[CORNER_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[CORNER_RB], iBC);
    m_Children[CORNER_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[CORNER_LB]);

	return S_OK;
}

void CQuadTree::Culling(CGameInstance* pGameInstance, const _float3* pVerticesPos, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrixInv)
{
    //  isDraw() 함수로 카메라와의 거리를 계산해서 LOD를 결정
    if (nullptr == m_Children[CORNER_LT] || true == isDraw(pGameInstance, pVerticesPos, WorldMatrixInv))
    {
        _uint iIndices[4] = {
            m_iCorners[CORNER_LT],
            m_iCorners[CORNER_RT],
            m_iCorners[CORNER_RB],
            m_iCorners[CORNER_LB],
        };

        // 안전 마진을 위한 타일 크기 계산
        _float fTileSize = XMVector3Length(XMLoadFloat3(&pVerticesPos[iIndices[1]]) - XMLoadFloat3(&pVerticesPos[iIndices[0]])).m128_f32[0];
        _float fSafetyMargin = fTileSize * 0.2f; // 20% 안전 마진

        _bool isIn[4] = {
            // 각 코너들이 카메라 프러스텀 안에 있는지 확인 (안전 마진 적용)
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[0]]), 1.f), fSafetyMargin),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[1]]), 1.f), fSafetyMargin),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[2]]), 1.f), fSafetyMargin),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[3]]), 1.f), fSafetyMargin),
        };

        // 프러스텀 컬링 조건을 완화하여 구멍 방지
        // 4개 코너 중 하나라도 프러스텀 안에 있으면 두 삼각형 모두 렌더링
        if (true == isIn[0] || true == isIn[1] || true == isIn[2] || true == isIn[3])
        {
            pIndices[(*pNumIndices)++] = iIndices[0];
            pIndices[(*pNumIndices)++] = iIndices[1];
            pIndices[(*pNumIndices)++] = iIndices[2];
        }

        if (true == isIn[0] || true == isIn[1] || true == isIn[2] || true == isIn[3])
        {
            pIndices[(*pNumIndices)++] = iIndices[0];
            pIndices[(*pNumIndices)++] = iIndices[2];
            pIndices[(*pNumIndices)++] = iIndices[3];
        }

        return;  // 여기서 끝
    }

    // 중앙 점에서 사각형의 대각선 거리, 다시 자식 사각형들을 재귀 호출
    _float fRadius = 
        XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenter]))
            .m128_f32[0];

    // fRadius 반지름 안에 있는 Frustum 안에 있는 terrain 사각형이 있다면 자식들을 호출한다.
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

	_float fWidth = static_cast<_float>(m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT]);

	if (fCamDistance * 0.5f > fWidth) 
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
