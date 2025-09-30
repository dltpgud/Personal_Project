#pragma once
#include "Base.h"

BEGIN(Engine)

class CDecal_Manager : public CBase
{
private:
    CDecal_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CDecal_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Add_DecalProto(const wstring& Key, const _tchar* FilePath, const _uint& TexNum);
    HRESULT Add_Decal(const wstring& Key,  const DECAL_DESC* DecalDesc);
    HRESULT Update(_float fTimeDelta);
    HRESULT Render(class CShader* pShader);
    HRESULT Clear();
    HRESULT Delete();

private:
    map<const _wstring, class CDecal*> m_ProtoDecal_Map;
    list<class CDecal*> m_Decals{};
    ID3D11Device* m_pDevice{};
    ID3D11DeviceContext* m_pContext{};

public:
    static CDecal_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END