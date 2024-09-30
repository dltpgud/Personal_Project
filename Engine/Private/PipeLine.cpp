#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Update()
{

    for (size_t i = 0; i < D3DTS_END; i++)
        XMStoreFloat4x4(&m_TransMatrixInverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransMatrix[i])));

    XMStoreFloat4(&m_vCamPosition, XMLoadFloat4x4(&m_TransMatrixInverse[D3DTS_VIEW]).r[3]);
    return S_OK;
}

CPipeLine* CPipeLine::Create()
{
    return new CPipeLine();
}

void CPipeLine::Free()
{
    __super::Free();
}
