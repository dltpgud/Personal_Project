#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CVIBuffer{pDevice, pContext}
{
}

CMesh::CMesh(const CMesh& Prototype) : CVIBuffer{Prototype}, m_iMaterialIndex{Prototype.m_iMaterialIndex}
{
    strcpy_s(m_szName, Prototype.m_szName);

    m_pPos = new _float3[m_iNumVertices];
    m_pIndices = new _uint[m_iNumIndexices];


    memcpy(m_pPos, Prototype.m_pPos, sizeof(_float3) * m_iNumVertices);
    memcpy(m_pIndices, Prototype.m_pIndices, sizeof(_uint) * m_iNumIndexices);
     

}
HRESULT CMesh::Initialize_Proto(CModel::TYPE eModelType, HANDLE& hFile, _fmatrix PreTransformMatrix)
{
    DWORD dwByte{};
    _bool bReadFile{};

    bReadFile = ReadFile(hFile, m_szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
    bReadFile = ReadFile(hFile, &m_iMaterialIndex, sizeof(m_iMaterialIndex), &dwByte, nullptr);
    bReadFile = ReadFile(hFile, &m_iNumVertices, sizeof(m_iNumVertices), &dwByte, nullptr);

    _uint iNumFaces;
    bReadFile = ReadFile(hFile, &iNumFaces, sizeof(iNumFaces), &dwByte, nullptr);
    m_iNumIndexices = iNumFaces * 3;
    _uint* pIndices = new _uint[m_iNumIndexices];
    bReadFile = ReadFile(hFile, pIndices, sizeof(_uint) * m_iNumIndexices, &dwByte, nullptr);


    m_pIndices = new _uint[m_iNumIndexices];

    for (_uint i = 0; i < m_iNumIndexices; i++)
    {
        m_pIndices[i] = pIndices[i];

    }
    m_iIndexStride = sizeof(_uint);
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    HRESULT hr = eModelType == CModel::TYPE_ANIM ? Load_AnimMesh(hFile) : Load_NonAnimMesh(hFile, PreTransformMatrix);


#pragma region INDEX_BUFFER

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndexices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iIndexStride;

    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    m_InitialDesc.pSysMem = pIndices;

    _uint		iNumIndices = { 0 };

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

#pragma endregion
    Safe_Delete_Array(pIndices);


    return S_OK;
}
/*
HRESULT CMesh::Initialize_Proto(_char* pName, _uint iMaterialIndex, _uint iNumVertex, _uint iNumFaces, VTXMESH* pVertex,
                                _uint* pIndices, _fmatrix PreTransformMatrix)
{
    strcpy_s(m_szName, pName);
    m_iMaterialIndex = iMaterialIndex;
    m_iVertexStride = sizeof(VTXMESH);
    m_iNumVertices = iNumVertex;
    m_iIndexStride = sizeof(_uint);
    m_iNumIndexices = iNumFaces * 3;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    // dx9 : 정점버퍼를 할당하고 -> 락언락해서 정점버퍼에 초기값을 채운다. 
    // dx9 : 정점버퍼에 초기값을 채우면서 정점버퍼를 할당한다
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    // 할당하고자하는 메모리공간의 크기(Byte)
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;

    // 버퍼의 속성 (정적, 동적) 
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    m_InitialDesc.pSysMem = pVertex;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER

    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndexices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
    m_InitialDesc.pSysMem = pIndices;

    _uint iNumIndices = {0};

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

#pragma endregion

    return S_OK;
}
*/
HRESULT CMesh::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const vector<class CBone*>& Bones, const _char* pConstantName)
{
    _float4x4			BoneMatrices[512];

    _uint		iNumBones = { 0 };

    for (auto& iBoneIndex : m_Bones)   /*뼈의 인덱스 만큼 돌면서 행렬을 곱해라*/
    {
        _matrix			CombinedMatrix = Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
        _matrix			OffsetMatrix = XMLoadFloat4x4(&m_OffsetMatrices[iNumBones]);

        XMStoreFloat4x4(&BoneMatrices[iNumBones++], OffsetMatrix * CombinedMatrix);
    }

    return pShader->Bind_Matrices(pConstantName, BoneMatrices, 512);
}

_float3* CMesh::Get_pPos(_int i)
{
    return &m_pPos[i];
}

_uint* CMesh::Get_pIndices(_int i)
{
    return &m_pIndices[i];
}

_uint CMesh::Get_iNumIndexices()
{
    return m_iNumIndexices;
}

_uint CMesh::Get_iNumVertices()
{
    return m_iNumVertices;
}


HRESULT CMesh::Load_AnimMesh(HANDLE hFile)
{
    DWORD dwByte{};
    _bool bReadFile{};

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];

    bReadFile = ReadFile(hFile, pVertices, sizeof(VTXANIMMESH) * m_iNumVertices, &dwByte, nullptr);
    m_pPos = new _float3[m_iNumVertices];
    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        m_pPos[i] = pVertices[i].vPosition;
    }

    m_iVertexStride = sizeof(VTXANIMMESH);



    bReadFile = ReadFile(hFile, &m_iNumBones, sizeof(m_iNumBones), &dwByte, nullptr);

    for (_uint i = 0; i < m_iNumBones; i++)
    {
        _uint iTemp{};
        bReadFile = ReadFile(hFile, &iTemp, sizeof(iTemp), &dwByte, nullptr);
        m_Bones.push_back(iTemp);
    }

    for (_uint i = 0; i < m_iNumBones; i++)
    {
        _float4x4 matTemp{};
        bReadFile = ReadFile(hFile, &matTemp, sizeof(matTemp), &dwByte, nullptr);
        m_OffsetMatrices.push_back(matTemp);
    }

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
        return E_FAIL;

    Safe_Delete_Array(pVertices);
    return S_OK;
}

HRESULT CMesh::Load_NonAnimMesh(HANDLE hFile, _fmatrix PreTransformMatrix)
{
    DWORD dwByte{};
    _bool bReadFile{};

    VTXANIMMESH* pLoadVertices = new VTXANIMMESH[m_iNumVertices];
    bReadFile = ReadFile(hFile, pLoadVertices, sizeof(VTXANIMMESH) * m_iNumVertices, &dwByte, nullptr);

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    m_pPos = new _float3[m_iNumVertices];

    m_iVertexStride = sizeof(VTXMESH);

    for (_uint i = 0; i < m_iNumVertices; i++)
    {
        pVertices[i].vPosition = pLoadVertices[i].vPosition;
        XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
        m_pPos[i] = pVertices[i].vPosition;
        pVertices[i].vNormal = pLoadVertices[i].vNormal;
        XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

        pVertices[i].vTangent = pLoadVertices[i].vTangent;
        pVertices[i].vTexcoord = pLoadVertices[i].vTexcoord;
    }

    bReadFile = ReadFile(hFile, &m_iNumBones, sizeof(m_iNumBones), &dwByte, nullptr);

    for (_uint i = 0; i < m_iNumBones; i++)
    {
        _uint iTemp{};
        bReadFile = ReadFile(hFile, &iTemp, sizeof(iTemp), &dwByte, nullptr);
    }

    for (_uint i = 0; i < m_iNumBones; i++)
    {
        _float4x4 matTemp{};
        bReadFile = ReadFile(hFile, &matTemp, sizeof(matTemp), &dwByte, nullptr);
    }

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
        return E_FAIL;

    Safe_Delete_Array(pLoadVertices);
    Safe_Delete_Array(pVertices);

    return S_OK;
}

/*
CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _char* pName, _uint iMaterialIndex,
                     _uint iNumVertex, _uint iNumFaces, VTXMESH* pVertex, _uint* pIndices, _fmatrix PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Proto(pName, iMaterialIndex, iNumVertex, iNumFaces, pVertex, pIndices,
                                           PreTransformMatrix)))
    {
        MSG_BOX("Failed To Created : CMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}*/

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, HANDLE& hFile, _fmatrix PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Proto(eModelType, hFile, PreTransformMatrix)))
    {
        MSG_BOX("Failed To Created : CMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
    CMesh* pInstance = new CMesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMesh::Free()
{
    __super::Free();

    Safe_Delete_Array(m_pPos);
    Safe_Delete_Array(m_pIndices);

    if (m_bClone)
    {
        Safe_Delete_Array(m_pPos);
        Safe_Delete_Array(m_pIndices);
    }
}
