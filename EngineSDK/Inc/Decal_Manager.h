#pragma once
#include "Base.h"

BEGIN(Engine)

class CDecal_Manager : public CBase
{
private:
    struct DecalProtoInfo
    {
        _wstring filePathFmt; // 예: L"Textures/Decal_ProtoA_%d.dds"
        _uint texCount = 0;        // 이 프로토의 슬라이스 개수
    };
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
    class CDecal* Find_Prototype(const wstring& ProtoKey);
   
    void Preallocate(_wstring ProtoTag, size_t count, void* desc);
    HRESULT BuildGlobalDecalArray();

private:
    map<const _wstring, class CDecal*> m_ProtoDecal_Map;
    list<class CDecal*> m_Decals{};
    ID3D11Device* m_pDevice{};
    ID3D11DeviceContext* m_pContext{};

    class CVIBuffer_DecalCube* m_pVIBufferCom{};
    vector<DecalInstanceData> m_InstanceData;
    ID3D11ShaderResourceView* m_pDecalArraySRV{};
    unordered_map<wstring, DecalProtoInfo> m_ProtoInfo; // Key -> Info
    unordered_map<wstring, _uint> m_ProtoOffset;        // Key -> StartSlice
    _uint m_TotalSlices = 0;        
    USE_LOCK;

public:
    static CDecal_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END