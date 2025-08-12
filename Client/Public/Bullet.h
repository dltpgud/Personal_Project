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
        _uint state = {  };
        _vector pTagetPos{};
    }CBULLET_DESC;
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
    virtual void Dead_Rutine(_float fTimeDelta) override;
    void Reset(void* Arg);

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();
    HRESULT BIND_BULLET_TYPE();
private:
    _vector m_pTagetPos = {};
    _vector m_vPlayerAt{};
    _vector m_vDir{};
    _bool m_bjump{ true };
    _bool m_bMakeWave = true;
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Point* m_pVIBufferCom = { nullptr };
    _float m_fCollSize = 0.f;
    _float2 m_pScale{};
_uint m_pParentState = { };


public:
    static CBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END