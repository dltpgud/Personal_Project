#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{pDevice, pContext}, m_pPos(nullptr)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
    : CVIBuffer{Prototype}, m_iNumVerticesX{Prototype.m_iNumVerticesX}, m_iNumVerticesZ{Prototype.m_iNumVerticesZ},
      m_pPos{Prototype.m_pPos}
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
    return S_OK;
}

void CVIBuffer_Terrain::Set_HightMap(const _tchar* pHeightMapFilePath)
{
#pragma region Read_File_init

    _ulong dwByte = {0};
    HANDLE hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return;

    BITMAPFILEHEADER fh{};
    BITMAPINFOHEADER ih{};

    ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
    ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);

    _uint* pPixel = new _uint[ih.biWidth * ih.biHeight];

    ReadFile(hFile, pPixel, sizeof(_uint) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

    CloseHandle(hFile);

    m_iNumVerticesX = ih.biWidth;
    m_iNumVerticesZ = ih.biHeight;
    m_iVertexStride = sizeof(VTXNORTEX);
    m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
    m_iIndexStride = sizeof(_uint); // 대충 65천개 넘어갈꺼 같으니..
    m_iNumIndexices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma endregion

#pragma region VERTEX_BUFFER

    VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];

    for (_uint i = 0; i < m_iNumVerticesZ; i++)
    {
        for (_uint j = 0; j < m_iNumVerticesX; j++)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            pVertices[iIndex].vPosition =
              _float3(static_cast<_float>(j), (pPixel[iIndex] & 0x000000ff) / 15.0f, static_cast<_float>(i)); // y가 파란색인 픽셀로 높이맵
            pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
            pVertices[iIndex].vTexcoord = _float2(static_cast<_float>(j) / (m_iNumVerticesX - 1.f), static_cast<_float>(i) / (m_iNumVerticesZ - 1.f));
        }
    }

#pragma endregion

#pragma region INDEX_BUFFER
    _uint* pIndices = new _uint[m_iNumIndexices];
    _uint iNumIndices = {0};

    for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
    {
        for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            _uint iIndices[4] = {iIndex + m_iNumVerticesX, iIndex + m_iNumVerticesX + 1, iIndex + 1, iIndex};

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[1];
            pIndices[iNumIndices++] = iIndices[2];

            /*법선 벡터 구하기*/
            _vector vSour, vDest, vNormal;

            vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[2];
            pIndices[iNumIndices++] = iIndices[3];

            vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
        }
    }

    for (size_t i = 0; i < m_iNumVertices; i++)
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

#pragma endregion

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    m_InitialDesc.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return;

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndexices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    m_InitialDesc.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return;

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pIndices);
    Safe_Delete_Array(pPixel);
}

void CVIBuffer_Terrain::Set_Buffer(_int x, _int z)
{
    m_iNumVerticesX = x;
    m_iNumVerticesZ = z;
    m_iVertexStride = sizeof(VTXNORTEX);
    m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
    m_iIndexStride = sizeof(_uint); // 대충 65천개 넘어갈꺼 같으니..
    m_iNumIndexices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    m_pPos = new _float3[m_iNumVertices];
#pragma region VERTEX_BUFFER

    VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];

    for (_uint i = 0; i < m_iNumVerticesZ; i++)
    {
        for (_uint j = 0; j < m_iNumVerticesX; j++)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            pVertices[iIndex].vPosition = _float3(static_cast<_float>(j), 0.f, static_cast<_float>(i)); // y가 파란색인 픽셀로 높이맵

            m_pPos[iIndex] = pVertices[iIndex].vPosition;

            pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
            pVertices[iIndex].vTexcoord = _float2(static_cast<_float>(j) / (m_iNumVerticesX - 1.f), static_cast<_float>(i) / (m_iNumVerticesZ - 1.f));
        }
    }

#pragma endregion

#pragma region INDEX_BUFFER
    _uint* pIndices = new _uint[m_iNumIndexices];
    _uint iNumIndices = {0};

    for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
    {
        for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            _uint iIndices[4] = {iIndex + m_iNumVerticesX, iIndex + m_iNumVerticesX + 1, iIndex + 1, iIndex};

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[1];
            pIndices[iNumIndices++] = iIndices[2];

            /*법선 벡터 구하기*/
            _vector vSour, vDest, vNormal;

            vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[2];
            pIndices[iNumIndices++] = iIndices[3];

            vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
        }
    }

    for (size_t i = 0; i < m_iNumVertices; i++)
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

#pragma endregion

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    m_InitialDesc.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return;

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndexices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    m_InitialDesc.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return;

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pIndices);
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
    CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Terrain::Free()
{
    __super::Free();

    if (false == m_bClone)
        Safe_Delete_Array(m_pPos);
}
