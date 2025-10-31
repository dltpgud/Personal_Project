#include "VIBuffer_Terrain.h"
#include "GameInstance.h"
#include "QuadTree.h"
#include "Collider.h"
CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{pDevice, pContext}, m_pPos(nullptr)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
    : CVIBuffer{Prototype}, m_iNumVerticesX{Prototype.m_iNumVerticesX}, m_iNumVerticesZ{Prototype.m_iNumVerticesZ},
      m_pPos{Prototype.m_pPos}, m_pIndices{Prototype.m_pIndices} 
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
    m_ePrimitiveTopology =  D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma endregion

#pragma region VERTEX_BUFFER

    VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
    m_pVertexPositions = new _float3[m_iNumVertices];
    for (_uint i = 0; i < m_iNumVerticesZ; i++)
    {
        for (_uint j = 0; j < m_iNumVerticesX; j++)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            pVertices[iIndex].vPosition = 
                m_pVertexPositions[iIndex] =
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
     m_pVertexPositions = new _float3[m_iNumVertices];
    for (_uint i = 0; i < m_iNumVerticesZ; i++)
    {
        for (_uint j = 0; j < m_iNumVerticesX; j++)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            pVertices[iIndex].vPosition = m_pVertexPositions[iIndex] =
                _float3(static_cast<_float>(j), 0.f, static_cast<_float>(i)); 

            m_pPos[iIndex] = pVertices[iIndex].vPosition;

            pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
            pVertices[iIndex].vTexcoord = _float2(static_cast<_float>(j) / (m_iNumVerticesX - 1.f), static_cast<_float>(i) / (m_iNumVerticesZ - 1.f));
        }
    }

    m_vMin = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
    m_vMax = _float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (UINT i = 0; i < m_iNumVerticesX * m_iNumVerticesZ; ++i)
    {
        const _float3& v = pVertices[i].vPosition;
        m_vMin.x = min(m_vMin.x, v.x);
        m_vMin.y = min(m_vMin.y, v.y);
        m_vMin.z = min(m_vMin.z, v.z);

        m_vMax.x = max(m_vMax.x, v.x);
        m_vMax.y = max(m_vMax.y, v.y);
        m_vMax.z = max(m_vMax.z, v.z);
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

void CVIBuffer_Terrain::DYNAMIC_Set_Buffer(_int x, _int z)
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
    m_pVertexPositions = new _float3[m_iNumVertices];
    for (_uint i = 0; i < m_iNumVerticesZ; i++)
    {
        for (_uint j = 0; j < m_iNumVerticesX; j++)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            pVertices[iIndex].vPosition = m_pVertexPositions[iIndex] =
                _float3(static_cast<_float>(j), 0.f, static_cast<_float>(i)); 

            m_pPos[iIndex] = pVertices[iIndex].vPosition;

            pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
            pVertices[iIndex].vTexcoord = _float2(static_cast<_float>(j) / (m_iNumVerticesX - 1.f),
                                                  static_cast<_float>(i) / (m_iNumVerticesZ - 1.f));
        }
    }

    
    m_vMin = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
    m_vMax = _float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (UINT i = 0; i < m_iNumVerticesX * m_iNumVerticesZ; ++i)
    {
        const _float3& v = pVertices[i].vPosition;
        m_vMin.x = min(m_vMin.x, v.x);
        m_vMin.y = min(m_vMin.y, v.y);
        m_vMin.z = min(m_vMin.z, v.z);

        m_vMax.x = max(m_vMax.x, v.x);
        m_vMax.y = max(m_vMax.y, v.y);
        m_vMax.z = max(m_vMax.z, v.z);
    }


#pragma endregion

#pragma region INDEX_BUFFER
    m_pIndices = new _uint[m_iNumIndexices];
    _uint iNumIndices = {0};

    for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
    {
        for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            _uint iIndices[4] = {iIndex + m_iNumVerticesX, iIndex + m_iNumVerticesX + 1, iIndex + 1, iIndex};

            m_pIndices[iNumIndices++] = iIndices[0];
            m_pIndices[iNumIndices++] = iIndices[1];
            m_pIndices[iNumIndices++] = iIndices[2];

            /*법선 벡터 구하기*/
            _vector vSour, vDest, vNormal;

            vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

            m_pIndices[iNumIndices++] = iIndices[0];
            m_pIndices[iNumIndices++] = iIndices[2];
            m_pIndices[iNumIndices++] = iIndices[3];

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
    m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    m_InitialDesc.pSysMem = m_pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return;

    Safe_Delete_Array(pVertices);
}

void CVIBuffer_Terrain::Set_QuadTree()
{
    m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
                                    m_iNumVerticesX * m_iNumVerticesZ -1, m_iNumVerticesX - 1, 0);
}

_bool CVIBuffer_Terrain::Picking_OnTerrain_QuadTree(_vector RayPos, _vector RayDir, CTransform* pTransform,
                                                      OUT _float* fDist, OUT _float3* vNormal, OUT _float3* vWorldPos)
{
    if (!m_pQuadTree)
        return false;

    _matrix invWorld = pTransform->Get_WorldMatrix_Inverse();
    _vector localRayPos = XMVector3TransformCoord(RayPos, invWorld);
    _vector localRayDir = XMVector3Normalize(XMVector3TransformNormal(RayDir, invWorld));

    _float fBestDist = FLT_MAX;
    _float3 vLocalHit{}, vLocalNormal{};
    
    m_pQuadTree->Picking_Ray(m_pVertexPositions, localRayPos, localRayDir, fBestDist, vLocalHit, vLocalNormal);

    if (fBestDist == FLT_MAX)
        return false;
    
    _vector worldHit = XMVector3TransformCoord(XMLoadFloat3(&vLocalHit), pTransform->Get_WorldMatrix());
    _vector worldNormal = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&vLocalNormal), pTransform->Get_WorldMatrix()));

    if (fDist)
        *fDist = fBestDist;
    if (vNormal)
        XMStoreFloat3(vNormal, worldNormal);

    _float3 vResult;
    XMStoreFloat3(&vResult, worldHit);

    *vWorldPos = vResult;

    return true;
}

_bool CVIBuffer_Terrain::Intersect_OnTerrain_QuadTree(CCollider* pColliderWorldSpace, CTransform* pTerrainTransform, OUT _vector* pWorldNormal, OUT _vector* pWorldHitPos)
{
    if (!m_pQuadTree || !pColliderWorldSpace)
        return false;

    // 월드->로컬
    _matrix invWorld = pTerrainTransform->Get_WorldMatrix_Inverse();

    CCollider::TYPE eType = pColliderWorldSpace->Get_Type();

    _float3 bestLocalHit{};
    _float3 bestLocalN{};
    _float bestPen = -FLT_MAX;
    _bool touched = false;

    switch (eType)
    {
    case CCollider::TYPE_SPHERE:
    {
        // 월드 스피어 가져옴
        const CBounding_Sphere* pSphBound = static_cast<const CBounding_Sphere*>(pColliderWorldSpace->Get_Bounding());
        BoundingSphere worldSphere = *pSphBound->Get_Desc();

        // 스피어를 로컬로 변환
        BoundingSphere localSphere;
        {
            _vector c = XMLoadFloat3(&worldSphere.Center);
            _vector lc = XMVector3TransformCoord(c, invWorld);
            XMStoreFloat3(&localSphere.Center, lc);

            // 스케일 보정 (반지름은 최대 scale 성분 사용)
            _vector S, R, T;
            XMMatrixDecompose(&S, &R, &T, invWorld);
            _float sx = XMVectorGetX(S);
            _float sy = XMVectorGetY(S);
            _float sz = XMVectorGetZ(S);
            _float maxScale = max(sx, max(sy, sz));
            localSphere.Radius = worldSphere.Radius * maxScale;
        }
       
         touched = m_pQuadTree->Intersect_Node(localSphere, m_pVertexPositions, &bestLocalHit, &bestLocalN, &bestPen);
    }
    break;

    case CCollider::TYPE_AABB:
    {
        const CBounding_AABB* pBoxBound = static_cast<const CBounding_AABB*>(pColliderWorldSpace->Get_Bounding());
        BoundingBox worldBox = *pBoxBound->Get_Desc();

        // AABB를 로컬로 변환
        BoundingBox localBox;
        worldBox.Transform(localBox, invWorld);

        touched = m_pQuadTree->Intersect_Node(localBox, m_pVertexPositions, &bestLocalHit, &bestLocalN, &bestPen);
    }
    break;

    case CCollider::TYPE_OBB:
    {
        const CBounding_OBB* pOBBBound = static_cast<const CBounding_OBB*>(pColliderWorldSpace->Get_Bounding());
        BoundingOrientedBox worldOBB = *pOBBBound->Get_Desc();

        BoundingOrientedBox localOBB;
        worldOBB.Transform(localOBB, invWorld);

        touched = m_pQuadTree->Intersect_Node(localOBB, m_pVertexPositions, &bestLocalHit, &bestLocalN, &bestPen);
    }
    break;
    }

    if (!touched || bestPen == -FLT_MAX)
        return false;

    // 2) 최종 결과를 로컬 -> 월드로 변환해서 리턴
    _matrix worldMat = pTerrainTransform->Get_WorldMatrix();
    XMVECTOR wh = XMVector3TransformCoord(XMLoadFloat3(&bestLocalHit), worldMat);
    XMVECTOR wn = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&bestLocalN), worldMat));

    if (pWorldHitPos)
        *pWorldHitPos = wh;
    if (pWorldNormal)
        *pWorldNormal = wn;

    return true;
}



AABB CVIBuffer_Terrain::Get_WorldAABB(CTransform* pTransform)
{
    AABB box{};
    box.min = m_vMin; // 로컬 AABB 최소값
    box.max = m_vMax; // 로컬 AABB 최대값

    _matrix world = pTransform->Get_WorldMatrix();

    // 8개 꼭짓점 변환 후 다시 AABB 구하기
    _float3 corners[8] = {
        {box.min.x, box.min.y, box.min.z}, {box.max.x, box.min.y, box.min.z}, {box.min.x, box.max.y, box.min.z},
        {box.max.x, box.max.y, box.min.z}, {box.min.x, box.min.y, box.max.z}, {box.max.x, box.min.y, box.max.z},
        {box.min.x, box.max.y, box.max.z}, {box.max.x, box.max.y, box.max.z},
    };

    _float3 worldMin = {FLT_MAX, FLT_MAX, FLT_MAX};
    _float3 worldMax = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

    for (int i = 0; i < 8; ++i)
    {
        _vector v = XMVector3TransformCoord(XMLoadFloat3(&corners[i]), world);
        _float3 w;
        XMStoreFloat3(&w, v);

        worldMin.x = min(worldMin.x, w.x);
        worldMin.y = min(worldMin.y, w.y);
        worldMin.z = min(worldMin.z, w.z);

        worldMax.x = max(worldMax.x, w.x);
        worldMax.y = max(worldMax.y, w.y);
        worldMax.z = max(worldMax.z, w.z);
    }

    box.min = worldMin;
    box.max = worldMax;
    return box;
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrixInverse)
{
    // 투영공간에 정의해둔 절두체를 로컬공간으로 변환
    m_pGameInstance->Frustum_Transform_To_LocalSpace(WorldMatrixInverse);

    _uint iNumIndices = {0};

    D3D11_MAPPED_SUBRESOURCE SubResource{};

    m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

    _uint* pIndices = static_cast<_uint*>(SubResource.pData);

    m_pQuadTree->Culling(m_pGameInstance, m_pVertexPositions, pIndices, &iNumIndices, WorldMatrixInverse);

    m_pContext->Unmap(m_pIB, 0);

    m_iNumIndexices = iNumIndices;
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

	Safe_Release(m_pQuadTree);
    Safe_Delete_Array(m_pPos);
    Safe_Delete_Array(m_pIndices);
}
