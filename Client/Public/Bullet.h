#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Point;
END

BEGIN(Client)

class CBullet final : public CSkill
{
public:

public:
    typedef struct CBULLET_DESC : CSkill::Skill_DESC
    {
        _vector pTagetPos{};
        _float2 fScale{};
        _float fRadius = 0.1f;
        _float fTrailLength = 3.f;
        _float fTrailWidth = 0.4f;
    }
    CBULLET_DESC;
private:
    CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBullet(const CBullet& Prototype);
    virtual ~CBullet() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual void Dead_Rutine() override;

private:
    virtual HRESULT Add_Components() override;
    HRESULT Bind_ShaderResources();
private:
    _vector m_pTagetPos = {};
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Point* m_pVIBufferCom = { nullptr };
    _float2 m_pScale{};
    _float3 m_fPrePos{};
    _float3 m_fCurPos{};

public:
    static CBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END