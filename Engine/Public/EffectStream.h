#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CEffectStream : public CBase
{
protected:
    CEffectStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEffectStream() = default;

public:

    virtual void Update(_float fTimeDelta) PURE;

    virtual HRESULT Render(class CShader* pShader) PURE;

    virtual HRESULT Trigger_Effect(void* pArg, _float fTimeDelta) PURE;

protected:
    virtual HRESULT Initialize(void* pArg) PURE;
    HRESULT CreateStructuredBuffer(_uint elementCount, _uint stride, ID3D11Buffer** outBuffer,ID3D11ShaderResourceView** outSRV, ID3D11UnorderedAccessView** outUAV, const void* initData = nullptr) ;
    HRESULT CreateRawBuffer(_uint byteWidth, ID3D11Buffer** outBuffer, ID3D11UnorderedAccessView** outUAV, _bool allowDrawIndirect = false);
    HRESULT Create_CS(_wstring strFilePath, _string strEntryPoint, ID3D11ComputeShader** ppComputeShader);

protected:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    class CGameInstance* m_pGameInstance = nullptr;

public:
    virtual void Free() override;

};
END 
