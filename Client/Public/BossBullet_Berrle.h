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
        const _uint* state = { nullptr };
        _bool LaserRightLeft{};
    }CBossBullet_Berrle_DESC;
private:
    CBossBullet_Berrle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBossBullet_Berrle(const CBossBullet_Berrle& Prototype);
    virtual ~CBossBullet_Berrle() = default;

public:
    /* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
    virtual HRESULT Initialize_Prototype() override;

    /* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
    /* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
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
   const _uint* m_pParentState = { nullptr };
public:
    static CBossBullet_Berrle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END