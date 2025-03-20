#include "..\Public\QuadTree.h"

#include "GameInstance.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
    m_iCorners[CORNER_LT] = iLT;
    m_iCorners[CORNER_RT] = iRT;
    m_iCorners[CORNER_RB] = iRB;
    m_iCorners[CORNER_LB] = iLB;

    if (1 == m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT])
        return S_OK;

    m_iCenter = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RB]) >> 1;

    _uint iLC, iTC, iRC, iBC;

    iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
    iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
    iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
    iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

    m_Children[CORNER_LT] = CQuadTree::Create(m_iCorners[CORNER_LT], iTC, m_iCenter, iLC);
    m_Children[CORNER_RT] = CQuadTree::Create(iTC, m_iCorners[CORNER_RT], iRC, m_iCenter);
    m_Children[CORNER_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[CORNER_RB], iBC);
    m_Children[CORNER_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[CORNER_LB]);

	return S_OK;
}

void CQuadTree::Culling(CGameInstance* pGameInstance, const _float3* pVerticesPos, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrixInv)
{
    if (nullptr == m_Children[CORNER_LT] || true == isDraw(pGameInstance, pVerticesPos, WorldMatrixInv))
    {
        _uint iIndices[4] = {
            m_iCorners[CORNER_LT],
            m_iCorners[CORNER_RT],
            m_iCorners[CORNER_RB],
            m_iCorners[CORNER_LB],
        };

        _bool isIn[4] = {
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[0]]), 1.f)),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[1]]), 1.f)),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[2]]), 1.f)),
            pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVerticesPos[iIndices[3]]), 1.f)),
        };

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

        return;
    }

    _float fRadius =
        XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenter]))
            .m128_f32[0];

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

	_float		fWidth = m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT];

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
