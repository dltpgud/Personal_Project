#pragma once

#include "Component.h"

BEGIN(Engine)
class CTransform;
class ENGINE_DLL CModel final : public CComponent
{
public:
    enum TYPE
    {
        TYPE_NONANIM,
        TYPE_ANIM,
        TYPE_END
    };
    
private:
    CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CModel(const CModel& Prototype);
    virtual ~CModel() = default;

public:
    virtual HRESULT Initialize_Proto(TYPE eModelType, const TCHAR* pModelFilePath,
                                     _fmatrix PreTransformMatrix = XMMatrixIdentity());
    virtual HRESULT Initialize(void* pArg) override;

public:
    _uint Get_NumMeshes() const
    {
        return m_iNumMeshes;
    }

    _uint Get_BoneIndex(const _char* pBoneName) const;
    const _float4x4* Get_BoneMatrix(const _char* pBoneName) const;
    const void Set_BoneUpdateMatrix(const _uint iIndex, _fmatrix NewMatrix) const;


    HRESULT Bind_Material_ShaderResource(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, _uint iIndex,
                                         const _char* pConstantName);
    HRESULT InsertAiTexture(aiTextureType eTextureType, _uint MashiIndex, const _tchar* Path);

    HRESULT Render(_uint iMeshIndex);

    HRESULT Bind_Mesh_BoneMatrices(class CShader* pShader, _uint iMeshIndex, const _char* pConstantName);
    _bool Play_Animation(_float fTimeDelta);

    void Set_Animation(_uint index, _bool IsLoop = false);


    void init_Loop();
    HRESULT Set_InstanceBuffer(const vector<_matrix>& vecObjMat);
    void Center_Ext(_float3* Center, _float3* extend);

    void Callback(_uint AnimIdx, _int Duration, function<void()> func);

    _bool RayIntersect(_vector vRayPos_WS, _vector vRayDir_WS, CTransform* pTransform, OUT _vector& vHitPos_WS, OUT _vector& vHitN_WS, OUT _float* fDist = nullptr);
   
    _float3 GetVetexPos(class CMesh* Mash, _int Pos);

  class CMesh* Get_Mash(_uint Mashinx);

public:

    HRESULT Ready_Model(const _tchar* pModelFilePath);

private:
    TYPE m_eModelType = {TYPE_END};
    _float4x4 m_PreTransformMatrix = {};
    _uint m_iNumMeshes = {0};
    vector<class CMesh*> m_Meshes;

    _uint m_iNumMaterials = {0};
    vector<class CMeshMaterial*> m_Materials;

    vector<class CBone*> m_Bones;

    _bool m_IsLoop = {false};
    _uint m_iCurrentAnimIndex = {};
    _uint m_iNumAnimations = {0};
    vector<class CAnimation*> m_Animations;
    

private:
    ID3D11Buffer* m_pPositionsBuffer;
    ID3D11ShaderResourceView* m_pPositionsSRV;
    ID3D11Buffer* m_pIndicesBuffer;
    ID3D11ShaderResourceView* m_pIndicesSRV;
    UINT m_TotalIndices = 0;
    UINT m_TotalVertices = 0;


public:
    static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType,
                          const TCHAR* pModelFilePath, _fmatrix PreTransformMatrix);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END