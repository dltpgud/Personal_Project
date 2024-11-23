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
 
    }CBossBullet_Berrle_DESC;
private:
    CBossBullet_Berrle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBossBullet_Berrle(const CBossBullet_Berrle& Prototype);
    virtual ~CBossBullet_Berrle() = default;

public:
    /* ���������� ȣ�� : ������ �ʿ��� ����� ���ſ� �۾����� �����Ѵ�.(��Ŷ, ���� �����) */
    virtual HRESULT Initialize_Prototype() override;

    /* ��Ŷ�̳� ���� ������� ���ؼ� �޾ƿ��� ���ϴ� �����鵵 �и��� �����Ѵ�. */
    /* �������� �����ϴ� �ʴ� �߰����� �ʱ�ȭ�� �ʿ��� ��� ȣ���Ѥ���. */
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
private:
    HRESULT Add_Components();

    HRESULT Bind_ShaderResources();
private:
    CModel* m_pModelCom = { nullptr };

private:
    _vector m_pTagetPos = {};
    _vector m_vDir{};

public:
    static CBossBullet_Berrle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END