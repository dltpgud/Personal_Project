#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;

END

BEGIN(Client)
class CTrail;
class CPlayerBullet final : public CSkill
{
public:
    typedef struct CPlayerBullet_DESC : CSkill::Skill_DESC
    {
        _vector vTagetPos{};
        _vector vLocalPos{};
        const _float4x4* fWorldPtr{};
        _float2 fScale{};
        _float fTrailLength = 10.f;
        _float fTrailWidth = 0.4f;
    }CPlayerBullet_DESC;

public:
    CPlayerBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPlayerBullet(const CPlayerBullet& Prototype);
    virtual ~CPlayerBullet() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual void Dead_Rutine() override;
    virtual HRESULT Render() override;

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();
private:
    _vector m_vTagetPos = {};
    _vector m_vDir{};
    const _float4x4* m_WorldPtr = { nullptr };
    _vector m_vLocalPos{};
    _bool m_bStart{false};
    _float2  m_fScale{};
    _float3 m_fPrePos{};
    _float3 m_fCurPos{};

private:
    CTexture* m_pTextureCom = {nullptr};
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
  
public:
    static CPlayerBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END