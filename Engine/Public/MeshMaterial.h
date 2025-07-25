#pragma once
#include "Base.h"

BEGIN(Engine)
class CMeshMaterial : public CBase
{
private:
    CMeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CMeshMaterial() = default;

public:

    HRESULT Bind_ShaderResource(class CShader* pShader, aiTextureType eTextureType, _uint iIndex,
                                const _char* pConstantName);

    HRESULT Initialize(HANDLE& hFile);

    HRESULT InsertAiTexture(aiTextureType eTextureType, const _tchar* Path);

private:
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};
    vector<ID3D11ShaderResourceView*> m_Materials[AI_TEXTURE_TYPE_MAX];

public:
    static CMeshMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE& hFile);
    virtual void Free() override;
};
END
