#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CTexture;
class CVIBuffer_Point;
class CTrailGPU;
END

BEGIN(Client)

class CHealthBall final : public CGameObject
{
public:

public:
    typedef struct CHealthBall_DESC : CGameObject::GAMEOBJ_DESC
    {
        _vector vPos{};
        _float fRadius = 0.1f;
        _float fTrailLength = 3.f;
        _float fTrailWidth = 0.4f;
    } CHealthBall_DESC;

private:
    CHealthBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CHealthBall(const CHealthBall& Prototype);
    virtual ~CHealthBall() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    virtual HRESULT Add_Components() override;
    HRESULT Bind_ShaderResources();
private:
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Point* m_pVIBufferCom = { nullptr };
    CShader* m_pShaderCom = {};
    CNavigation* m_pNavigationCom{};

    _float2 m_pScale{};
    _float3 m_fPrePos{};
    _float3 m_fCurPos{};
    _vector m_vDir;
    _bool   m_bStop{};
    _float4 m_Clolor{0.f,1.f,0.f,1.f};
    vector<_float3> m_vPosvec;
    CTrailGPU* m_pTrail{};
    _uint m_iTrailIndex{};

     bool bJumpStarted = false;

public:
    static CHealthBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END