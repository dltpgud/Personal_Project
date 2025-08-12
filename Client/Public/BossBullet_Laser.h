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


public:
    typedef struct CBossBullet_Laser_DESC : CSkill::Skill_DESC
    {
         _uint state = { };
        _bool    bRightLeft{ nullptr };
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
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;

    virtual void Dead_Rutine(_float fTimeDelta) override;
    virtual HRESULT Render() override;
private:
    HRESULT Add_Components();

    HRESULT Bind_ShaderResources();
private:
    CModel* m_pModelCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };
private:
    _vector m_pTagetPos = {};
    _vector* m_vNPos{nullptr};
    _vector* m_vNRight{ nullptr };
    _vector* m_vNUP{ nullptr };
    _vector* m_vNLook{ nullptr };
    const _float4x4* m_pSocketMatrix = { nullptr };
    const _float4x4* m_pParentMatrix = { nullptr };
    _float4x4 m_WorldMatrix{};
    _bool m_bRightLeft = { false };
    _uint m_pParentState = {  };
public:
    static CBossBullet_Laser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END