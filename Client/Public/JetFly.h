#pragma once
#include "Client_Defines.h"
#include "Actor.h"
BEGIN(Engine)

END

BEGIN(Client)
class CMonsterHP;
class CJetFly final : public CActor
{
public:
    enum PARTOBJID
    {
        PART_BODY,
        PART_WEAPON,
        PART_EFFECT,
        PART_HP,
        PART_END
    };
    enum STATE
    {
     ST_BarrelRoll_Left,
     ST_BarrelRoll_Right,
     ST_Idle,
     ST_Shoot,
     ST_Sragger,
     ST_Walk_Back,
     ST_Walk_Front,
     ST_Walk_Left,
     ST_Walk_Right,
     ST_Hit_Back,
     ST_Hit_Front,
     ST_Hit_Left,
     ST_Hit_Right
    };                   

private:
    CJetFly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CJetFly(const CJetFly& Prototype);
    virtual ~CJetFly() = default;

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
    virtual void HIt_Routine()override;
    virtual void Dead_Routine(_float fTimeDelta) override;

    void NON_intersect(_float fTimedelta);



private:
    HRESULT Add_Components();
    HRESULT Add_PartObjects();
    CMonsterHP* m_pPartHP = { nullptr };

public:
    static CJetFly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
