#pragma once
#include "Base.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Cube;
class CDecal : public CBase
{
public:
    CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDecal(const CDecal& Prototype);
    virtual ~CDecal() = default;

public:
    HRESULT Initialize_Proto(const _tchar* FilePath, const _uint& TexNum);
    HRESULT Initialize(void* pArg);
    void Update(_float fTimeDelta);
    HRESULT Render(class CShader* pShader);
    _uint Get_Dead()
    {
        return m_bDead;
    }
    _wstring GetPoolID() { return L"CDecal"; };
private:
    class CVIBuffer_Cube* m_pVIBufferCom = nullptr;
    class CTexture* m_pTextureCom{};
    ID3D11Device* m_pDevice{};
    ID3D11DeviceContext* m_pContext{};

private:
    _float m_fLifeTime{}, m_fDecalTime{};
    _uint m_bDead{OBJ_NOEVENT};
    _float3 m_fDecalPos{};
    _float3 m_fDecalDir{};

public:
    static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* FilePath,
                          const _uint& TexNum);
    CDecal* Clone(void*);
    virtual void Free() override;
};
END
