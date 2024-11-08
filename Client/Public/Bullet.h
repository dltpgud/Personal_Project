#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

END

BEGIN(Client)

class CBullet final : public CGameObject
{
public:
    enum PLATER_BULLET {
        TYPE_HENDGUN = 0,
        TYPE_ASSAULTRIFLE,
        TYPE_MISSILEGATLING,
        TYPE_HEAVYCROSSBOW
    };

    enum MONSTER_BULLET {
        TYPE_GUNPAWN =4,
        TYPE_JETFLY,
        TYPE_BOOMBOT,
        TYPE_MECANOBOT,
    };
public:
    typedef struct CBULLET_DESC : CGameObject::GAMEOBJ_DESC
    {
        _float* Fall_Y{};
        _float* Damage{};
        _vector pTagetPos{};
        _vector vPos{};
        _vector vPlayerAt{};
        _uint iWeaponType{};
        _float LifTime{};
        _bool bSturn{};
    }CBULLET_DESC;
private:
    CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBullet(const CBullet& Prototype);
    virtual ~CBullet() = default;

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
    virtual _float Get_Scalra_float() override;
private:
    HRESULT Add_Components();

private:
    _vector m_pTagetPos = {};
    _vector m_vPos{};
    _vector m_vPlayerAt{};
    _vector m_vDir{};
    _float m_fDeadTime{0.f};
    _float m_fLifeTime{ 0.f};
    _float* m_fFall_Y = { nullptr };
    _float* m_pDamage = { nullptr };
    _uint m_iWeaponType{ 0 };
public:
    static CBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END