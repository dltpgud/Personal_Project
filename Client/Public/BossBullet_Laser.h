#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CModel;
class CTexture;
END

BEGIN(Client)

class CBossBullet_Laser final : public CSkill
{
public:
    typedef struct CBossBullet_Laser_DESC : CSkill::Skill_DESC
    {
        _bool            bRightLeft    = {};
        const _float4x4* pSocketMatrix = { nullptr };
        const _float4x4* pParentMatrix = { nullptr };
    } CBossBullet_Laser_DESC;

private:
    CBossBullet_Laser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBossBullet_Laser(const CBossBullet_Laser& Prototype);
    virtual ~CBossBullet_Laser() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    Late_Update(_float fTimeDelta) override;
    virtual void    Dead_Rutine() override;
    virtual HRESULT Render() override;

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();
private:
    CModel*          m_pModelCom     = {};
    CTexture*        m_pTextureCom   = {};
    _vector          m_pTagetPos     = {};
    const _float4x4* m_pSocketMatrix = {};
    const _float4x4* m_pParentMatrix = {};
    _float4x4        m_WorldMatrix   = {};
    _bool            m_bRightLeft    = {};

public:
    static CBossBullet_Laser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END