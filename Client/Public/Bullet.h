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
    /* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
    virtual HRESULT Initialize_Prototype() override;

    /* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
    /* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
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