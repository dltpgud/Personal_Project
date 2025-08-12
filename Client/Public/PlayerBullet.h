#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CPlayerBullet final : public CSkill
{
public:
    typedef struct CPlayerBullet_DESC : CSkill::Skill_DESC
    {
        _vector pTagetPos{};
        _vector vPlayerAt{};
        _vector Local{};
        const _float4x4* WorldPtr{};
        _uint iWeaponType{};
    }CPlayerBullet_DESC;
private:
    CPlayerBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPlayerBullet(const CPlayerBullet& Prototype);
    virtual ~CPlayerBullet() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual void Dead_Rutine(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    HRESULT BIND_BULLET_TYPE();
private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();
private:
    _vector m_pTagetPos = {};
    _vector m_vPlayerAt{};
    _vector m_vDir{};
    const _float4x4* m_WorldPtr = { nullptr };
    _vector m_Local{};
    _bool m_bStart{false};
    _float2  m_pScale{};
    _uint m_iWeaponType{};

    _uint m_iTexNum{ 0 };
    _float m_DeadSum{ 0.f };
private:
    CTexture* m_pTextureCom[2] = {nullptr};
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
public:
    static CPlayerBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END