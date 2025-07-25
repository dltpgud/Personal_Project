#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CVIBuffer{pDevice, pContext}
{
}

CMesh::CMesh(const CMesh& Prototype)
    : CVIBuffer{Prototype}, m_iMaterialIndex{Prototype.m_iMaterialIndex}
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

    for (_uint i = 0; i < m_iNumIndexices; i++) { m_pIndices[i] = pIndices[i]; }
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

    _uint iNumIndices = {0};

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

#pragma endregion
    Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const vector<class CBone*>& Bones, const _char* pConstantName)
{
    _float4x4 BoneMatrices[512];

    _uint iNumBones = {0};

    for (auto& iBoneIndex : m_Bones) /*뼈의 인덱스 만큼 돌면서 행렬을 곱해라*/
    {
        _matrix CombinedMatrix = Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
        _matrix OffsetMatrix = XMLoadFloat4x4(&m_OffsetMatrices[iNumBones]);

        XMStoreFloat4x4(&BoneMatrices[iNumBones++], OffsetMatrix * CombinedMatrix);
    }

    return pShader->Bind_Matrices(pConstantName, BoneMatrices, 512);
}

HRESULT CMesh::Render()
{

    if (m_pInst_Buffer == nullptr)
        return __super::Render(); // 인스턴싱이 아닐경우 , 상위 렌더 함수 호출
    else   // 인스턴싱
        m_pContext->DrawIndexedInstanced(m_iNumIndexices, m_iNumInstance, 0, 0, 0);


    return S_OK;
}

_float3* CMesh::Get_pPos(_int i)
{
    return &m_pPos[i];
}

_uint CMesh::Get_pIndices(_int i)
{
    return m_pIndices[i];
}

_uint CMesh::Get_iNumIndexices()
{
    return m_iNumIndexices;
}

_uint CMesh::Get_iNumVertices()
{
    return m_iNumVertices;
}

HRESULT CMesh::Set_InstanceBuffer(const vector<_matrix>& vecObjMat)
{
    m_iNumInstance = (_uint)vecObjMat.size();         //인스턴싱할 월드 메트리스 개수
    m_iInstVertexStride = sizeof VTXMATRIX_INSTANCE;  //인스턴스 데이터를 구성하는 정점 하나당 바이트 크기
    m_iNumVertexBuffers = 2;                         // 사용할 버퍼개수

   // 인스턴스 버퍼 설정
    ZeroMemory(&m_Inst_BufferDesc, sizeof m_Inst_BufferDesc); 
    m_Inst_BufferDesc.ByteWidth = m_iInstVertexStride * m_iNumInstance;
    m_Inst_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_Inst_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_Inst_BufferDesc.CPUAccessFlags = 0;
    m_Inst_BufferDesc.MiscFlags = 0;
    m_Inst_BufferDesc.StructureByteStride = m_iInstVertexStride;


    // m_pInst_BufferData :: 인스턴싱할 월드 메트릭스 정보를 담은 VTXMATRIX_INSTANCE 구조체 변수
    m_pInst_BufferData = new VTXMATRIX_INSTANCE[m_iNumInstance];
    for (size_t i = 0; i < m_iNumInstance; ++i)  
    {  // 인스터싱할 월드 메트리스 개수 만큼 반복하여 월드 메트릭스 정보를 채워준다.
        _matrix WorldMat = vecObjMat[i];
        XMStoreFloat4(&m_pInst_BufferData[i].vRight, WorldMat.r[0]);
        XMStoreFloat4(&m_pInst_BufferData[i].vUp, WorldMat.r[1]);
        XMStoreFloat4(&m_pInst_BufferData[i].vLook, WorldMat.r[2]);
        XMStoreFloat4(&m_pInst_BufferData[i].vPos, WorldMat.r[3]);
    }

    ZeroMemory(&m_Inst_BufferSRD, sizeof m_Inst_BufferSRD);
    m_Inst_BufferSRD.pSysMem = m_pInst_BufferData;

    if (FAILED(m_pDevice->CreateBuffer(&m_Inst_BufferDesc, &m_Inst_BufferSRD, &m_pInst_Buffer))) // 인스턴싱 버퍼를 생성한다
        return E_FAIL;

    Safe_Release(m_pIB); //기존의 인덱버퍼를 해제하고, 인스턴싱 개수를 반영해 다시 만든다.
    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndexices * m_iNumInstance;

    _uint* pIndices = new _uint[m_iNumIndexices * m_iNumInstance];
      for (_uint i = 0; i < m_iNumInstance; ++i)
         memcpy(&pIndices[i * m_iNumIndexices], m_pIndices, sizeof(_uint) * m_iNumIndexices);

    D3D11_SUBRESOURCE_DATA m_tInitialData_Inst{};
    m_tInitialData_Inst.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_tInitialData_Inst, &m_pIB)))
        return E_FAIL;

   Safe_Delete_Array(pIndices);
   return S_OK;
}

HRESULT CMesh::Bind_Buffers()
{
    	if (nullptr == m_pContext)
        return E_FAIL;

        else if (nullptr == m_pInst_Buffer)
            return __super::Bind_Buffers(); // 인스턴싱이 아닐 경우, 하나의 버텍스 버퍼를 사용해 그리게 한다.

        ID3D11Buffer* pVertexBuffers[] = {
            m_pVB,  // 정점 버퍼
            m_pInst_Buffer, // 인스턴스 버퍼
        };

        _uint iVertexStrides[] = {m_iVertexStride, m_iInstVertexStride};

        _uint iOffsets[] = {
            0,
            0,
        };
        /*정점들을 장치에 바인딩 한다*/
        m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
        /* 인덱스를 장치에 바인딩한다. */ 
        m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

        //기본 형식 및 입력 어셈블러 단계의 입력 데이터를 설명하는 데이터 순서에 대한 정보를 바인딩
        m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
     
    return S_OK;
}

HRESULT CMesh::Load_AnimMesh(HANDLE hFile)
{
    DWORD dwByte{};
    _bool bReadFile{};

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];

    bReadFile = ReadFile(hFile, pVertices, sizeof(VTXANIMMESH) * m_iNumVertices, &dwByte, nullptr);
    m_pPos = new _float3[m_iNumVertices];
    for (size_t i = 0; i < m_iNumVertices; i++) { m_pPos[i] = pVertices[i].vPosition; }

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
        XMStoreFloat3(&pVertices[i].vPosition,
                      XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
        m_pPos[i] = pVertices[i].vPosition;
        pVertices[i].vNormal = pLoadVertices[i].vNormal;
        XMStoreFloat3(&pVertices[i].vPosition,
                      XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

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

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, HANDLE& hFile,
                     _fmatrix PreTransformMatrix)
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
    Safe_Release(m_pInst_Buffer);
    Safe_Delete(m_pInst_BufferData);
}
