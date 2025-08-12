#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CBossBullet_Berrle final : public CSkill
{
public:


public:
    typedef struct CBossBullet_Berrle_DESC : CSkill::Skill_DESC
    {
        _vector pTagetPos{};
        const _float4x4* LaserpSocketMatrix = { nullptr };
        const _float4x4* LaserpParentMatrix = { nullptr };
         _uint state = {  };
        _bool LaserRightLeft{};
    }CBossBullet_Berrle_DESC;
private:
    CBossBullet_Berrle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBossBullet_Berrle(const CBossBullet_Berrle& Prototype);
    virtual ~CBossBullet_Berrle() = default;

public:

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void Dead_Rutine(_float fTimeDelta) override;
    HRESULT Initialize_SkillType();

private:
    HRESULT Add_Components();

    HRESULT Bind_ShaderResources();
private:
    CModel* m_pModelCom = { nullptr };

private:
    _vector m_pTagetPos = {};
    _vector m_vDir{};
    _float  m_fScale{};

    _float4x4 m_WorldMatrix{};
   const _float4x4* m_LaserpSocketMatrix = { nullptr };
   const   _float4x4*m_LaserpParentMatrix = { nullptr };
   _bool m_LaserRightLeft{};
   _uint m_pParentState = {};
public:
    static CBossBullet_Berrle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END