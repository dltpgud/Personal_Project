#pragma once

#include "VIBuffer.h"
#include "Model.h"
BEGIN(Engine)
class CMesh final : public CVIBuffer
{
private:
    CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMesh(const CMesh& Prototype);
    virtual ~CMesh() = default;

public:

    virtual HRESULT Initialize_Proto(CModel::TYPE eModelType, HANDLE& hFile, _fmatrix PreTransformMatrix);

    virtual HRESULT Initialize(void* pArg) override;

    _uint Get_MaterialIndex() const
    {
        return m_iMaterialIndex;
    }

    HRESULT Bind_BoneMatrices(class CShader* pShader, const vector<class CBone*>& Bones, const _char* pConstantName);
    HRESULT Render();
    _float3* Get_pPos(_int i);
    _uint Get_pIndices(_int i);

    _uint Get_iNumIndexices();
    _uint Get_iNumVertices();
    HRESULT Set_InstanceBuffer(const vector<_matrix>& vecObjMat);
    HRESULT Bind_Buffers();
    private:
    HRESULT Load_AnimMesh(HANDLE hFile);
    HRESULT Load_NonAnimMesh(HANDLE hFile, _fmatrix PreTransformMatrix);

    _char			m_szName[MAX_PATH] = "";
    _uint			m_iMaterialIndex = { 0 };
    _uint			m_iNumBones = {0};
    vector<_uint>			m_Bones;
    vector<_float4x4>		m_OffsetMatrices;
    ID3D11Buffer* m_pInst_Buffer{};
    _uint m_iInstVertexStride{};
    _uint m_iNumInstance = {0};
    D3D11_BUFFER_DESC m_Inst_BufferDesc = {};
    D3D11_SUBRESOURCE_DATA m_Inst_BufferSRD = {};
    VTXMATRIX_INSTANCE* m_pInst_BufferData{};
    _float3* m_pPos{};
    _uint* m_pIndices{};
public:
    static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, HANDLE& hFile, _fmatrix PreTransformMatrix);

    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};
END