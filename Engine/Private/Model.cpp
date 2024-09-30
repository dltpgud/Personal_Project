#include "Model.h"
#include "Mesh.h"
#include "MeshMaterial.h"
#include "Transform.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

CModel::CModel(const CModel& Prototype)
    : CComponent{Prototype} 
    , m_eModelType{ Prototype.m_eModelType }
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_iNumMeshes{ Prototype.m_iNumMeshes }
    , m_Meshes{ Prototype.m_Meshes }
    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials }
    , m_iNumAnimations{ Prototype.m_iNumAnimations }
    , m_Animations{ Prototype.m_Animations }
{
    for (auto& pAnimation : m_Animations)
        Safe_AddRef(pAnimation);

    for (auto& pPrototypeBone : Prototype.m_Bones) m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pMesh : m_Meshes) Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials) Safe_AddRef(pMaterial);

    //   m_ivtNum = new _uint[m_iNumMeshes];
    //   memcpy(m_ivtNum, Prototype.m_ivtNum, sizeof(_uint) * m_iNumMeshes);

    //   m_iNumIndex = new _uint[m_iNumMeshes];
    //   memcpy(m_iNumIndex, Prototype.m_iNumIndex, sizeof(_uint) * m_iNumMeshes);

    //  m_pPos = new _float3*[m_iNumMeshes];
    // m_pIndex = new _uint*[m_iNumMeshes];

    // for (_uint i = 0; i < m_iNumMeshes; i++)
    // {
    //     m_pPos[i] = new _float3[m_ivtNum[i]];
    //     m_pIndex[i] = new _uint[m_iNumIndex[i]];
    //
    //     memcpy(m_pPos[i], Prototype.m_pPos[i], sizeof(_float3) * m_ivtNum[i]);
    //     memcpy(m_pIndex[i], Prototype.m_pIndex[i], sizeof(_uint) * m_iNumIndex[i]);
    // }
}

HRESULT CModel::Initialize_Proto(TYPE eModelType, const TCHAR* pModelFilePath, _fmatrix PreTransformMatrix)
{
    m_eModelType = eModelType;

    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    Ready_AniModel(pModelFilePath);

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Bind_Material_ShaderResource(CShader* pShader, _uint iMeshIndex, aiTextureType eType, _uint iIndex,
                                             const _char* pConstantName)
{
    if (iMeshIndex >= m_Meshes.size())
        return E_FAIL;

    _uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    return m_Materials[iMaterialIndex]->Bind_ShaderResource(pShader, eType, iIndex, pConstantName);
}

HRESULT CModel::Render(_uint iMeshIndex)
{
    m_Meshes[iMeshIndex]->Bind_Buffers();
    m_Meshes[iMeshIndex]->Render();

    return S_OK;
}

HRESULT CModel::Bind_Mesh_BoneMatrices(CShader* pShader, _uint iMeshIndex, const _char* pConstantName)
{
    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, m_Bones, pConstantName);
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
    /* 모델의 뼈의 행렬(TransformationMatrix)을 현재 애니메이션에 맞는 상태로 갱신해준다. */
    _bool isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(m_Bones, m_IsLoop, fTimeDelta);

    /* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
    for (auto& pBone : m_Bones)
    {
        /*컴바인된 메트릭스 자체를 m_PreTransformMatrix로 회전시켜 올바른 위치를 바라 보게 하자 */
        pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
    }
    return isFinished;
}

_float CModel::Check_Pick(_vector RayPos, _vector RayDir, CTransform* pTransform)
{
    _matrix matWorld = pTransform->Get_WorldMatrix_Inverse();

    _vector CurRayPos = XMVector3TransformCoord(RayPos, matWorld);
    _vector CurRayDir = XMVector3TransformNormal(RayDir, matWorld);
    CurRayDir = XMVector3Normalize(CurRayDir);

    for (_uint i = 0; i < m_iNumMeshes; i++)
    {

        for (_uint j = 0; j < m_Meshes[i]->Get_iNumIndexices(); j++)
        {
            _float3 v0 = m_Meshes[i]->Get_pPos(i)[m_Meshes[i]->Get_pIndices(i)[j++]];
            _float3 v1 = m_Meshes[i]->Get_pPos(i)[m_Meshes[i]->Get_pIndices(i)[j++]];
            _float3 v2 = m_Meshes[i]->Get_pPos(i)[m_Meshes[i]->Get_pIndices(i)[j++]];

            //     _float3 v2 =        m_pPos[i][m_pIndex[i][j++]];
            _float fDist{};

            if (DirectX::TriangleTests::Intersects(CurRayPos, CurRayDir, XMLoadFloat3(&v0), XMLoadFloat3(&v1),
                                                   XMLoadFloat3(&v2), fDist))
            {
                return fDist;
            }
        }
    }

    return _float(0xffff);
}

/*
HRESULT CModel::Ready_Model(const _tchar* pModelFilePath)
{
    HANDLE hFile = CreateFile(pModelFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    DWORD dwByte = {0};

    _bool bReadFile;
    bReadFile = ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);
    m_ivtNum = new _uint[m_iNumMeshes];
    m_pPos = new _float3*[m_iNumMeshes];
    m_pIndex = new _uint*[m_iNumMeshes];
    m_iNumIndex = new _uint[m_iNumMeshes];

    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        _char szName[1024];
        bReadFile = ReadFile(hFile, &szName, sizeof(char) * 1024, &dwByte, nullptr);

        _uint iNumMaterialIndex;
        bReadFile = ReadFile(hFile, &iNumMaterialIndex, sizeof(iNumMaterialIndex), &dwByte, nullptr);

        _uint iNumVertex;
        bReadFile = ReadFile(hFile, &iNumVertex, sizeof(iNumVertex), &dwByte, nullptr);
        m_ivtNum[i] = iNumVertex;

        _uint iNumFaces;

        bReadFile = ReadFile(hFile, &iNumFaces, sizeof(iNumFaces), &dwByte, nullptr);
        m_iNumIndex[i] = iNumFaces * 3;

        m_pPos[i] = new _float3[iNumVertex];
        VTXMESH* pVertices = new VTXMESH[iNumVertex];
        bReadFile = ReadFile(hFile, pVertices, sizeof(VTXMESH) * iNumVertex, &dwByte, nullptr);

        for (_uint j = 0; j < iNumVertex; j++) { m_pPos[i][j] = pVertices[j].vPosition; }

        m_pIndex[i] = new _uint[iNumFaces * 3];
        bReadFile = ReadFile(hFile, m_pIndex[i], sizeof(_uint) * iNumFaces * 3, &dwByte, nullptr);

        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, szName, iNumMaterialIndex, *m_ivtNum, iNumFaces, pVertices,
                                     m_pIndex[i], XMLoadFloat4x4(&m_PreTransformMatrix));

        Safe_Delete_Array(pVertices);

        if (!pMesh)
        {
            MSG_BOX("Failed To Ready_Meshes : Mesh");
            return E_FAIL;
        }

        m_Meshes.push_back(pMesh);
    }

    bReadFile = ReadFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < m_iNumMaterials; i++)
    {
        _uint iNumTexture[AI_TEXTURE_TYPE_MAX]{};
        bReadFile = ReadFile(hFile, iNumTexture, sizeof(_uint) * AI_TEXTURE_TYPE_MAX, &dwByte, nullptr);

        _tchar*** szMaterialPath = new _tchar**[AI_TEXTURE_TYPE_MAX];
        for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
        {
            szMaterialPath[j] = new _tchar*[iNumTexture[j]];

            for (_uint k = 0; k < iNumTexture[j]; k++)
            {
                szMaterialPath[j][k] = new _tchar[MAX_PATH];

                bReadFile = ReadFile(hFile, szMaterialPath[j][k], sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
            }
        }

        CMeshMaterial* pMaterial =
            CMeshMaterial::Create(m_pDevice, m_pContext, iNumTexture, (const _tchar***)szMaterialPath);

        m_Materials.push_back(pMaterial);

        for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
        {
            for (_uint k = 0; k < iNumTexture[j]; k++) { Safe_Delete_Array(szMaterialPath[j][k]); }

            Safe_Delete_Array(szMaterialPath[j]);
        }

        Safe_Delete_Array(szMaterialPath);
    }

    CloseHandle(hFile);

    return S_OK;
}*/

HRESULT CModel::Ready_AniModel(const _tchar* pModelFilePath)
{
    HANDLE hFile = CreateFile(pModelFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    DWORD dwByte = {0};

    _bool bReadFile;

    bReadFile = ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, hFile, XMLoadFloat4x4(&m_PreTransformMatrix));

        if (!pMesh)
        {
            MSG_BOX("Failed To Load Mesh");
            return E_FAIL;
        }

        m_Meshes.push_back(pMesh);
    }

    bReadFile = ReadFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < m_iNumMaterials; i++)
    {
        CMeshMaterial* pMaterial = CMeshMaterial::Create(m_pDevice, m_pContext, hFile);

        if (!pMaterial)
        {
            MSG_BOX("Failed To Load pMaterial");
            return E_FAIL;
        }

        m_Materials.push_back(pMaterial);
    }

    if (m_eModelType == TYPE_NONANIM)
    {
        CloseHandle(hFile);
        return S_OK;
    }

    _uint iNumBone{};
    bReadFile = ReadFile(hFile, &iNumBone, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iNumBone; i++)
    {
        CBone* pBone = CBone::Create(hFile);

        if (!pBone)
        {
            MSG_BOX("Failed To Load Bone Information");
            return E_FAIL;
        }
        m_Bones.push_back(pBone);
    }

    bReadFile = ReadFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < m_iNumAnimations; i++)
    {
        CAnimation* pAnimation = CAnimation::Create(hFile);

        if (!pAnimation)
        {
            MSG_BOX("Failed To Load Animation");
            return E_FAIL;
        }

        m_Animations.push_back(pAnimation);
    }

    CloseHandle(hFile);

    return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType,
                       const TCHAR* pModelFilePath, _fmatrix PreTransformMatrix)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Proto(eModelType, pModelFilePath, PreTransformMatrix)))
    {
        MSG_BOX("Failed To Created : CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModel::Free()
{
    __super::Free();

    for (auto& pAnimation : m_Animations) Safe_Release(pAnimation);


    for (auto& pBone : m_Bones) { Safe_Release(pBone); }

    for (auto& pMaterial : m_Materials) Safe_Release(pMaterial);

    for (auto& pMesh : m_Meshes) Safe_Release(pMesh);

    // for (_uint i = 0; i < m_iNumMeshes; i++)
    //{
    //     Safe_Delete_Array(m_pPos[i]);
    //     Safe_Delete_Array(m_pIndex[i]);
    // }
    //
    // Safe_Delete_Array(m_pPos);
    // Safe_Delete_Array(m_pIndex);
    //
    // Safe_Delete_Array(m_ivtNum);
    // Safe_Delete_Array(m_iNumIndex);
}
