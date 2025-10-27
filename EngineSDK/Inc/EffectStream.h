#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CEffectStream : public CBase
{
protected:
    CEffectStream(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEffectStream() = default;

public:

    virtual HRESULT Initialize(void* pArg) PURE;

    virtual void Update(_float fTimeDelta) PURE;

    virtual HRESULT Render(class CShader* pShader) PURE;

    virtual HRESULT Trigger_Effect(void* pArg) PURE;

    virtual void Clear_EffectPlan() {};

    virtual void Register_EffectPlan(_uint iEffectID, _float fDelayTime, _float fLifeTime,
                                     const _float4x4* pTransform) {};

    virtual void Register_EffectPlanOncePtr(_uint iEffectID, _float fDelayTime, _float fLifeTime,
                                            const _float4x4* pTransform) {};

    virtual _bool IsNoAlive() const {return false;};

protected:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    class CGameInstance* m_pGameInstance = nullptr;

public:
    virtual void Free() override;

};
END