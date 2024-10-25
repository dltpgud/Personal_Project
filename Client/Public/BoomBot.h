#pragma once
#include "Client_Defines.h"
#include "Actor.h"
BEGIN(Engine)

END

BEGIN(Client)

class CBoomBot final : public CActor
{
public:
    enum PARTOBJID
    {
        PART_BODY,
        PART_WEAPON,
        PART_EFFECT,
        PART_END
    };
    enum STATE
    {
     ST_Idle, 
     ST_PreShoot,
     ST_Run_Back,
     ST_Run_Front,
     ST_Run_Left,
     ST_Run_Left_Back,
     ST_Run_Right,
     ST_Run_Right_Back,
     ST_Shoot,
     ST_Stun_Loop,
     ST_Stun_Start,
     ST_Aim_Down,
     ST_Aim_Middle,
     ST_Aim_Down_Left,
     ST_Aim_Down_Right,
     ST_Aim_Left,
     ST_Aim_Right,
     ST_Aim_Top,
     ST_Aim_Top_Left,
     ST_Aim_Top_Right,
     ST_Hit_Back,
     ST_Hit_Front,
     ST_Hit_Left, 
     ST_Hit_Right
    };                   

private:
    CBoomBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBoomBot(const CBoomBot& Prototype);
    virtual ~CBoomBot() = default;

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
    virtual void HIt_Routine()override;
    virtual void Dead_Routine() override;
    void NON_intersect(_float fTimedelta);



private:
    HRESULT Add_Components();
    HRESULT Add_PartObjects();
    HRESULT Bind_ShaderResources();


public:
    static CBoomBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
