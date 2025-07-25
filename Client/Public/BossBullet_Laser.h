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
        const _uint* state = { nullptr };
        _bool    bRightLeft{ nullptr };
        const _float4x4* pSocketMatrix = { nullptr };
        const _float4x4* pParentMatrix = { nullptr };

    } CBossBullet_Laser_DESC;
private:
    CBossBullet_Laser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBossBullet_Laser(const CBossBullet_Laser& Prototype);
    virtual ~CBossBullet_Laser() = default;

public:
    /* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
    virtual HRESULT Initialize_Prototype() override;

    /* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
    /* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
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
    const _uint* m_pParentState = { nullptr };
public:
    static CBossBullet_Laser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END