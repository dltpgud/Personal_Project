#include "Model.h"
#include "Mesh.h"
#include "MeshMaterial.h"
#include "Transform.h"
#include "Bone.h"
#include "Animation.h"
#include "GameInstance.h"
CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

CModel::CModel(const CModel& Prototype)
    : CComponent{Prototype}, m_eModelType{Prototype.m_eModelType}, m_PreTransformMatrix{Prototype.m_PreTransformMatrix},
      m_iNumMeshes{Prototype.m_iNumMeshes}, m_Meshes{Prototype.m_Meshes}, m_iNumMaterials{Prototype.m_iNumMaterials},
      m_Materials{Prototype.m_Materials}, m_iNumAnimations{Prototype.m_iNumAnimations}
{
    for (auto& pPrototypeAnimation : Prototype.m_Animations) m_Animations.push_back(pPrototypeAnimation->Clone());

    for (auto& pPrototypeBone : Prototype.m_Bones) m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pMesh : m_Meshes) Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials) Safe_AddRef(pMaterial);
}

HRESULT CModel::Initialize_Proto(TYPE eModelType, const TCHAR* pModelFilePath, _fmatrix PreTransformMatrix)
{
    m_eModelType = eModelType;

    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
    
    Ready_Model(pModelFilePath);

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

_uint CModel::Get_BoneIndex(const _char* pBoneName) const
{
    _uint iBoneIndex = {0};
    auto iter = find_if(m_Bones.begin(), m_Bones.end(),
                        [&](class CBone* pBone) -> _bool
                        {
                            if (!strcmp(pBone->Get_Name(), pBoneName))
                                return true;

                            ++iBoneIndex;

                            return false;
                        });

    return iBoneIndex;
}

const _float4x4* CModel::Get_BoneMatrix(const _char* pBoneName) const
{
    return m_Bones[Get_BoneIndex(pBoneName)]->Get_CombinedTransformationFloat4x4Ptr();
}

const void CModel::Set_BoneUpdateMatrix(const _uint iIndex, _fmatrix NewMatrix) const
{
    vector<_int> vecParentNum{};
    _int FindBone = iIndex;
    vecParentNum.reserve(m_Bones.size());
    vecParentNum.push_back(FindBone);

    m_Bones[FindBone]->New_CombinedTransformationMatrix(NewMatrix);

    for (_int i = FindBone + 1;  i < m_Bones.size(); i++)
    {
      for (auto j = vecParentNum.begin(); j != vecParentNum.end(); j++)
      {
          if (*j == m_Bones[i]->Get_ParentBoneIndex())
          {
              m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
              vecParentNum.push_back(i);
          }
      } 
    }
}

HRESULT CModel::Bind_Material_ShaderResource(CShader* pShader, _uint iMeshIndex, aiTextureType eType, _uint iIndex,
                                             const _char* pConstantName)
{
    if (iMeshIndex >= m_Meshes.size())
     return E_FAIL;

    _uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    return m_Materials[iMaterialIndex]->Bind_ShaderResource(pShader, eType, iIndex, pConstantName);
}

HRESULT CModel::InsertAiTexture(aiTextureType eTextureType, _uint MashiIndex, const _tchar* Path)
{
   if (MashiIndex >= m_Meshes.size())
       return E_FAIL;

   _uint iMaterialIndex = m_Meshes[MashiIndex]->Get_MaterialIndex();

   return m_Materials[iMaterialIndex]->InsertAiTexture(eTextureType, Path);

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
    _bool isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(m_Bones, m_IsLoop, fTimeDelta);

    for (auto& pBone : m_Bones)
    {
         pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
    }

    return isFinished;
}

void CModel::Set_Animation(_uint index, _bool IsLoop)
{
    m_iCurrentAnimIndex = index;
    m_IsLoop = IsLoop;

    m_Animations[m_iCurrentAnimIndex]->init_Loop(m_Bones);
}

void CModel::init_Loop()
{
    m_Animations[m_iCurrentAnimIndex]->init_Loop(m_Bones);
}

HRESULT CModel::Set_InstanceBuffer(const vector<_matrix>& vecObjMat)
{
    for (auto& pMesh : m_Meshes)
        if (FAILED(pMesh->Set_InstanceBuffer(vecObjMat)))
            return E_FAIL;
    return S_OK;
}

void CModel::Center_Ext(_float3* Center, _float3* extend)
{
    _float3 minPoint(FLT_MAX, FLT_MAX, FLT_MAX);
    _float3 maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);


    for (_uint i = 0; i < m_iNumMeshes; i++)
    {
        _uint NumIndexces = m_Meshes[i]->Get_iNumVertices();
        for (_uint j = 0; j < NumIndexces; j++)
        {
            minPoint.x = min(minPoint.x, m_Meshes[i]->Get_pPos(j)->x);
            minPoint.y = min(minPoint.y, m_Meshes[i]->Get_pPos(j)->y);
            minPoint.z = min(minPoint.z, m_Meshes[i]->Get_pPos(j)->z);

            maxPoint.x = max(maxPoint.x, m_Meshes[i]->Get_pPos(j)->x);
            maxPoint.y = max(maxPoint.y, m_Meshes[i]->Get_pPos(j)->y);
            maxPoint.z = max(maxPoint.z, m_Meshes[i]->Get_pPos(j)->z);
        }
    }

    *Center = {
    (minPoint.x + maxPoint.x) / 2.0f,
    (minPoint.y + maxPoint.y) / 2.0f,
    (minPoint.z + maxPoint.z) / 2.0f
    };

    *extend   = {
        (maxPoint.x - minPoint.x) / 2.0f,
        (maxPoint.y - minPoint.y) / 2.0f,
        (maxPoint.z - minPoint.z) / 2.0f
    };
}

void CModel::Callback(_uint AnimIdx, _int Duration, function<void()> func)
{
    m_Animations[AnimIdx]->Callback(Duration, func);
}

_bool CModel::RayIntersect(_vector vRayPos_WS, _vector vRayDir_WS, CTransform* pTransform, OUT _vector& vHitPos_WS,
                           OUT _vector& vHitN_WS, OUT _float* fDist)
{
  _matrix W = pTransform->Get_WorldMatrix();
   _matrix Wi = pTransform->Get_WorldMatrix_Inverse();

   _vector oL = XMVector3TransformCoord(vRayPos_WS, Wi);
   _vector dL = XMVector3Normalize(XMVector3TransformNormal(vRayDir_WS, Wi));

   struct HitResult
   {
       _bool hit = false;
       _float dist = FLT_MAX;
       _vector posL = XMVectorZero();
       _vector nL = XMVectorZero();
   };

   HitResult g_bestResult;

     if (m_eModelType == TYPE_ANIM)
       for (auto& mesh : m_Meshes) mesh->Build_MeshAABB_Local();

   _float fDis;
   for (auto& mesh : m_Meshes)
   {
       HitResult localBest;
   
       _float3 MeahMin = mesh->GetAABBMinLocal();
       _float3 MeahMax = mesh->GetAABBMaxLocal();
       _float3 Center = {(MeahMin.x + MeahMax.x) / 2.0f, (MeahMin.y + MeahMax.y) / 2.0f, (MeahMin.z + MeahMax.z) / 2.0f};
       _float3 extend = {(MeahMax.x - MeahMin.x) / 2.0f, (MeahMax.y - MeahMin.y) / 2.0f, (MeahMax.z - MeahMin.z) / 2.0f};

       BoundingBox AABB = BoundingBox(Center, extend);

       if (!AABB.Intersects(oL, dL, fDis))
           continue;

       const _uint triCount = mesh->Get_iNumIndexices() / 3;

       for (_uint t = 0; t < triCount; ++t)
       {
           _uint i0 = mesh->Get_pIndices(t * 3 + 0);
           _uint i1 = mesh->Get_pIndices(t * 3 + 1);
           _uint i2 = mesh->Get_pIndices(t * 3 + 2);

           const _float3& A = GetVertexPos(mesh, i0);   
           const _float3& B = GetVertexPos(mesh, i1); 
           const _float3& C = GetVertexPos(mesh, i2);   

           // --- Per-tri AABB ---
           _float3 triMin{min(A.x, min(B.x, C.x)), min(A.y, min(B.y, C.y)), min(A.z, min(B.z, C.z))};
           _float3 triMax{max(A.x, max(B.x, C.x)), max(A.y, max(B.y, C.y)), max(A.z, max(B.z, C.z))};
           _float3 Center = {(triMin.x + triMax.x) / 2.0f, (triMin.y + triMax.y) / 2.0f, (triMin.z + triMax.z) / 2.0f};
           _float3 extend = {(triMax.x - triMin.x) / 2.0f, (triMax.y - triMin.y) / 2.0f, (triMax.z - triMin.z) / 2.0f};
            
           BoundingBox AABB = BoundingBox(Center,extend);
           
           if (!AABB.Intersects(oL, dL, fDis))
               continue;

           // --- Backface Cull ---
           XMVECTOR vA = XMLoadFloat3(&A);
           XMVECTOR vB = XMLoadFloat3(&B);
           XMVECTOR vC = XMLoadFloat3(&C);
           XMVECTOR e0 = XMVectorSubtract(vB, vA);
           XMVECTOR e1 = XMVectorSubtract(vC, vA);
           XMVECTOR n = XMVector3Normalize(XMVector3Cross(e0, e1));
           if (XMVectorGetX(XMVector3Dot(n, dL)) > 0.f)
               continue;

           _float tDist = 0.f;
           if (DirectX::TriangleTests::Intersects(oL, dL, vA, vB, vC, tDist))
           {
               if (tDist < localBest.dist)
               {
                   localBest.hit = true;
                   localBest.dist = tDist;
                   localBest.posL = XMVectorAdd(oL, XMVectorScale(dL, tDist));
                   localBest.nL = n;
               }
           }
       }

       if (localBest.hit)
       {
          g_bestResult = localBest;
           break;
       }
   }

   if (!g_bestResult.hit)
       return false;

   vHitPos_WS = XMVector3TransformCoord(g_bestResult.posL, W);
   vHitN_WS = XMVector3Normalize(XMVector3TransformNormal(g_bestResult.nL, W));

  if (XMVectorGetX(XMVector3Dot(vHitN_WS, vRayDir_WS)) > 0.f)
       vHitN_WS = XMVectorNegate(vHitN_WS);

  if (fDist)
   *fDist = g_bestResult.dist;
  
   return true;
}

_float3 CModel::GetVertexPos(CMesh* Mash, _int Pos)
{
    if (m_eModelType == TYPE_NONANIM)
    {
        return *Mash->Get_pPos(Pos);
    }
    else 
     
    return Mash->GetVetexPosAnim(Pos);
}

CMesh* CModel::Get_Mash(_uint Mashinx)
{
    return m_Meshes[Mashinx];
}

HRESULT CModel::Ready_Model(const _tchar* pModelFilePath)
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
}
