#pragma once
#include "Client_Defines.h"
#include "Actor.h"
BEGIN(Engine)

END

BEGIN(Client)
class CMonsterHP;
class CBoomBot final : public CActor
{
public:
    enum PARTOBJID
    {
        PART_BODY,
        PART_WEAPON,
        PART_HP,
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

    virtual HRESULT Initialize_Prototype() override;
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
    HRESULT Bind_ShaderResources();

    CMonsterHP* m_pPartHP = { nullptr };

public:
    static CBoomBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
