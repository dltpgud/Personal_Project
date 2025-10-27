#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Point;
END

BEGIN(Client)

class CShockWave_Bullet final : public CSkill
{
public:

public:
    typedef struct CBULLET_DESC : CSkill::Skill_DESC
    {
        _vector pTagetPos{};
        _float2 fScale{};
        _float fRadius = 0.1f;
    }
    CBULLET_DESC;
private:
    CShockWave_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CShockWave_Bullet(const CShockWave_Bullet& Prototype);
    virtual ~CShockWave_Bullet() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual void Dead_Rutine() override;
    virtual HRESULT CreateEffect(_vector RayStartPos, _vector RayDir, _vector RayEndPos,
                                 _vector vNomal = XMVectorZero(), void* pArg = nullptr) override;


private:
    virtual HRESULT Add_Components() override;
    HRESULT Bind_ShaderResources();
private:
    _vector m_pTagetPos = {};
    CTexture* m_pTextureCom = { nullptr };
    CTexture* m_pTrailTextureCom = {nullptr};
    CVIBuffer_Point* m_pVIBufferCom = { nullptr };
    _float2 m_pScale{};
    _uint m_iTrailIndex{};

public:
    static CShockWave_Bullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END